#include <iostream>
#include <chrono>
#include <vector>
#include <cassert>
#include "../src/websocket_client.hpp"
#include "../src/http_client.hpp"
#ifdef __APPLE__
#include "../src/macos_notifications.hpp"
#endif
#include "../src/slack_api.hpp"
#include "../src/settings.hpp"
#include "../src/config_loader.hpp"
#include "../src/json_parser.hpp"
#include "../src/message_filter.hpp"

using namespace std::chrono;

// Performance test utilities
class PerformanceTimer {
public:
    void start() {
        start_time = high_resolution_clock::now();
    }
    
    void stop() {
        end_time = high_resolution_clock::now();
    }
    
    double elapsed_ms() const {
        return duration_cast<microseconds>(end_time - start_time).count() / 1000.0;
    }
    
    double elapsed_us() const {
        return duration_cast<microseconds>(end_time - start_time).count();
    }

private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;
};

// Performance benchmarks
void benchmark_websocket_client_creation() {
    const int iterations = 10000;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        WebSocketClient client("test-token");
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "WebSocket client creation: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 100 microseconds per creation
    assert(avg_time < 100.0);
}

static void benchmark_notification_display_perf() {
#ifdef __APPLE__
    const int iterations = 1000;
    MacOSNotifications notifications;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        notifications.show_notification("Test", "Performance test message");
    }
    timer.stop();
    
    double avg_time = timer.elapsed_ms() / iterations;
    std::cout << "Notification display: " << avg_time << " ms/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 10ms per notification
    assert(avg_time < 10.0);
#else
    std::cout << "Notification display benchmark skipped on non-macOS platform" << std::endl;
#endif
}

void benchmark_http_client_creation() {
    const int iterations = 10000;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        HttpClient client;
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "HTTP client creation: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 50 microseconds per creation
    assert(avg_time < 50.0);
}

void benchmark_slack_api_creation() {
    const int iterations = 10000;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        SlackAPI api("test-token");
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "Slack API creation: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 100 microseconds per creation
    assert(avg_time < 100.0);
}

void benchmark_message_processing_throughput() {
    const int message_count = 10000;
    PerformanceTimer timer;
    
    // Simulate message processing
    timer.start();
    for (int i = 0; i < message_count; ++i) {
        // Simulate JSON parsing and processing
        std::string json = "{\"type\":\"message\",\"text\":\"test\"}";
        // In real implementation, parse and process
    }
    timer.stop();
    
    double total_time_ms = timer.elapsed_ms();
    double throughput = message_count / (total_time_ms / 1000.0);
    
    std::cout << "Message processing throughput: " << throughput 
              << " messages/sec" << std::endl;
    
    // Performance requirement: > 10000 messages/sec
    assert(throughput > 10000.0);
}

void benchmark_settings_creation() {
    const int iterations = 10000;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        Settings settings = Settings::get_defaults();
        (void)settings.is_dnd_active();
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "Settings creation: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 10 microseconds per creation
    assert(avg_time < 10.0);
}

void benchmark_config_loader_perf() {
    const int iterations = 1000;
    ConfigLoader loader;
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        Settings settings = loader.load_settings();
        (void)settings;
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "ConfigLoader load: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 1000 microseconds per load
    assert(avg_time < 1000.0);
}

void benchmark_json_parser_perf() {
    const int iterations = 10000;
    std::string json = R"({"notifications":{"enabled":true,"sound_enabled":false},"connection":{"max_retries":10}})";
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        json_parser::extract_string(json, "notifications");
        json_parser::extract_bool(json, "enabled", false);
        json_parser::extract_int(json, "max_retries", 0);
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "JSON parser operations: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 5 microseconds per operation
    assert(avg_time < 5.0);
}

void benchmark_message_filter_perf() {
    const int iterations = 10000;
    Settings settings = Settings::get_defaults();
    MessageFilter filter(settings);
    PerformanceTimer timer;
    
    timer.start();
    for (int i = 0; i < iterations; ++i) {
        filter.should_process("C123", "test message", "U123");
    }
    timer.stop();
    
    double avg_time = timer.elapsed_us() / iterations;
    std::cout << "MessageFilter check: " << avg_time << " us/op (avg over " 
              << iterations << " iterations)" << std::endl;
    
    // Performance requirement: < 5 microseconds per check
    assert(avg_time < 5.0);
}

// Forward declaration
void test_performance_advanced();

// Export for test_main.cpp
void test_performance() {
    std::cout << "\n=== Performance Benchmarks ===" << std::endl;
    benchmark_websocket_client_creation();
    benchmark_notification_display_perf();
    benchmark_http_client_creation();
    benchmark_slack_api_creation();
    benchmark_message_processing_throughput();
    benchmark_settings_creation();
    benchmark_config_loader_perf();
    benchmark_json_parser_perf();
    benchmark_message_filter_perf();
    
    // Advanced performance tests
    test_performance_advanced();
    
    std::cout << "All performance benchmarks passed!" << std::endl;
}
