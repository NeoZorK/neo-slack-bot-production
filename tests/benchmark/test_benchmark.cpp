#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include "../src/websocket_client.hpp"
#include "../src/http_client.hpp"
#ifdef __APPLE__
#include "../src/macos_notifications.hpp"
#endif

using namespace std::chrono;

// Benchmark utilities
class Benchmark {
public:
    void start() {
        start_time = high_resolution_clock::now();
    }
    
    void stop() {
        end_time = high_resolution_clock::now();
    }
    
    double elapsed_ns() const {
        return duration_cast<nanoseconds>(end_time - start_time).count();
    }
    
    double elapsed_us() const {
        return duration_cast<microseconds>(end_time - start_time).count();
    }
    
    double elapsed_ms() const {
        return duration_cast<milliseconds>(end_time - start_time).count();
    }

private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;
};

// Benchmark: Object creation overhead
void benchmark_object_creation() {
    const int iterations = 100000;
    Benchmark bench;
    
    std::cout << "\n=== Object Creation Benchmarks ===" << std::endl;
    
    // WebSocket client
    bench.start();
    for (int i = 0; i < iterations; ++i) {
        WebSocketClient client("test-token");
    }
    bench.stop();
    std::cout << "WebSocketClient: " << (bench.elapsed_us() / iterations) 
              << " us/creation" << std::endl;
    
    // HTTP client
    bench.start();
    for (int i = 0; i < iterations; ++i) {
        HttpClient client;
    }
    bench.stop();
    std::cout << "HttpClient: " << (bench.elapsed_us() / iterations) 
              << " us/creation" << std::endl;
    
    // macOS Notifications
#ifdef __APPLE__
    bench.start();
    for (int i = 0; i < iterations; ++i) {
        MacOSNotifications notifications;
    }
    bench.stop();
    std::cout << "MacOSNotifications: " << (bench.elapsed_us() / iterations) 
              << " us/creation" << std::endl;
#endif
}

// Benchmark: Memory allocation patterns
void benchmark_memory_allocation() {
    const int iterations = 10000;
    Benchmark bench;
    
    std::cout << "\n=== Memory Allocation Benchmarks ===" << std::endl;
    
    // Vector push_back
    bench.start();
    std::vector<int> vec1;
    for (int i = 0; i < iterations; ++i) {
        vec1.push_back(i);
    }
    bench.stop();
    std::cout << "vector::push_back (no reserve): " << bench.elapsed_us() 
              << " us" << std::endl;
    
    // Vector push_back with reserve
    bench.start();
    std::vector<int> vec2;
    vec2.reserve(iterations);
    for (int i = 0; i < iterations; ++i) {
        vec2.push_back(i);
    }
    bench.stop();
    std::cout << "vector::push_back (with reserve): " << bench.elapsed_us() 
              << " us" << std::endl;
}

// Benchmark: String operations
void benchmark_string_operations() {
    const int iterations = 10000;
    Benchmark bench;
    
    std::cout << "\n=== String Operation Benchmarks ===" << std::endl;
    
    // String concatenation
    bench.start();
    std::string result;
    for (int i = 0; i < iterations; ++i) {
        result += "test" + std::to_string(i);
    }
    bench.stop();
    std::cout << "String concatenation: " << bench.elapsed_us() 
              << " us" << std::endl;
    
    // String concatenation with reserve
    bench.start();
    std::string result2;
    result2.reserve(iterations * 10);
    for (int i = 0; i < iterations; ++i) {
        result2 += "test" + std::to_string(i);
    }
    bench.stop();
    std::cout << "String concatenation (reserved): " << bench.elapsed_us() 
              << " us" << std::endl;
}

// Benchmark: JSON parsing (simulated)
void benchmark_json_parsing() {
    const int iterations = 1000;
    Benchmark bench;
    
    std::cout << "\n=== JSON Parsing Benchmarks ===" << std::endl;
    
    std::string json = R"({"type":"message","text":"test","user":"U123","channel":"C456"})";
    
    bench.start();
    for (int i = 0; i < iterations; ++i) {
        // Simulate JSON parsing (will be replaced with nlohmann/json)
        (void)json.find("\"text\""); // Simulate parsing
        // In real implementation: json::parse(json)["text"]
    }
    bench.stop();
    std::cout << "JSON parsing (simulated): " << (bench.elapsed_us() / iterations) 
              << " us/parse" << std::endl;
}

// Benchmark: Notification display
static void benchmark_notification_display_bench() {
#ifdef __APPLE__
    const int iterations = 100;
    MacOSNotifications notifications;
    Benchmark bench;
    
    std::cout << "\n=== Notification Benchmarks ===" << std::endl;
    
    bench.start();
    for (int i = 0; i < iterations; ++i) {
        notifications.show_notification("Benchmark", "Test notification #" + std::to_string(i));
    }
    bench.stop();
    std::cout << "Notification display: " << (bench.elapsed_ms() / iterations) 
              << " ms/notification" << std::endl;
#else
    std::cout << "Notification benchmarks skipped on non-macOS platform" << std::endl;
#endif
}

// Export for test_main.cpp
void test_benchmark() {
    std::cout << "\n=== Running Benchmarks ===" << std::endl;
    benchmark_object_creation();
    benchmark_memory_allocation();
    benchmark_string_operations();
    benchmark_json_parsing();
    benchmark_notification_display_bench();
    std::cout << "\nAll benchmarks completed!" << std::endl;
}
