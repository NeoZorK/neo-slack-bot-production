#pragma once

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <string>

namespace bot_network {

/**
 * @brief Global rate limiter for coordinating multiple bot instances
 * 
 * Thread-safe singleton that ensures all bots respect Slack API rate limits
 * (typically 1 message per second per channel).
 */
class BotNetworkRateLimiter {
public:
    static BotNetworkRateLimiter& get_instance() {
        static BotNetworkRateLimiter instance;
        return instance;
    }
    
    /**
     * @brief Wait until it's safe to send a message
     * @param channel_id Channel ID (for per-channel rate limiting)
     * @return true if allowed to send, false if shutdown requested
     */
    bool wait_for_permission(const std::string& channel_id);
    
    /**
     * @brief Record that a message was sent
     * @param channel_id Channel ID
     */
    void record_sent(const std::string& channel_id);
    
    /**
     * @brief Handle rate limit error from API
     * @param channel_id Channel ID
     * @param retry_after_seconds Seconds to wait before retry
     */
    void handle_rate_limit(const std::string& channel_id, int retry_after_seconds);
    
    /**
     * @brief Shutdown rate limiter (wake all waiting threads)
     */
    void shutdown();
    
    /**
     * @brief Reset rate limiter state
     */
    void reset();

private:
    BotNetworkRateLimiter();
    ~BotNetworkRateLimiter() = default;
    BotNetworkRateLimiter(const BotNetworkRateLimiter&) = delete;
    BotNetworkRateLimiter& operator=(const BotNetworkRateLimiter&) = delete;
    
    struct ChannelState {
        std::chrono::steady_clock::time_point last_sent;
        std::chrono::steady_clock::time_point rate_limit_until;
        int consecutive_rate_limits;
    };
    
    std::mutex mutex_;
    std::condition_variable cv_;
    std::map<std::string, ChannelState> channel_states_;
    std::atomic<bool> shutdown_;
    
    static constexpr int MIN_DELAY_MS = 1100; // 1.1 seconds between messages (safety margin)
    static constexpr int MAX_BACKOFF_MS = 60000; // Max 60 seconds backoff
};

} // namespace bot_network
