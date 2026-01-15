#include "logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <functional>

namespace {
    constexpr size_t DEFAULT_MAX_FILE_SIZE = 10 * 1024 * 1024; // 10 MB
    
    // Simple hash function for message deduplication
    std::string hash_message(const std::string& message) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(message));
    }

    // Helper to escape JSON string
    std::string json_escape(const std::string& s) {
        std::ostringstream o;
        for (char c : s) {
            switch (c) {
                case '"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b"; break;
                case '\f': o << "\\f"; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        o << "\\u"
                          << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }
}

Logger::Logger() 
    : current_level_(LogLevel::INFO),
      log_format_(LogFormat::PLAIN),
      max_file_size_(DEFAULT_MAX_FILE_SIZE),
      console_output_enabled_(true) {
}

Logger::~Logger() {
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
}

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    current_level_ = level;
}

void Logger::set_format(LogFormat format) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_format_ = format;
}

void Logger::set_file_path(const std::string& path) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
    log_file_path_ = path;
    
    if (path.empty()) {
        log_file_.reset();
        return;
    }
    
    log_file_ = std::make_unique<std::ofstream>(log_file_path_, std::ios::app);
}

void Logger::set_max_file_size(size_t max_size_bytes) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    max_file_size_ = max_size_bytes;
}

void Logger::set_console_output(bool enabled) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    console_output_enabled_ = enabled;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < current_level_) {
        return;
    }
    write_log(level, message);
}

bool Logger::should_suppress_duplicate(const std::string& message, LogLevel level) {
    // Suppress duplicates for ERROR and WARN level messages
    if (level < LogLevel::WARN) {
        return false;
    }
    
    auto now = std::chrono::steady_clock::now();
    std::string msg_hash = hash_message(message);
    
    auto it = duplicate_tracker_.find(msg_hash);
    if (it == duplicate_tracker_.end()) {
        // First time seeing this message
        duplicate_tracker_[msg_hash] = {1, now, now};
        return false;
    }
    
    DuplicateInfo& info = it->second;
    auto time_since_first = std::chrono::duration_cast<std::chrono::seconds>(
        now - info.first_seen).count();
    auto time_since_last_log = std::chrono::duration_cast<std::chrono::seconds>(
        now - info.last_logged).count();
    
    // Reset counter if enough time has passed (but keep suppression active)
    if (time_since_first > RESET_WINDOW_SECONDS) {
        // Reset but start suppressing immediately if we've seen many duplicates before
        int old_count = info.count;
        info.count = 1;
        info.first_seen = now;
        info.last_logged = now;
        // If we had many duplicates before reset, start suppressing immediately
        if (old_count > MAX_DUPLICATES_BEFORE_SUPPRESS * 2) {
            info.count = MAX_DUPLICATES_BEFORE_SUPPRESS + 1; // Start in suppressed state
            return true;
        }
        return false;
    }
    
    info.count++;
    
    // Log first few occurrences, then suppress
    if (info.count <= MAX_DUPLICATES_BEFORE_SUPPRESS) {
        info.last_logged = now;
        return false;
    }
    
    // Suppress if logged recently, but log summary every SUPPRESS_WINDOW_SECONDS
    if (time_since_last_log >= SUPPRESS_WINDOW_SECONDS) {
        info.last_logged = now;
        // Log summary message
        std::string summary = message + " (suppressed " + 
                             std::to_string(info.count - MAX_DUPLICATES_BEFORE_SUPPRESS) + 
                             " duplicate(s))";
        std::string formatted = format_message(level, summary);
        if (console_output_enabled_) {
            if (level >= LogLevel::ERR) {
                std::cerr << formatted << std::endl;
            } else {
                std::cout << formatted << std::endl;
            }
        }
        if (!log_file_path_.empty() && log_file_ && log_file_->is_open()) {
            *log_file_ << formatted << std::endl;
            log_file_->flush();
        }
    }
    
    return true;
}

void Logger::write_log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    // Check if we should suppress this duplicate error
    if (should_suppress_duplicate(message, level)) {
        return;
    }
    
    std::string formatted = format_message(level, message);
    
    if (console_output_enabled_) {
        if (level >= LogLevel::ERR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    }
    
    if (!log_file_path_.empty()) {
        rotate_log_if_needed();
        if (log_file_ && log_file_->is_open()) {
            *log_file_ << formatted << std::endl;
            log_file_->flush();
        }
    }
}

std::string Logger::format_message(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    
    if (log_format_ == LogFormat::JSON) {
        oss << "{"
            << "\"ts\":\"" << std::put_time(std::localtime(&time_t), "%Y-%m-%dT%H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count() << "\","
            << "\"level\":\"" << std::string(level_to_string(level)).substr(0, 5) << "\"," // Trim whitespace
            << "\"message\":\"" << json_escape(message) << "\""
            << "}";
    } else {
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        oss << " [" << level_to_string(level) << "] " << message;
    }
    
    return oss.str();
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::rotate_log_if_needed() {
    if (log_file_path_.empty()) return;
    
    if (std::filesystem::exists(log_file_path_)) {
        size_t file_size = std::filesystem::file_size(log_file_path_);
        if (file_size >= max_file_size_) {
            if (log_file_ && log_file_->is_open()) {
                log_file_->close();
            }
            
            std::string backup_path = log_file_path_ + ".old";
            if (std::filesystem::exists(backup_path)) {
                std::filesystem::remove(backup_path);
            }
            std::filesystem::rename(log_file_path_, backup_path);
            
            log_file_ = std::make_unique<std::ofstream>(log_file_path_, std::ios::app);
        }
    }
}
