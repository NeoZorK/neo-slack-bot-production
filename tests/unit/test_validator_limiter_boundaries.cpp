#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <optional>
#include "../../src/input_validator.hpp"
#include "../../src/rate_limiter.hpp"

void test_validator_boundaries() {
    // Channel ID Boundaries
    // Min length 9 (C + 8 digits)
    assert(InputValidator::is_valid_channel_id("C12345678")); // 9 chars - Valid
    assert(!InputValidator::is_valid_channel_id("C1234567")); // 8 chars - Invalid
    
    // Max length 15 (C + 14 digits)
    assert(InputValidator::is_valid_channel_id("C12345678901234")); // 15 chars - Valid
    assert(!InputValidator::is_valid_channel_id("C123456789012345")); // 16 chars - Invalid
    
    // User ID Boundaries
    // Min length 9 (U + 8 digits)
    assert(InputValidator::is_valid_user_id("U12345678")); // 9 chars - Valid
    assert(!InputValidator::is_valid_user_id("U1234567")); // 8 chars - Invalid
    
    // Max length 15 (U + 14 digits)
    assert(InputValidator::is_valid_user_id("U12345678901234")); // 15 chars - Valid
    assert(!InputValidator::is_valid_user_id("U123456789012345")); // 16 chars - Invalid
    
    // Message Text Boundaries
    std::string max_msg(4000, 'a');
    assert(InputValidator::is_valid_message_text(max_msg));
    
    std::string too_long_msg(4001, 'a');
    assert(!InputValidator::is_valid_message_text(too_long_msg));
    
    std::cout << "  ✓ Validator boundaries test passed" << std::endl;
}

void test_rate_limiter_parsing() {
    RateLimiter limiter;
    
    // Test 1: Valid retry_after
    std::string resp1 = R"({"ok":false,"error":"rate_limited","retry_after":42})";
    limiter.handle_rate_limit_response(resp1);
    assert(limiter.is_rate_limited());
    // Note: get_retry_after_seconds() returns remaining time, which might decrease slightly
    // But we just set it, so it should be close to 42
    long retry = limiter.get_retry_after_seconds();
    assert(retry > 0 && retry <= 42);
    
    // Test 2: Invalid/Missing retry_after (defaults to window)
    // Reset first
    limiter.reset(); 
    std::string resp2 = R"({"ok":false,"error":"rate_limited"})";
    limiter.handle_rate_limit_response(resp2);
    assert(limiter.is_rate_limited());
    retry = limiter.get_retry_after_seconds();
    assert(retry > 0); // Should use default window (60s)
    
    // Test 3: Not rate limited response
    limiter.reset();
    std::string resp3 = R"({"ok":true})";
    limiter.handle_rate_limit_response(resp3);
    assert(!limiter.is_rate_limited());
    
    std::cout << "  ✓ Rate limiter parsing test passed" << std::endl;
}

void test_validator_limiter_boundaries() {
    std::cout << "Testing Validator and Limiter Boundaries..." << std::endl;
    test_validator_boundaries();
    test_rate_limiter_parsing();
    std::cout << "All Validator and Limiter boundary tests passed!" << std::endl;
}
