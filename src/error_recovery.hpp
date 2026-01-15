#ifndef ERROR_RECOVERY_HPP
#define ERROR_RECOVERY_HPP

#include <string>
#include <functional>
#include <chrono>
#include <memory>
#include <mutex>

enum class ErrorType {
    NETWORK_ERROR,
    AUTHENTICATION_ERROR,
    API_ERROR,
    PARSING_ERROR,
    SYSTEM_ERROR,
    RATE_LIMIT_ERROR,
    UNKNOWN_ERROR
};

enum class RecoveryStrategy {
    RETRY,
    EXPONENTIAL_BACKOFF,
    FAIL_FAST,
    IGNORE
};

/**
 * @brief Error recovery and retry management system
 * 
 * Thread Safety: Thread-safe. All public methods are protected by mutex.
 * Multiple threads can safely call methods concurrently.
 */
class ErrorRecovery {
public:
    /**
     * @brief Construct error recovery system
     */
    ErrorRecovery();
    
    /**
     * @brief Handle error occurrence
     * @param type Type of error
     * @param message Error message
     */
    void handle_error(ErrorType type, const std::string& message);
    
    /**
     * @brief Set recovery strategy for error type
     * @param type Error type
     * @param strategy Recovery strategy to use
     */
    void set_recovery_strategy(ErrorType type, RecoveryStrategy strategy);
    
    /**
     * @brief Set maximum number of retries
     * @param max_retries Maximum retry attempts
     */
    void set_max_retries(int max_retries);
    
    /**
     * @brief Set initial retry delay
     * @param seconds Initial delay in seconds
     */
    void set_initial_retry_delay(int seconds);
    
    /**
     * @brief Check if error should be retried
     * @param type Error type to check
     * @return true if should retry, false otherwise
     */
    bool should_retry(ErrorType type) const;
    
    /**
     * @brief Get retry delay for attempt number
     * @param type Error type to calculate delay for
     * @param attempt_number Attempt number (1-based)
     * @return Delay in seconds
     */
    int get_retry_delay(ErrorType type, int attempt_number) const;
    
    /**
     * @brief Reset error count for error type
     * @param type Error type to reset
     */
    void reset_error_count(ErrorType type);
    
    /**
     * @brief Get error count for error type
     * @param type Error type to query
     * @return Number of errors of this type
     */
    int get_error_count(ErrorType type) const;

private:
    struct ErrorState {
        int count;
        std::chrono::steady_clock::time_point last_occurrence;
        RecoveryStrategy strategy;
    };
    
    ErrorState& get_error_state(ErrorType type);
    RecoveryStrategy get_strategy_for_error(ErrorType type) const;
    
    ErrorState error_states_[7];
    int max_retries_;
    int initial_retry_delay_seconds_;
    mutable std::mutex mutex_;
};

#endif // ERROR_RECOVERY_HPP
