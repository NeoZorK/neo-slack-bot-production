#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include "../src/config_loader.hpp"
#include "../src/settings.hpp"

namespace fs = std::filesystem;

void test_config_loader_get_config_path() {
    std::string path = ConfigLoader::get_config_path();
    std::cout << "Debug: path='" << path << "'" << std::endl;
#ifdef _WIN32
    std::cout << "Debug: _WIN32 is defined" << std::endl;
    if (const char* env = std::getenv("APPDATA")) std::cout << "Debug: APPDATA=" << env << std::endl;
    else std::cout << "Debug: APPDATA is null" << std::endl;
#else
    std::cout << "Debug: _WIN32 is NOT defined" << std::endl;
    if (const char* env = std::getenv("HOME")) std::cout << "Debug: HOME=" << env << std::endl;
    else std::cout << "Debug: HOME is null" << std::endl;
#endif

    assert(!path.empty());
    
#ifdef _WIN32
    assert(path.find("neo-slack-bot\\config.json") != std::string::npos || 
           path.find("neo-slack-bot/config.json") != std::string::npos);
#elif defined(__linux__)
    assert(path.find(".config/neo-slack-bot/config.json") != std::string::npos);
#else
    assert(path.find("Library/Application Support/neo-slack-bot/config.json") != std::string::npos);
#endif
}

void test_config_loader_create_default_config() {
    std::string test_path = "/tmp/neo-slack-bot-test/config.json";
    
    if (fs::exists(test_path)) {
        fs::remove(test_path);
    }
    if (fs::exists(fs::path(test_path).parent_path())) {
        fs::remove_all(fs::path(test_path).parent_path());
    }
    
    bool created = ConfigLoader::create_default_config(test_path);
    assert(created);
    assert(fs::exists(test_path));
    
    std::ifstream file(test_path);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    assert(content.find("notifications") != std::string::npos);
    assert(content.find("connection") != std::string::npos);
    assert(content.find("events") != std::string::npos);
    
    file.close();
    fs::remove_all(fs::path(test_path).parent_path());
}

void test_config_loader_load_defaults() {
    ConfigLoader loader;
    Settings settings = loader.load_settings();
    
    assert(settings.notifications.enabled == true);
    assert(settings.connection.max_retries == 10);
    assert(settings.connection.timeout_seconds == 30);
}

void test_config_loader_load_from_file() {
    std::string test_path = "/tmp/neo-slack-bot-test/config.json";
    fs::create_directories(fs::path(test_path).parent_path());
    
    std::ofstream file(test_path);
    file << R"({"notifications":{"enabled":false,"sound_enabled":false},"connection":{"max_retries":5}})";
    file.close();
    
    ConfigLoader loader;
    Settings settings = Settings::get_defaults();
    
    bool loaded = loader.load_from_file(settings, test_path);
    assert(loaded);
    assert(settings.notifications.enabled == false);
    assert(settings.notifications.sound_enabled == false);
    assert(settings.connection.max_retries == 5);
    
    fs::remove_all(fs::path(test_path).parent_path());
}

void test_config_loader_env_overrides() {
    std::string test_path = "/tmp/neo-slack-bot-test/config.json";
    fs::create_directories(fs::path(test_path).parent_path());
    
    std::ofstream file(test_path);
    file << R"({"notifications":{"enabled":false},"connection":{"max_retries":5}})";
    file.close();
    
    std::string env1_name = "NEO_SLACK_NOTIFICATIONS_ENABLED";
    std::string env2_name = "NEO_SLACK_CONNECTION_MAX_RETRIES";
#ifdef _WIN32
    _putenv((env1_name + "=1").c_str());
    _putenv((env2_name + "=20").c_str());
#else
    setenv(env1_name.c_str(), "1", 1);
    setenv(env2_name.c_str(), "20", 1);
#endif
    
    ConfigLoader loader;
    Settings settings = loader.load_settings();
    
    assert(settings.notifications.enabled == true);
    assert(settings.connection.max_retries == 20);
    
#ifdef _WIN32
    _putenv((env1_name + "=").c_str());
    _putenv((env2_name + "=").c_str());
#else
    unsetenv(env1_name.c_str());
    unsetenv(env2_name.c_str());
#endif
    
    fs::remove_all(fs::path(test_path).parent_path());
}

void test_config_loader_negative_scenarios() {
    Settings settings = Settings::get_defaults();
    ConfigLoader loader;
    
    // 1. File does not exist
    bool loaded = loader.load_from_file(settings, "/non/existent/path/config.json");
    assert(!loaded);
    
    // 2. Empty file
    std::string test_path = "/tmp/neo-slack-bot-test/empty_config.json";
    fs::create_directories(fs::path(test_path).parent_path());
    std::ofstream file(test_path);
    file.close(); // Empty
    
    loaded = loader.load_from_file(settings, test_path);
    assert(!loaded);
    
    // 3. Invalid JSON content
    std::ofstream file2(test_path);
    file2 << "This is not JSON";
    file2.close();
    
    Settings s2 = Settings::get_defaults();
    loaded = loader.load_from_file(s2, test_path);
    // Should return true (file read success) but settings unchanged
    assert(loaded);
    assert(s2.notifications.enabled == true);
    
    // 4. Permission denied (or invalid path) for creation
    bool created = loader.create_default_config("/sys/kernel/config.json"); 
    assert(!created);
    
    fs::remove_all(fs::path(test_path).parent_path());
    
    std::cout << "  ✓ ConfigLoader negative tests passed" << std::endl;
}

void test_config_loader_partial_json() {
    std::string test_path = "/tmp/neo-slack-bot-test/partial.json";
    fs::create_directories(fs::path(test_path).parent_path());
    
    std::ofstream file(test_path);
    file << R"({"connection":{"max_retries":99}})";
    file.close();
    
    ConfigLoader loader;
    Settings settings = Settings::get_defaults();
    
    bool loaded = loader.load_from_file(settings, test_path);
    assert(loaded);
    assert(settings.notifications.enabled == true); // default untouched
    assert(settings.connection.max_retries == 99); // updated
    
    fs::remove_all(fs::path(test_path).parent_path());
    
    std::cout << "  ✓ ConfigLoader partial JSON test passed" << std::endl;
}

void test_config_loader() {
    test_config_loader_get_config_path();
    test_config_loader_create_default_config();
    test_config_loader_load_defaults();
    test_config_loader_load_from_file();
    test_config_loader_negative_scenarios();
    test_config_loader_partial_json();
    test_config_loader_env_overrides();
    std::cout << "All config loader tests passed!" << std::endl;
}
