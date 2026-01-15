#include "config_loader.hpp"
#include "json_parser.hpp"
#include "input_validator.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#ifdef __APPLE__
#include "platform/macos/macos_keychain_impl.hpp"
#endif

namespace fs = std::filesystem;

ConfigLoader::ConfigLoader() {
}

ConfigLoader::~ConfigLoader() {
    stop_watching();
}

void ConfigLoader::start_watching(SettingsUpdateCallback callback) {
    if (watching_) {
        return;
    }

    std::string config_path = get_config_path();
    if (config_path.empty() || !fs::exists(config_path)) {
        return;
    }

    last_write_time_ = fs::last_write_time(config_path);
    watching_ = true;
    
    watcher_thread_ = std::thread([this, callback, config_path]() {
        while (watching_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            if (!watching_) break;

            try {
                if (fs::exists(config_path)) {
                    auto current_time = fs::last_write_time(config_path);
                    if (current_time > last_write_time_) {
                        last_write_time_ = current_time;
                        // Give file time to be fully written
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        Settings new_settings = load_settings();
                        if (callback) {
                            callback(new_settings);
                        }
                    }
                }
            } catch (const std::exception& e) {
                // Log error but continue watching
                // Using std::cerr since Logger might not be safe here if it depends on config
                std::cerr << "Error watching config file: " << e.what() << std::endl;
            }
        }
    });
}

void ConfigLoader::stop_watching() {
    watching_ = false;
    if (watcher_thread_.joinable()) {
        watcher_thread_.join();
    }
}

std::string ConfigLoader::get_config_path() {
    if (const char* env_path = std::getenv("NEO_SLACK_CONFIG_FILE")) {
        return std::string(env_path);
    }

#ifdef _WIN32
    const char* app_data = std::getenv("APPDATA");
    if (!app_data) {
        const char* user_profile = std::getenv("USERPROFILE");
        if (!user_profile) {
            return "";
        }
        fs::path config_dir = fs::path(user_profile) / "AppData" / "Roaming" / "neo-slack-bot";
        return (config_dir / "config.json").string();
    }
    fs::path config_dir = fs::path(app_data) / "neo-slack-bot";
    return (config_dir / "config.json").string();
#elif defined(__linux__)
    const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config) {
        fs::path config_dir = fs::path(xdg_config) / "neo-slack-bot";
        return (config_dir / "config.json").string();
    }
    
    const char* home = std::getenv("HOME");
    if (!home) {
        return "";
    }
    fs::path config_dir = fs::path(home) / ".config" / "neo-slack-bot";
    return (config_dir / "config.json").string();
#else // macOS and others
    const char* home = std::getenv("HOME");
    if (!home) {
        return "";
    }
    
    fs::path config_dir = fs::path(home) / "Library/Application Support/neo-slack-bot";
    return (config_dir / "config.json").string();
#endif
}

Settings ConfigLoader::load_settings() {
    Settings settings = Settings::get_defaults();
    
    std::string config_path = get_config_path();
    if (!config_path.empty() && fs::exists(config_path)) {
        load_from_file(settings, config_path);
    } else if (!config_path.empty()) {
        create_default_config(config_path);
    }
    
    load_from_env(settings);
    
    return settings;
}

void ConfigLoader::load_from_env(Settings& settings) {
    std::string val;
    
    // Load Tokens
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    if (bot_token) {
        settings.bot_token = bot_token;
    }
#ifdef __APPLE__
    else {
        auto secret = MacOSKeychainImpl::get_password("neo-slack-bot", "SLACK_BOT_TOKEN");
        if (secret) settings.bot_token = *secret;
    }

    if (!settings.bot_token.empty() &&
        InputValidator::is_valid_bot_token(settings.bot_token)) {
        MacOSKeychainImpl::set_password("neo-slack-bot", "SLACK_BOT_TOKEN", settings.bot_token);
    }
#endif

    const char* app_token = std::getenv("SLACK_APP_TOKEN");
    if (app_token) {
        settings.app_token = app_token;
    }
#ifdef __APPLE__
    else {
        auto secret = MacOSKeychainImpl::get_password("neo-slack-bot", "SLACK_APP_TOKEN");
        if (secret) settings.app_token = *secret;
    }

    if (!settings.app_token.empty() &&
        InputValidator::is_valid_app_token(settings.app_token)) {
        MacOSKeychainImpl::set_password("neo-slack-bot", "SLACK_APP_TOKEN", settings.app_token);
    }
#endif

    val = get_env("NEO_SLACK_NOTIFICATIONS_ENABLED");
    if (!val.empty()) {
        settings.notifications.enabled = (val == "1" || val == "true");
    }
    
    val = get_env("NEO_SLACK_NOTIFICATIONS_SOUND_ENABLED");
    if (!val.empty()) {
        settings.notifications.sound_enabled = (val == "1" || val == "true");
    }
    
    val = get_env("NEO_SLACK_CONNECTION_MAX_RETRIES");
    if (!val.empty()) {
        settings.connection.max_retries = std::stoi(val);
    }
    
    val = get_env("NEO_SLACK_CONNECTION_TIMEOUT_SECONDS");
    if (!val.empty()) {
        settings.connection.timeout_seconds = std::stoi(val);
    }
    
    val = get_env("NEO_SLACK_CONNECTION_PING_INTERVAL_SECONDS");
    if (!val.empty()) {
        settings.connection.ping_interval_seconds = std::stoi(val);
    }
    
    val = get_env("NEO_SLACK_CONNECTION_RETRY_DELAY_SECONDS");
    if (!val.empty()) {
        settings.connection.retry_delay_seconds = std::stoi(val);
    }
}

bool ConfigLoader::load_from_file(Settings& settings, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    file.close();
    
    if (json.empty()) {
        return false;
    }
    
    std::string notifications_json = json_parser::extract_string(json, "notifications");
    if (!notifications_json.empty()) {
        settings.notifications.enabled = json_parser::extract_bool(notifications_json, "enabled", settings.notifications.enabled);
        settings.notifications.sound_enabled = json_parser::extract_bool(notifications_json, "sound_enabled", settings.notifications.sound_enabled);
        
        std::string dnd_json = json_parser::extract_string(notifications_json, "dnd_hours");
        if (!dnd_json.empty()) {
            settings.notifications.dnd_hours.enabled = json_parser::extract_bool(dnd_json, "enabled", false);
            std::string start = json_parser::extract_string(dnd_json, "start");
            std::string end = json_parser::extract_string(dnd_json, "end");
            if (!start.empty()) settings.notifications.dnd_hours.start = start;
            if (!end.empty()) settings.notifications.dnd_hours.end = end;
        }
        
        std::string channel_filter_json = json_parser::extract_string(notifications_json, "channel_filter");
        if (!channel_filter_json.empty()) {
            std::string mode = json_parser::extract_string(channel_filter_json, "mode");
            if (mode == "whitelist") {
                settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
            } else if (mode == "blacklist") {
                settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
            } else {
                settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::ALL;
            }
            
            auto channels = json_parser::extract_string_array(channel_filter_json, "channels");
            for (const auto& ch : channels) {
                settings.notifications.channel_filter.channels.insert(ch);
            }
        }
    }
    
    std::string connection_json = json_parser::extract_string(json, "connection");
    if (!connection_json.empty()) {
        settings.connection.max_retries = json_parser::extract_int(connection_json, "max_retries", settings.connection.max_retries);
        settings.connection.timeout_seconds = json_parser::extract_int(connection_json, "timeout_seconds", settings.connection.timeout_seconds);
        settings.connection.ping_interval_seconds = json_parser::extract_int(connection_json, "ping_interval_seconds", settings.connection.ping_interval_seconds);
        settings.connection.retry_delay_seconds = json_parser::extract_int(connection_json, "retry_delay_seconds", settings.connection.retry_delay_seconds);
    }
    
    std::string events_json = json_parser::extract_string(json, "events");
    if (!events_json.empty()) {
        auto message_types = json_parser::extract_string_array(events_json, "message_types");
        for (const auto& type : message_types) {
            settings.events.message_types.insert(type);
        }
        
        std::string keyword_filter_json = json_parser::extract_string(events_json, "keyword_filter");
        if (!keyword_filter_json.empty()) {
            settings.events.keyword_filter.enabled = json_parser::extract_bool(keyword_filter_json, "enabled", false);
            std::string mode = json_parser::extract_string(keyword_filter_json, "mode");
            if (mode == "exclude") {
                settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::EXCLUDE;
            } else {
                settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
            }
            
            auto keywords = json_parser::extract_string_array(keyword_filter_json, "keywords");
            settings.events.keyword_filter.keywords = keywords;
        }
        
        std::string user_filter_json = json_parser::extract_string(events_json, "user_filter");
        if (!user_filter_json.empty()) {
            std::string mode = json_parser::extract_string(user_filter_json, "mode");
            if (mode == "whitelist") {
                settings.events.user_filter.mode = Settings::EventSettings::UserFilter::WHITELIST;
            } else if (mode == "blacklist") {
                settings.events.user_filter.mode = Settings::EventSettings::UserFilter::BLACKLIST;
            } else {
                settings.events.user_filter.mode = Settings::EventSettings::UserFilter::ALL;
            }
            
            auto users = json_parser::extract_string_array(user_filter_json, "users");
            for (const auto& user : users) {
                settings.events.user_filter.users.insert(user);
            }
        }
    }
    
    return true;
}


std::string ConfigLoader::get_env(const std::string& name) {
    const char* val = std::getenv(name.c_str());
    return val ? std::string(val) : "";
}
