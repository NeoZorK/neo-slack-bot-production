#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <cassert>
#include "../src/event_parser.hpp"
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"
#include "../src/command_handler.hpp"
#include "../src/rate_limiter.hpp"
#include "../src/metrics.hpp"

using namespace std::chrono;
using namespace std;

// Concurrent stress tests

void stress_test_concurrent_message_processing(int thread_count, int messages_per_thread) {
    std::vector<std::thread> threads;
    std::atomic<int> processed_count{0};
    std::atomic<int> error_count{0};
    
    auto worker = [&](int thread_id) {
        Settings settings = Settings::get_defaults();
        MessageFilter filter(settings);
        
        for (int i = 0; i < messages_per_thread; ++i) {
            try {
                std::string json = R"({
                    "type": "events_api",
                    "envelope_id": "test-envelope-)" + std::to_string(thread_id) + 
                    "-" + std::to_string(i) + R"(",
                    "payload": {
                        "type": "event_callback",
                        "event": {
                            "type": "message",
                            "text": "Test message )" + std::to_string(i) + R"(",
                            "channel": "C12345678",
                            "user": "U1234567890"
                        }
                    }
                })";
                
                auto channel = event_parser::extract_channel_id(json);
                auto text = event_parser::extract_message_text(json);
                
                if (channel && text) {
                    if (filter.should_process(*channel, *text)) {
                        processed_count++;
                    }
                }
            } catch (...) {
                error_count++;
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
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Concurrent message processing stress test:" << std::endl;
    std::cout << "  Threads: " << thread_count << std::endl;
    std::cout << "  Messages per thread: " << messages_per_thread << std::endl;
    std::cout << "  Total messages: " << (thread_count * messages_per_thread) << std::endl;
    std::cout << "  Processed: " << processed_count.load() << std::endl;
    std::cout << "  Errors: " << error_count.load() << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    if (duration_ms > 0) {
        double throughput = (processed_count.load() * 1000.0) / duration_ms;
        std::cout << "  Throughput: " << throughput << " messages/sec" << std::endl;
    }
    
    double error_rate = (double)error_count.load() / 
                       (thread_count * messages_per_thread) * 100.0;
    assert(error_rate < 1.0); // Less than 1% error rate
    assert(processed_count.load() > 0);
}

void stress_test_command_handler_concurrent(int thread_count, int commands_per_thread) {
    CommandHandler handler;
    
    // Register test commands
    std::atomic<int> command_count{0};
    handler.register_command("test", [&command_count](const std::string&, 
                                                      const std::vector<std::string>&) {
        command_count++;
    });
    
    std::vector<std::thread> threads;
    
    auto worker = [&](int /* thread_id */) {
        for (int i = 0; i < commands_per_thread; ++i) {
            handler.handle_command("test arg1 arg2", 
                                  "C12345678", 
                                  "U1234567890");
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
    
    std::cout << "Command handler concurrent stress test:" << std::endl;
    std::cout << "  Threads: " << thread_count << std::endl;
    std::cout << "  Commands per thread: " << commands_per_thread << std::endl;
    std::cout << "  Total commands: " << (thread_count * commands_per_thread) << std::endl;
    std::cout << "  Processed: " << command_count.load() << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    if (duration_ms > 0) {
        double throughput = (command_count.load() * 1000.0) / duration_ms;
        std::cout << "  Throughput: " << throughput << " commands/sec" << std::endl;
    }
    
    assert(command_count.load() == thread_count * commands_per_thread);
}

void stress_test_metrics_concurrent(int thread_count, int operations_per_thread) {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    std::vector<std::thread> threads;
    
    auto worker = [&](int /*thread_id*/) {
        for (int i = 0; i < operations_per_thread; ++i) {
            metrics.increment_event_count("message");
            metrics.record_latency("process_message", milliseconds(10 + (i % 50)));
            metrics.update_memory_usage();
            
            if (i % 10 == 0) {
                metrics.record_error("test_error");
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
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    
    uint64_t total_events = metrics.get_event_count("message");
    uint64_t total_errors = metrics.get_error_count("test_error");
    
    std::cout << "Metrics concurrent stress test:" << std::endl;
    std::cout << "  Threads: " << thread_count << std::endl;
    std::cout << "  Operations per thread: " << operations_per_thread << std::endl;
    std::cout << "  Total events recorded: " << total_events << std::endl;
    std::cout << "  Total errors recorded: " << total_errors << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    if (duration_ms > 0) {
        double throughput = (thread_count * operations_per_thread * 1000.0) / duration_ms;
        std::cout << "  Throughput: " << throughput << " operations/sec" << std::endl;
    }
    
    assert(total_events == static_cast<uint64_t>(thread_count * operations_per_thread));
    assert(total_errors == static_cast<uint64_t>((thread_count * operations_per_thread) / 10));
}

// Export for test_stress_advanced.cpp
void test_stress_concurrent() {
    stress_test_concurrent_message_processing(10, 1000);
    stress_test_command_handler_concurrent(10, 500);
    stress_test_metrics_concurrent(10, 1000);
}
