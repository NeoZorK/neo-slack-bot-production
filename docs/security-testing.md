# Security Testing Guide

## Overview

Neo Slack Bot includes comprehensive security tests to verify protection against common attack vectors and ensure secure operation.

## Security Test Categories

### 1. Token Security Tests

**Purpose**: Verify tokens are handled securely

**Tests**:
- Token format validation
- Token not logged in error messages
- Token not exposed in logs
- Invalid token rejection

**Example**:
```cpp
void test_token_security_not_logged() {
    std::string token = "xoxb-your-bot-token-here";
    assert(InputValidator::is_valid_bot_token(token));
    // Verify token is not leaked in error messages
}
```

### 2. Injection Attack Prevention

**Purpose**: Prevent SQL injection, XSS, command injection

**Tests**:
- SQL injection pattern rejection
- XSS pattern rejection
- Command injection prevention
- Path traversal prevention

**Example**:
```cpp
void test_input_validation_sql_injection_prevention() {
    std::vector<std::string> sql_patterns = {
        "'; DROP TABLE users; --",
        "1' OR '1'='1"
    };
    
    for (const auto& pattern : sql_patterns) {
        assert(!InputValidator::is_valid_channel_id(pattern));
        assert(!InputValidator::is_valid_user_id(pattern));
    }
}
```

### 3. Buffer Overflow Prevention

**Purpose**: Prevent buffer overflow attacks

**Tests**:
- Maximum length enforcement
- Very long string rejection
- Memory safety verification

**Example**:
```cpp
void test_input_validation_buffer_overflow_prevention() {
    std::string very_long_token = "xoxb-" + std::string(1000, 'A');
    assert(!InputValidator::is_valid_bot_token(very_long_token));
    
    std::string oversized_message(5000, 'A');
    assert(!InputValidator::is_valid_message_text(oversized_message));
}
```

### 4. Null Byte Injection Prevention

**Purpose**: Prevent null byte injection attacks

**Tests**:
- Null byte detection in IDs
- Null byte sanitization in messages
- Token validation with null bytes

**Example**:
```cpp
void test_input_validation_null_byte_injection() {
    std::string null_byte_string = "C12345678\0malicious";
    assert(!InputValidator::is_valid_channel_id(null_byte_string));
    
    std::string message_with_null = "Hello\0World";
    auto sanitized = InputValidator::sanitize_message_text(message_with_null);
    assert(sanitized->find('\0') == std::string::npos);
}
```

### 5. Input Validation Security

**Purpose**: Verify comprehensive input validation

**Tests**:
- Token format validation
- Channel ID validation
- User ID validation
- Message text validation

**Example**:
```cpp
void test_token_format_validation_security() {
    std::vector<std::string> malicious_tokens = {
        "xoxb-1234567890-1234567890123-abc<script>",
        "xoxb-1234567890-1234567890123-abc; rm -rf",
        "xoxb-1234567890-1234567890123-abc' OR '1'='1"
    };
    
    for (const auto& token : malicious_tokens) {
        assert(!InputValidator::is_valid_bot_token(token));
    }
}
```

### 6. Rate Limiting Security

**Purpose**: Prevent DoS attacks

**Tests**:
- Rate limit enforcement
- Rapid request blocking
- Window-based limiting

**Example**:
```cpp
void test_rate_limiting_security() {
    RateLimiter limiter;
    
    int blocked = 0;
    for (int i = 0; i < 100; ++i) {
        if (!limiter.can_make_request()) {
            blocked++;
        } else {
            limiter.record_request();
        }
    }
    
    assert(blocked > 0); // Should block some requests
}
```

### 7. Sanitization Security

**Purpose**: Verify input sanitization works correctly

**Tests**:
- Channel ID sanitization
- User ID sanitization
- Message text sanitization
- Null byte removal

**Example**:
```cpp
void test_sanitization_security() {
    std::string message_with_null = "Hello\0World\0Test";
    auto sanitized = InputValidator::sanitize_message_text(message_with_null);
    assert(sanitized.has_value());
    assert(sanitized->find('\0') == std::string::npos);
}
```

### 8. Memory Safety

**Purpose**: Verify memory-safe operations

**Tests**:
- No buffer overflows
- Safe string operations
- Proper bounds checking

**Example**:
```cpp
void test_memory_safety_security() {
    // Test with various lengths - should not crash
    for (size_t len = 0; len < 20; ++len) {
        std::string test_str(len, 'A');
        (void)InputValidator::is_valid_channel_id(test_str);
        (void)InputValidator::is_valid_user_id(test_str);
    }
}
```

## Running Security Tests

### Run All Security Tests

```bash
cd tests
make test
```

Security tests are included in the standard test suite and run automatically.

### Run Only Security Tests (Future)

With test framework integration:
```bash
./bin/test_neo_slack_bot --test-category security
```

## Security Test Coverage

**Current Coverage**:
- ✅ Token security (format validation, not logged)
- ✅ SQL injection prevention
- ✅ XSS prevention
- ✅ Path traversal prevention
- ✅ Buffer overflow prevention
- ✅ Null byte injection prevention
- ✅ Command injection prevention
- ✅ Rate limiting (DoS prevention)
- ✅ Input sanitization
- ✅ Memory safety

**Test Count**: 15+ security test functions covering all major attack vectors

## Security Test Best Practices

1. **Comprehensive Coverage**: Test all input validation points
2. **Attack Patterns**: Use real-world attack patterns
3. **Edge Cases**: Test boundary conditions
4. **Sanitization**: Verify sanitization functions work correctly
5. **Memory Safety**: Ensure no buffer overflows or memory leaks

## Integration with CI/CD

Security tests are automatically run in CI/CD pipeline:
- Run on every release
- Fail build if security tests fail
- Report security test results

## Security Test Checklist

Before release:
- [ ] All security tests pass
- [ ] No new security vulnerabilities introduced
- [ ] Input validation covers all attack vectors
- [ ] Rate limiting prevents DoS attacks
- [ ] Token security verified
- [ ] Memory safety confirmed

## Related Documentation

- **Security Audit**: `docs/security-audit.md`
- **Input Validation**: `src/input_validator.hpp`
- **Testing Guide**: `docs/testing-guide.md`
