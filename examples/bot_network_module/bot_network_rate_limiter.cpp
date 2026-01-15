#include "bot_network_rate_limiter.hpp"
#include <algorithm>
#include <thread>

namespace bot_network {

BotNetworkRateLimiter::BotNetworkRateLimiter()
    : shutdown_(false) {
}

bool BotNetworkRateLimiter::wait_for_permission(const std::string& channel_id) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    while (!shutdown_.load()) {
        auto now = std::chrono::steady_clock::now();
        auto& state = channel_states_[channel_id];
        
        // Initialize last_sent if this is first access (use epoch to allow immediate first send)
        if (state.last_sent.time_since_epoch().count() == 0) {
            state.last_sent = now - std::chrono::milliseconds(MIN_DELAY_MS + 100); // Allow immediate first send
        }
        
        // Check if we're in rate limit cooldown
        if (now < state.rate_limit_until) {
            auto wait_time = state.rate_limit_until - now;
            cv_.wait_for(lock, wait_time);
            continue;
        }
        
        // Check minimum delay since last message
        auto time_since_last = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - state.last_sent).count();
        
        if (time_since_last >= MIN_DELAY_MS) {
            // We can send now - update last_sent immediately to prevent race condition
            state.last_sent = now;
            return true;
        }
        
        // Wait until minimum delay has passed
        auto wait_ms = MIN_DELAY_MS - time_since_last;
        cv_.wait_for(lock, std::chrono::milliseconds(wait_ms));
    }
    
    return false; // Shutdown requested
}

void BotNetworkRateLimiter::record_sent(const std::string& channel_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& state = channel_states_[channel_id];
    // Note: last_sent is already updated in wait_for_permission to prevent race conditions
    // This method is kept for explicit recording if needed
    state.consecutive_rate_limits = 0; // Reset on success
    cv_.notify_all(); // Wake up waiting threads
}

void BotNetworkRateLimiter::handle_rate_limit(const std::string& channel_id, int retry_after_seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& state = channel_states_[channel_id];
    
    state.consecutive_rate_limits++;
    
    // Exponential backoff: retry_after + (2^consecutive_failures - 1) seconds
    int backoff_seconds = retry_after_seconds;
    if (state.consecutive_rate_limits > 1) {
        int extra_backoff = (1 << (state.consecutive_rate_limits - 1)) - 1;
        backoff_seconds += std::min(extra_backoff, 30); // Cap at 30 extra seconds
    }
    
    auto now = std::chrono::steady_clock::now();
    state.rate_limit_until = now + std::chrono::seconds(backoff_seconds);
    
    cv_.notify_all(); // Wake up waiting threads
}

void BotNetworkRateLimiter::shutdown() {
    shutdown_.store(true);
    std::lock_guard<std::mutex> lock(mutex_);
    cv_.notify_all();
}

void BotNetworkRateLimiter::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_states_.clear();
    shutdown_.store(false);
    cv_.notify_all();
}

} // namespace bot_network
