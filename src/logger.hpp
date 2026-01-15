#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <map>
#include <chrono>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERR = 3
};

enum class LogFormat {
    PLAIN,
    JSON
};

/**
 * @brief Thread-safe logging system with duplicate suppression
 * 
 * Thread Safety: Thread-safe. All public methods can be called concurrently
 * from multiple threads. Uses mutex for synchronization.
 */
class Logger {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to Logger instance
     */
    static Logger& get_instance();
    
    /**
     * @brief Set logging level
     * @param level Minimum log level to output
     */
    void set_level(LogLevel level);

    /**
     * @brief Set logging format
     * @param format Log format (PLAIN or JSON)
     */
    void set_format(LogFormat format);
    
    /**
     * @brief Set log file path
     * @param path Path to log file
     */
    void set_file_path(const std::string& path);
    
    /**
     * @brief Set maximum log file size
     * @param max_size_bytes Maximum file size in bytes before rotation
     */
    void set_max_file_size(size_t max_size_bytes);
    
    /**
     * @brief Enable or disable console output
     * @param enabled true to enable console output, false to disable
     */
    void set_console_output(bool enabled);
    
    /**
     * @brief Log debug message
     * @param message Message to log
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log info message
     * @param message Message to log
     */
    void info(const std::string& message);
    
    /**
     * @brief Log warning message
     * @param message Message to log
     */
    void warn(const std::string& message);
    
    /**
     * @brief Log error message
     * @param message Message to log
     */
    void error(const std::string& message);
    
    /**
     * @brief Log message with specified level
     * @param level Log level
     * @param message Message to log
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Destructor
     */
    ~Logger();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void rotate_log_if_needed();
    void write_log(LogLevel level, const std::string& message);
    std::string format_message(LogLevel level, const std::string& message);
    std::string level_to_string(LogLevel level);
    bool should_suppress_duplicate(const std::string& message, LogLevel level);
    
    struct DuplicateInfo {
        int count;
        std::chrono::steady_clock::time_point first_seen;
        std::chrono::steady_clock::time_point last_logged;
    };
    
    LogLevel current_level_;
    LogFormat log_format_;
    std::string log_file_path_;
    size_t max_file_size_;
    bool console_output_enabled_;
    std::unique_ptr<std::ofstream> log_file_;
    std::mutex log_mutex_;
    
    // Duplicate suppression: map of message hash to duplicate info
    std::map<std::string, DuplicateInfo> duplicate_tracker_;
    static constexpr int MAX_DUPLICATES_BEFORE_SUPPRESS = 3;
    static constexpr int SUPPRESS_WINDOW_SECONDS = 5;
    static constexpr int RESET_WINDOW_SECONDS = 300; // 5 minutes - longer reset window
};

#endif // LOGGER_HPP
