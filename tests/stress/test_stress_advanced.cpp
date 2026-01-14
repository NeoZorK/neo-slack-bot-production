#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include "../src/input_validator.hpp"
#include "../src/event_parser.hpp"
#include "../src/rate_limiter.hpp"

// Forward declaration
void test_stress_concurrent();

using namespace std::chrono;
using namespace std;

// Advanced stress tests for comprehensive system testing

void stress_test_input_validator_high_load(int iterations) {
    std::vector<std::string> test_tokens;
    std::vector<std::string> test_channels;
    std::vector<std::string> test_users;
    
    // Generate test data
    for (int i = 0; i < 1000; ++i) {
        test_tokens.push_back(
            "xox" "b-1234567890-1234567890123-abcdefghijklmnopqrstuvwx" +
            std::to_string(i));
        test_channels.push_back("C" + std::string(8, '0' + (i % 10)));
        test_users.push_back("U" + std::string(10, '0' + (i % 10)));
    }
    
    auto start = high_resolution_clock::now();
    
    int valid_count = 0;
    for (int i = 0; i < iterations; ++i) {
        int idx = i % test_tokens.size();
        if (InputValidator::is_valid_bot_token(test_tokens[idx])) {
            valid_count++;
        }
        if (InputValidator::is_valid_channel_id(test_channels[idx])) {
            valid_count++;
        }
        if (InputValidator::is_valid_user_id(test_users[idx])) {
            valid_count++;
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Input validator high load stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Valid checks: " << valid_count << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    if (duration_ms > 0) {
        double throughput = (iterations * 1000.0) / duration_ms;
        std::cout << "  Throughput: " << throughput << " validations/sec" << std::endl;
    }
    
    assert(duration_ms >= 0);
    assert(valid_count > 0);
}

void stress_test_event_parser_high_throughput(int iterations) {
    std::vector<std::string> test_events = {
        R"({"type":"hello"})",
        R"({"type":"events_api","envelope_id":"test-123","payload":{"type":"event_callback","event":{"type":"message","text":"test","channel":"C12345678","user":"U1234567890"}}})",
        R"({"type":"events_api","envelope_id":"test-456","payload":{"type":"event_callback","event":{"type":"app_mention","text":"<@U123> hello","channel":"C12345678","user":"U1234567890"}}})",
        R"({"type":"slash_commands","envelope_id":"test-789","payload":{"command":"/status","text":"","channel_id":"C12345678","user_id":"U1234567890"}})",
        R"({"type":"disconnect"})"
    };
    
    auto start = high_resolution_clock::now();
    
    int parsed_count = 0;
    for (int i = 0; i < iterations; ++i) {
        const auto& json = test_events[i % test_events.size()];
        
        auto type = event_parser::parse_type(json);
        if (type != event_parser::event_type::unknown) {
            parsed_count++;
        }
        
        auto envelope_id = event_parser::extract_envelope_id(json);
        if (envelope_id.has_value()) {
            parsed_count++;
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Event parser high throughput stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Parsed successfully: " << parsed_count << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    if (duration_ms > 0) {
        double throughput = (iterations * 1000.0) / duration_ms;
        std::cout << "  Throughput: " << throughput << " parses/sec" << std::endl;
    }
    
    assert(duration_ms >= 0);
    assert(parsed_count > iterations);
}

void stress_test_rate_limiter_high_frequency(int iterations) {
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
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    auto duration_us = duration_cast<microseconds>(end - start).count();
    
    std::cout << "Rate limiter high frequency stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Allowed: " << allowed << std::endl;
    std::cout << "  Blocked: " << blocked << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms (" << duration_us << " us)" << std::endl;
    if (duration_us > 0) {
        double throughput = (iterations * 1000000.0) / duration_us;
        std::cout << "  Throughput: " << throughput << " checks/sec" << std::endl;
    }
    
    assert(duration_us >= 0);
    assert(allowed + blocked == iterations);
}

// Export for test_stress.cpp
void test_stress_advanced() {
    std::cout << "\n--- Advanced Stress Tests ---" << std::endl;
    
    test_stress_concurrent();
    stress_test_input_validator_high_load(100000);
    stress_test_event_parser_high_throughput(50000);
    stress_test_rate_limiter_high_frequency(100000);
    
    std::cout << "All advanced stress tests passed!" << std::endl;
}
