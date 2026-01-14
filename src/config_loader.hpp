#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include "settings.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <filesystem>

/**
 * @brief Handles configuration loading and saving
 * 
 * Loads settings from:
 * 1. Default values
 * 2. Config file (~/.config/neo-slack-bot/config.json or similar)
 * 3. Environment variables (override config file)
 * 
 * Supports hot reload via file watching.
 */
class ConfigLoader {
public:
    using SettingsUpdateCallback = std::function<void(const Settings&)>;

    ConfigLoader();
    ~ConfigLoader();
    
    /**
     * @brief Load settings from all sources
     * @return Fully populated Settings object
     */
    Settings load_settings();
    
    /**
     * @brief Start watching config file for changes
     * @param callback Function to call when settings change
     */
    void start_watching(SettingsUpdateCallback callback);

    /**
     * @brief Stop watching config file
     */
    void stop_watching();

    /**
     * @brief Get path to configuration file
     * @return Absolute path to config file
     */
    static std::string get_config_path();
    
    /**
     * @brief Create default configuration file
     * @param path Path to config file
     * @return true if file was created successfully
     */
    static bool create_default_config(const std::string& path);
    
    /**
     * @brief Load settings from JSON config file (public for testing)
     * @param settings Settings object to populate
     * @param path Path to config file
     * @return true if file was loaded successfully
     */
    bool load_from_file(Settings& settings, const std::string& path);

private:
    std::thread watcher_thread_;
    std::atomic<bool> watching_{false};
    std::filesystem::file_time_type last_write_time_;

private:
    /**
     * @brief Load settings from environment variables
     * @param settings Settings object to populate
     */
    void load_from_env(Settings& settings);
    
    /**
     * @brief Get environment variable value
     * @param name Variable name
     * @return Value or empty string
     */
    std::string get_env(const std::string& name);
};

#endif // CONFIG_LOADER_HPP
