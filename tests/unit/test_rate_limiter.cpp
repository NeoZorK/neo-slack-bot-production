#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include "../src/rate_limiter.hpp"

void test_rate_limiter_basic() {
    RateLimiter limiter;
    
    assert(limiter.can_make_request());
    limiter.record_request();
    assert(limiter.can_make_request());
    
    std::cout << "  ✓ Rate limiter basic test passed" << std::endl;
}

void test_rate_limiter_rate_limit_detection() {
    RateLimiter limiter;
    
    std::string rate_limit_response = R"({"ok":false,"error":"rate_limited","retry_after":30})";
    limiter.handle_rate_limit_response(rate_limit_response);
    
    assert(limiter.is_rate_limited());
    assert(limiter.get_retry_after_seconds() > 0);
    
    std::cout << "  ✓ Rate limiter rate limit detection test passed" << std::endl;
}

void test_rate_limiter_window() {
    RateLimiter limiter;
    
    for (int i = 0; i < 20; ++i) {
        assert(limiter.can_make_request());
        limiter.record_request();
    }
    
    assert(!limiter.can_make_request());
    
    std::cout << "  ✓ Rate limiter window test passed" << std::endl;
}

void test_rate_limiter_reset() {
    RateLimiter limiter;
    
    std::string rate_limit_response = R"({"ok":false,"error":"rate_limited","retry_after":30})";
    limiter.handle_rate_limit_response(rate_limit_response);
    assert(limiter.is_rate_limited());
    
    limiter.reset();
    assert(!limiter.is_rate_limited());
    
    std::cout << "  ✓ Rate limiter reset test passed" << std::endl;
}

void test_rate_limiter_thread_safety() {
    RateLimiter limiter;
    std::vector<std::thread> threads;
    const int thread_count = 10;
    const int requests_per_thread = 5;
    std::atomic<int> success_count{0};
    std::atomic<int> blocked_count{0};
    
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&limiter, &success_count, &blocked_count]() {
            for (int j = 0; j < requests_per_thread; ++j) {
                if (limiter.can_make_request()) {
                    limiter.record_request();
                    success_count++;
                } else {
                    blocked_count++;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(success_count.load() > 0);
    
    std::cout << "  ✓ Rate limiter thread safety test passed" << std::endl;
}

void test_rate_limiter() {
    std::cout << "Testing RateLimiter..." << std::endl;
    
    test_rate_limiter_basic();
    test_rate_limiter_rate_limit_detection();
    test_rate_limiter_window();
    test_rate_limiter_reset();
    test_rate_limiter_thread_safety();
    
    std::cout << "RateLimiter tests passed!" << std::endl;
}
