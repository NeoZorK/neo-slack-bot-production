#include "error_recovery.hpp"
#include "logger.hpp"
#include <algorithm>
#include <cmath>
#include <mutex>

ErrorRecovery::ErrorRecovery() 
    : max_retries_(3),
      initial_retry_delay_seconds_(1) {
    for (int i = 0; i < 7; ++i) {
        error_states_[i].count = 0;
        error_states_[i].last_occurrence = std::chrono::steady_clock::now();
        error_states_[i].strategy = RecoveryStrategy::EXPONENTIAL_BACKOFF;
    }
    
    set_recovery_strategy(ErrorType::AUTHENTICATION_ERROR, RecoveryStrategy::FAIL_FAST);
    set_recovery_strategy(ErrorType::PARSING_ERROR, RecoveryStrategy::IGNORE);
}

void ErrorRecovery::handle_error(ErrorType type, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    ErrorState& state = get_error_state(type);
    state.count++;
    state.last_occurrence = std::chrono::steady_clock::now();
    
    Logger::get_instance().error("Error [" + std::to_string(static_cast<int>(type)) + 
                                  "]: " + message + " (count: " + 
                                  std::to_string(state.count) + ")");
}

void ErrorRecovery::set_recovery_strategy(ErrorType type, RecoveryStrategy strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    ErrorState& state = get_error_state(type);
    state.strategy = strategy;
}

void ErrorRecovery::set_max_retries(int max_retries) {
    std::lock_guard<std::mutex> lock(mutex_);
    max_retries_ = max_retries;
}

void ErrorRecovery::set_initial_retry_delay(int seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    initial_retry_delay_seconds_ = seconds;
}

bool ErrorRecovery::should_retry(ErrorType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const ErrorState& state = error_states_[static_cast<int>(type)];
    
    if (state.count > max_retries_) {
        return false;
    }
    
    RecoveryStrategy strategy = get_strategy_for_error(type);
    switch (strategy) {
        case RecoveryStrategy::RETRY:
        case RecoveryStrategy::EXPONENTIAL_BACKOFF:
            return state.count <= max_retries_;
        case RecoveryStrategy::FAIL_FAST:
            return false;
        case RecoveryStrategy::IGNORE:
            return true;
        default:
            return false;
    }
}

int ErrorRecovery::get_retry_delay(ErrorType type, int attempt_number) const {
    std::lock_guard<std::mutex> lock(mutex_);
    RecoveryStrategy strategy = get_strategy_for_error(type);
    
    switch (strategy) {
        case RecoveryStrategy::EXPONENTIAL_BACKOFF: {
            int delay = initial_retry_delay_seconds_ * (1 << attempt_number);
            return std::min(delay, 60);
        }
        case RecoveryStrategy::RETRY:
            return initial_retry_delay_seconds_;
        default:
            return 0;
    }
}

void ErrorRecovery::reset_error_count(ErrorType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    ErrorState& state = get_error_state(type);
    state.count = 0;
}

int ErrorRecovery::get_error_count(ErrorType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return error_states_[static_cast<int>(type)].count;
}

ErrorRecovery::ErrorState& ErrorRecovery::get_error_state(ErrorType type) {
    // Note: This method assumes mutex is already locked by caller
    return error_states_[static_cast<int>(type)];
}

RecoveryStrategy ErrorRecovery::get_strategy_for_error(ErrorType type) const {
    // Note: This method assumes mutex is already locked by caller
    return error_states_[static_cast<int>(type)].strategy;
}
