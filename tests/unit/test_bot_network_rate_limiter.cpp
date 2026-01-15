/**
 * @file test_bot_network_rate_limiter.cpp
 * @brief Unit tests for BotNetworkRateLimiter
 */

#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include "../../examples/bot_network_module/bot_network_rate_limiter.hpp"

using namespace std::chrono;
using namespace bot_network;

void test_bot_network_rate_limiter() {
    std::cout << "\n--- Bot Network Rate Limiter Tests ---" << std::endl;
    
    auto& limiter = BotNetworkRateLimiter::get_instance();
    limiter.reset();
    
    const std::string channel_id = "C1234567890";
    
    // Test 1: First request should be allowed immediately
    std::cout << "  Testing immediate first request..." << std::endl;
    auto start = steady_clock::now();
    bool allowed = limiter.wait_for_permission(channel_id);
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    assert(allowed);
    assert(elapsed < 100); // Should be immediate (< 100ms)
    limiter.record_sent(channel_id);
    std::cout << "  ✓ First request allowed immediately" << std::endl;
    
    // Test 2: Second request should wait minimum delay
    std::cout << "  Testing minimum delay enforcement..." << std::endl;
    start = steady_clock::now();
    allowed = limiter.wait_for_permission(channel_id);
    elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    assert(allowed);
    assert(elapsed >= 1100); // Should wait at least 1.1 seconds
    assert(elapsed < 1200); // But not too long
    limiter.record_sent(channel_id);
    std::cout << "  ✓ Minimum delay enforced (" << elapsed << "ms)" << std::endl;
    
    // Test 3: Rate limit handling
    std::cout << "  Testing rate limit handling..." << std::endl;
    limiter.handle_rate_limit(channel_id, 5); // 5 second rate limit
    start = steady_clock::now();
    allowed = limiter.wait_for_permission(channel_id);
    elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    assert(allowed);
    assert(elapsed >= 5000); // Should wait at least 5 seconds
    assert(elapsed < 5100);
    limiter.record_sent(channel_id);
    std::cout << "  ✓ Rate limit cooldown respected (" << elapsed << "ms)" << std::endl;
    
    // Test 4: Multiple channels independent
    std::cout << "  Testing multiple channel independence..." << std::endl;
    const std::string channel2 = "C0987654321";
    
    // Check channel2 (should be immediate)
    start = steady_clock::now();
    bool allowed2 = limiter.wait_for_permission(channel2);
    elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    assert(allowed2);
    assert(elapsed < 100); // Channel2 should be allowed immediately
    limiter.record_sent(channel2);
    
    // Check channel_id (should wait due to cooldown from previous test)
    start = steady_clock::now();
    bool allowed1 = limiter.wait_for_permission(channel_id);
    elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
    assert(allowed1);
    assert(elapsed >= 1000); // Should wait (approx 1.1s)
    limiter.record_sent(channel_id);
    
    std::cout << "  ✓ Multiple channels independent" << std::endl;
    
    // Test 5: Concurrent access (stress test)
    std::cout << "  Testing concurrent access..." << std::endl;
    const int num_threads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> success_count(0);
    std::atomic<int> total_waits(0);
    
    auto worker = [&](int /*thread_id*/) {
        for (int i = 0; i < 5; ++i) {
            auto wait_start = steady_clock::now();
            if (limiter.wait_for_permission(channel_id)) {
                auto wait_time = duration_cast<milliseconds>(steady_clock::now() - wait_start).count();
                total_waits += wait_time;
                limiter.record_sent(channel_id);
                success_count++;
            }
        }
    };
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(success_count == num_threads * 5);
    std::cout << "  ✓ Concurrent access handled correctly (" << success_count << " requests)" << std::endl;
    std::cout << "    Average wait time: " << (total_waits / success_count) << "ms" << std::endl;
    
    // Test 6: Shutdown
    std::cout << "  Testing shutdown..." << std::endl;
    limiter.shutdown();
    bool shutdown_allowed = limiter.wait_for_permission(channel_id);
    assert(!shutdown_allowed);
    limiter.reset();
    std::cout << "  ✓ Shutdown works correctly" << std::endl;
    
    std::cout << "All Bot Network Rate Limiter tests passed!" << std::endl;
}
