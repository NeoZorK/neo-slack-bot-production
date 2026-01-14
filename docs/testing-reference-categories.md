# Testing Reference - Test Categories

# Testing Reference

## Test Categories

### Unit Tests

**Purpose**: Test individual functions and classes in isolation

**Scope**:
- Single function or method
- Single class
- Isolated component

**Characteristics**:
- Fast execution
- No external dependencies
- Deterministic results
- Easy to debug

**Example**:
```cpp
void test_websocket_client_creation() {
    WebSocketClient client("test-token");
    assert(!client.is_connected());
}
```

### Integration Tests

**Purpose**: Test interaction between components

**Scope**:
- Multiple classes working together
- Component integration
- Data flow between modules

**Characteristics**:
- May use real dependencies
- Test realistic scenarios
- Verify interfaces
- Check error propagation

**Example**:
```cpp
void test_slack_api_send_message() {
    HttpClient http_client;
    SlackAPI api("test-token", http_client);
    bool result = api.send_message("channel", "test");
    assert(result);
}
```

### System Tests

**Purpose**: Test complete workflows end-to-end

**Scope**:
- Full application flow
- Real Slack API (test workspace)
- Complete event processing

**Characteristics**:
- Slower execution
- May require external services
- Test user scenarios
- Verify system behavior

**Example**:
```cpp
void test_complete_message_flow() {
    // Setup
    WebSocketClient ws("test-token");
    MacOSNotifications notifications;
    
    // Connect
    ws.connect();
    
    // Process message
    ws.handle_message("test message");
    
    // Verify notification
    assert(notifications.last_notification() == "test message");
}
```

### Security Tests

**Purpose**: Test security measures and attack prevention

**Scope**:
- Input validation security
- Token security
- Injection attack prevention
- Buffer overflow prevention
- DoS attack prevention

**Characteristics**:
- Test malicious input patterns
- Verify security boundaries
- Test sanitization functions
- Verify rate limiting

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

**Security Test Categories**:
- ✅ Token security (not logged, format validation)
- ✅ SQL injection prevention
- ✅ XSS prevention
- ✅ Path traversal prevention
- ✅ Buffer overflow prevention
- ✅ Null byte injection prevention
- ✅ Command injection prevention
- ✅ Rate limiting (DoS prevention)
- ✅ Memory safety
- ✅ Input sanitization

## Test Framework

### Framework Selection

**Current**: Custom test framework (minimal)

**Future Options**:
- **Catch2**: Modern C++ test framework
- **Google Test**: Popular C++ testing framework
- **doctest**: Lightweight header-only framework

### Test Macros

**Basic Assertions**:
```cpp
assert(condition);                    // Basic assertion
assert_eq(actual, expected);         // Equality check
assert_ne(actual, expected);         // Inequality check
assert_true(condition);              // Boolean true
assert_false(condition);             // Boolean false
```

**Exception Testing** (future):
```cpp
assert_throws<ExceptionType>([]() {
    // code that should throw
});
```

### Test Fixtures


## See Also

- [Testing Reference - Test Execution](testing-reference-execution.md) - Test execution and commands
- [Testing Reference](testing-reference.md) - Overview
