#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <cassert>
#include "../src/event_parser.hpp"
#include "../src/json_parser.hpp"
#include "../src/message_filter.hpp"
#include "../src/command_handler.hpp"
#include "../src/settings.hpp"

using namespace std::chrono;

// Performance throughput tests

void performance_test_event_parser_throughput(int iterations) {
    std::string json = R"({
        "type": "events_api",
        "envelope_id": "test-envelope-123",
        "payload": {
            "type": "event_callback",
            "event": {
                "type": "message",
                "text": "Hello, world!",
                "channel": "C12345678",
                "user": "U1234567890",
                "ts": "1234567890.123456"
            }
        }
    })";
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto type = event_parser::parse_type(json);
        auto envelope_id = event_parser::extract_envelope_id(json);
        auto channel = event_parser::extract_channel_id(json);
        auto text = event_parser::extract_message_text(json);
        auto user = event_parser::extract_user_id(json);
        (void)type; (void)envelope_id; (void)channel; (void)text; (void)user;
    }
    
    auto end = high_resolution_clock::now();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    double throughput = (iterations * 1000000.0) / duration_us;
    
    std::cout << "\n--- Event Parser Throughput Test ---" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Total time: " << (duration_us / 1000.0) << " ms" << std::endl;
    std::cout << "  Avg time: " << (duration_us / iterations) << " us/operation" << std::endl;
    std::cout << "  Throughput: " << throughput << " operations/sec" << std::endl;
    
    assert(throughput > 100000.0); // > 100K operations/sec
}

void performance_test_json_parser_throughput(int iterations) {
    std::string json = R"({
        "notifications": {
            "enabled": true,
            "sound_enabled": false,
            "dnd_hours": {
                "enabled": false,
                "start": "22:00",
                "end": "08:00"
            }
        },
        "connection": {
            "max_retries": 10,
            "timeout_seconds": 30,
            "ping_interval_seconds": 30
        },
        "events": {
            "keyword_filter": {
                "enabled": true,
                "mode": "include",
                "keywords": ["urgent", "important", "critical"]
            }
        }
    })";
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        json_parser::extract_string(json, "notifications");
        json_parser::extract_string(json, "connection");
        json_parser::extract_string(json, "events");
        json_parser::extract_bool(json, "enabled", false);
        json_parser::extract_int(json, "max_retries", 0);
        json_parser::extract_string_array(json, "keywords");
    }
    
    auto end = high_resolution_clock::now();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    double throughput = (iterations * 1000000.0) / duration_us;
    
    std::cout << "\n--- JSON Parser Throughput Test ---" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Total time: " << (duration_us / 1000.0) << " ms" << std::endl;
    std::cout << "  Throughput: " << throughput << " operations/sec" << std::endl;
    
    assert(throughput > 50000.0); // > 50K operations/sec
}

void performance_test_concurrent_throughput(int thread_count, int operations_per_thread) {
    std::atomic<int> completed{0};
    std::vector<std::thread> threads;
    
    auto worker = [&](int /* thread_id */) {
        Settings settings = Settings::get_defaults();
        MessageFilter filter(settings);
        CommandHandler handler;
        
        for (int i = 0; i < operations_per_thread; ++i) {
            filter.should_process("C12345678", "test message", "U1234567890");
            completed++;
            handler.handle_command("test", "C12345678", "U1234567890");
            completed++;
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
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    double throughput = (completed.load() * 1000.0) / duration_ms;
    
    std::cout << "\n--- Concurrent Throughput Test ---" << std::endl;
    std::cout << "  Threads: " << thread_count << std::endl;
    std::cout << "  Operations per thread: " << operations_per_thread << std::endl;
    std::cout << "  Total operations: " << completed.load() << std::endl;
    std::cout << "  Total time: " << duration_ms << " ms" << std::endl;
    std::cout << "  Throughput: " << throughput << " operations/sec" << std::endl;
    
    assert(completed.load() == thread_count * operations_per_thread * 2); // 2 ops per iteration
    assert(throughput > 10000.0); // > 10K operations/sec
}

// Export for test_performance_advanced.cpp
void test_performance_throughput() {
    performance_test_event_parser_throughput(100000);
    performance_test_json_parser_throughput(100000);
    performance_test_concurrent_throughput(10, 1000);
}
