#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <cassert>
#include "../src/metrics.hpp"
#include "../src/websocket_client.hpp"
#include "../src/http_client.hpp"
#include "../src/slack_api.hpp"
#include "../src/message_filter.hpp"
#include "../src/command_handler.hpp"
#include "../src/settings.hpp"

// Forward declarations
void test_performance_latency();
void test_performance_throughput();

using namespace std::chrono;

// Advanced performance tests with detailed metrics

void performance_test_memory_footprint() {
    std::cout << "\n--- Memory Footprint Test ---" << std::endl;
    
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    // Measure baseline memory
    metrics.update_memory_usage();
    size_t baseline = metrics.get_current_memory_usage();
    
    // Create objects
    std::vector<std::unique_ptr<WebSocketClient>> clients;
    std::vector<std::unique_ptr<HttpClient>> http_clients;
    std::vector<std::unique_ptr<SlackAPI>> apis;
    
    for (int i = 0; i < 100; ++i) {
        clients.push_back(std::make_unique<WebSocketClient>("test-token"));
        http_clients.push_back(std::make_unique<HttpClient>());
        apis.push_back(std::make_unique<SlackAPI>("test-token"));
    }
    
    metrics.update_memory_usage();
    size_t after_creation = metrics.get_current_memory_usage();
    
    clients.clear();
    http_clients.clear();
    apis.clear();
    
    metrics.update_memory_usage();
    size_t after_cleanup = metrics.get_current_memory_usage();
    
    std::cout << "  Baseline memory: " << baseline << " bytes" << std::endl;
    std::cout << "  After creating 100 objects: " << after_creation << " bytes" << std::endl;
    std::cout << "  After cleanup: " << after_cleanup << " bytes" << std::endl;
    std::cout << "  Memory increase: " << (after_creation - baseline) << " bytes" << std::endl;
    std::cout << "  Memory per object set: " << ((after_creation - baseline) / 100) << " bytes" << std::endl;
    
    assert(after_creation >= baseline);
    // Memory should be released after cleanup (allowing some overhead)
    assert(after_cleanup <= after_creation);
}

void performance_test_cpu_utilization(int duration_seconds) {
    std::cout << "\n--- CPU Utilization Test ---" << std::endl;
    
    std::atomic<bool> running{true};
    std::atomic<int> operations{0};
    
    auto worker = [&]() {
        Settings settings = Settings::get_defaults();
        MessageFilter filter(settings);
        CommandHandler handler;
        
        while (running.load()) {
            filter.should_process("C12345678", "test message", "U1234567890");
            handler.handle_command("test", "C12345678", "U1234567890");
            operations++;
        }
    };
    
    std::thread worker_thread(worker);
    
    std::this_thread::sleep_for(seconds(duration_seconds));
    running.store(false);
    worker_thread.join();
    
    int total_ops = operations.load();
    double ops_per_sec = total_ops / (double)duration_seconds;
    
    std::cout << "  Duration: " << duration_seconds << " seconds" << std::endl;
    std::cout << "  Total operations: " << total_ops << std::endl;
    std::cout << "  Operations/sec: " << ops_per_sec << std::endl;
    
    assert(total_ops > 0);
    assert(ops_per_sec > 1000.0); // > 1K operations/sec sustained
}

// Export for test_performance.cpp
void test_performance_advanced() {
    std::cout << "\n=== Advanced Performance Tests ===" << std::endl;
    
    test_performance_latency();
    test_performance_throughput();
    performance_test_memory_footprint();
    performance_test_cpu_utilization(2);
    
    std::cout << "All advanced performance tests passed!" << std::endl;
}
