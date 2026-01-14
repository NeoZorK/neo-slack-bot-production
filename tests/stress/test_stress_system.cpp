#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#ifdef __APPLE__
#include "../src/macos_notifications.hpp"
#endif
#include "../src/logger.hpp"
#include "../src/error_recovery.hpp"
#include "../src/rate_limiter.hpp"

using namespace std::chrono;
using namespace std;

void stress_test_notification_burst(int notification_count) {
#ifdef __APPLE__
    MacOSNotifications notifications;
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < notification_count; ++i) {
        notifications.show_notification(
            "Stress Test",
            "Notification #" + std::to_string(i)
        );
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Notification burst stress test:" << std::endl;
    std::cout << "  Notifications: " << notification_count << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms (" << duration_us << " us)" << std::endl;
    if (duration_us > 0) {
        std::cout << "  Rate: " << (notification_count * 1000000.0 / duration_us) 
                  << " notifications/sec" << std::endl;
    } else {
        std::cout << "  Rate: > 1M notifications/sec (extremely fast)" << std::endl;
    }
    
    assert(duration_us >= 0);
#else
    (void)notification_count;
    std::cout << "Notification burst stress test skipped on non-macOS platform" << std::endl;
#endif
}

void stress_test_long_running(int duration_seconds) {
#ifdef __APPLE__
    MacOSNotifications notifications;
    auto start = high_resolution_clock::now();
    auto end_time = start + seconds(duration_seconds);
    int notification_count = 0;
    
    while (high_resolution_clock::now() < end_time) {
        notifications.show_notification(
            "Long Running Test",
            "Notification #" + std::to_string(notification_count++)
        );
        std::this_thread::sleep_for(milliseconds(100));
    }
    
    auto actual_duration = duration_cast<seconds>(
        high_resolution_clock::now() - start
    ).count();
    
    std::cout << "Long running stress test:" << std::endl;
    std::cout << "  Target duration: " << duration_seconds << " seconds" << std::endl;
    std::cout << "  Actual duration: " << actual_duration << " seconds" << std::endl;
    std::cout << "  Notifications sent: " << notification_count << std::endl;
    
    assert(actual_duration >= duration_seconds - 1);
#else
    (void)duration_seconds;
    std::cout << "Long running stress test skipped on non-macOS platform" << std::endl;
#endif
}

void stress_test_logger(int iterations) {
    Logger& logger = Logger::get_instance();
    logger.set_level(LogLevel::INFO);
    logger.set_console_output(false);
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        logger.info("Stress test message " + std::to_string(i));
        logger.debug("Debug message " + std::to_string(i));
        logger.warn("Warning message " + std::to_string(i));
        logger.error("Error message " + std::to_string(i));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Logger stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    if (duration > 0) {
        std::cout << "  Rate: " << (iterations * 1000.0 / duration) << " logs/sec" << std::endl;
    }
    
    logger.set_console_output(true);
    assert(duration >= 0);
}

void stress_test_error_recovery(int iterations) {
    ErrorRecovery recovery;
    recovery.set_max_retries(10);
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        recovery.handle_error(ErrorType::API_ERROR, "Test error " + std::to_string(i));
        recovery.should_retry(ErrorType::API_ERROR);
        recovery.get_retry_delay(ErrorType::API_ERROR, i % 10);
        if (i % 100 == 0) {
            recovery.reset_error_count(ErrorType::API_ERROR);
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "ErrorRecovery stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    if (duration > 0) {
        std::cout << "  Rate: " << (iterations * 1000.0 / duration) << " operations/sec" << std::endl;
    }
    
    assert(duration >= 0);
}

void stress_test_rate_limiter(int iterations) {
    RateLimiter limiter;
    
    auto start = high_resolution_clock::now();
    
    int allowed = 0;
    int blocked = 0;
    
    for (int i = 0; i < iterations; ++i) {
        if (limiter.can_make_request()) {
            limiter.record_request();
            allowed++;
        } else {
            blocked++;
        }
        if (i % 100 == 0) {
            limiter.reset();
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "RateLimiter stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Allowed: " << allowed << std::endl;
    std::cout << "  Blocked: " << blocked << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    if (duration > 0) {
        std::cout << "  Rate: " << (iterations * 1000.0 / duration) << " checks/sec" << std::endl;
    }
    
    assert(duration >= 0);
}

void test_stress_system() {
    stress_test_notification_burst(100);
    stress_test_logger(10000);
    stress_test_error_recovery(10000);
    stress_test_rate_limiter(10000);
}
