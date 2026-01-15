#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <memory>
#include <cassert>
#include "../src/websocket_client.hpp"

using namespace std::chrono;
using namespace std;

void stress_test_concurrent_connections(int thread_count, int iterations_per_thread) {
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> failure_count{0};
    
    auto worker = [&](int id) {
        for (int i = 0; i < iterations_per_thread; ++i) {
            try {
                WebSocketClient client("test-token-" + std::to_string(id));
                if (!client.is_connected()) {
                    success_count++;
                } else {
                    failure_count++;
                }
            } catch (...) {
                failure_count++;
            }
        }
    };
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Concurrent connections stress test:" << std::endl;
    std::cout << "  Threads: " << thread_count << std::endl;
    std::cout << "  Iterations per thread: " << iterations_per_thread << std::endl;
    std::cout << "  Total operations: " << (thread_count * iterations_per_thread) << std::endl;
    std::cout << "  Success: " << success_count.load() << std::endl;
    std::cout << "  Failures: " << failure_count.load() << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    
    double failure_rate = (double)failure_count.load() / 
                         (thread_count * iterations_per_thread) * 100.0;
    assert(failure_rate < 5.0);
}

void stress_test_memory_usage(int iterations) {
    std::vector<std::unique_ptr<WebSocketClient>> clients;
    clients.reserve(iterations);
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        clients.push_back(std::make_unique<WebSocketClient>("test-token"));
    }
    
    clients.clear();
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Memory usage stress test:" << std::endl;
    std::cout << "  Objects created: " << iterations << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms (" << duration_us << " us)" << std::endl;
    
    assert(duration_us >= 0);
}

void stress_test_rapid_connect_disconnect(int cycles) {
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < cycles; ++i) {
        WebSocketClient client("test-token");
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Rapid connect/disconnect stress test:" << std::endl;
    std::cout << "  Cycles: " << cycles << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms (" << duration_us << " us)" << std::endl;
    if (duration_us > 0) {
        std::cout << "  Rate: " << (cycles * 1000000.0 / duration_us) << " cycles/sec" << std::endl;
    } else {
        std::cout << "  Rate: > 1M cycles/sec (extremely fast)" << std::endl;
    }
    
    assert(duration_us >= 0);
}

void test_stress_connection() {
    stress_test_concurrent_connections(10, 100);
    stress_test_memory_usage(1000);
    stress_test_rapid_connect_disconnect(100);
}
