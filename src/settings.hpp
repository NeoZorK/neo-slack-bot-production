#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <vector>
#include <set>
#include <chrono>

/**
 * @brief User settings structure for Neo Slack Bot
 * 
 * Contains all configurable settings organized by category:
 * - Notifications (enabled, sound, dnd hours, channel filter)
 * - Connection (retries, timeout, ping interval, retry delay)
 * - Events (message types, keyword filter, user filter)
 * 
 * Thread Safety: NOT thread-safe. Settings are typically loaded once at startup
 * and then read-only during runtime. External synchronization required if
 * modified from multiple threads.
 */
struct Settings {
    // Auth tokens (loaded from ENV or Keychain)
    std::string bot_token;
    std::string app_token;

    // Notification settings
    struct NotificationSettings {
        bool enabled = true;
        bool sound_enabled = true;
        
        struct DndHours {
            bool enabled = false;
            std::string start = "22:00";
            std::string end = "08:00";
        } dnd_hours;
        
        struct ChannelFilter {
            enum Mode { ALL, WHITELIST, BLACKLIST };
            Mode mode = ALL;
            std::set<std::string> channels;
        } channel_filter;
    } notifications;
    
    // Connection settings
    struct ConnectionSettings {
        int max_retries = 10;
        int timeout_seconds = 30;
        int ping_interval_seconds = 30;
        int retry_delay_seconds = 1;
    } connection;
    
    // Event filtering settings
    struct EventSettings {
        std::set<std::string> message_types;
        
        struct KeywordFilter {
            bool enabled = false;
            enum Mode { INCLUDE, EXCLUDE };
            Mode mode = INCLUDE;
            std::vector<std::string> keywords;
        } keyword_filter;
        
        struct UserFilter {
            enum Mode { ALL, WHITELIST, BLACKLIST };
            Mode mode = ALL;
            std::set<std::string> users;
        } user_filter;
    } events;
    
    /**
     * @brief Check if current time is within DND hours
     * @return true if DND is enabled and current time is in DND range
     */
    bool is_dnd_active() const;
    
    /**
     * @brief Get default settings
     * @return Settings object with default values
     */
    static Settings get_defaults();
};

#endif // SETTINGS_HPP
