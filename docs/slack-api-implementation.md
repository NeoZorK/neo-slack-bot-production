# Slack API - Implementation Details

## Error Handling

### Error Types

1. **Network Errors**
   - Connection timeout
   - DNS resolution failure
   - SSL/TLS handshake failure

2. **Authentication Errors**
   - Invalid token (`invalid_auth`)
   - Missing scope (`missing_scope`)
   - Token expired

3. **API Errors**
   - Rate limit exceeded (`rate_limited`)
   - Invalid request parameters
   - Server errors (5xx)

4. **WebSocket Errors**
   - Connection closed unexpectedly
   - Protocol errors (unmasked frames, invalid opcode)
   - SSL errors

### Error Recovery

**File**: `src/error_recovery.cpp`

```cpp
class ErrorRecovery {
    void handle_error(ErrorType type, const std::string& message);
    bool should_retry(ErrorType type) const;
    int get_retry_delay(int attempt) const;
    void reset_error_count(ErrorType type);
};
```

**Recovery Strategies**:

- **Network Errors**: Retry with exponential backoff (1s, 2s, 4s, ...)
- **Rate Limits**: Wait for `Retry-After` header
- **Authentication Errors**: Fail fast, don't retry
- **WebSocket Errors**: Reconnect with exponential backoff

### Error Logging

All errors are logged via the `Logger` singleton:

```cpp
Logger::get_instance().error("API request failed: " + error_message);
Logger::get_instance().warn("Rate limited, waiting " + std::to_string(seconds) + " seconds");
```

## Rate Limiting

### Slack API Rate Limits

Slack enforces rate limits on HTTP API requests:

- **Tier 1**: 1 request per second
- **Tier 2**: 20 requests per minute
- **Tier 3**: 50+ requests per minute (varies by method)

### Rate Limiter Implementation

**File**: `src/rate_limiter.cpp`

```cpp
class RateLimiter {
    bool can_make_request() const;
    void record_request();
    void handle_rate_limit_response(const std::string& response);
    int get_retry_after_seconds() const;
};
```

**Features**:
- Token bucket algorithm
- Automatic retry-after handling
- Request tracking
- Thread-safe operations

### Rate Limit Handling

```cpp
if (!rate_limiter_->can_make_request()) {
    int wait_seconds = rate_limiter_->get_retry_after_seconds();
    std::this_thread::sleep_for(std::chrono::seconds(wait_seconds));
    continue;
}

rate_limiter_->record_request();
rate_limiter_->handle_rate_limit_response(response);
```

## Implementation Details

### HTTP Client

**Library**: cpp-httplib (header-only)

**File**: `src/http_client.cpp`

**Features**:
- Automatic HTTPS support (OpenSSL)
- Redirect following
- Custom headers
- Timeout configuration
- Form-urlencoded support

**Usage**:
```cpp
HttpClient client;
client.set_header("Authorization", "Bearer " + token);
client.set_header("Content-Type", "application/json");
client.set_timeout(30);

std::string response;
bool success = client.post("https://slack.com/api/chat.postMessage", 
                          json_body, response);
```

### WebSocket Client

**Implementation**: Native OpenSSL + POSIX sockets

**Files**: 
- `src/websocket_client.cpp` - Main WebSocket client
- `src/websocket_client_connection.cpp` - Connection logic
- `src/websocket_client_messaging.cpp` - Messaging logic

**Features**:
- RFC 6455 compliant
- Frame masking (client-to-server)
- Ping/pong keepalive
- Automatic reconnection
- Exponential backoff

**Connection Lifecycle**:
1. Parse dynamic URL
2. Establish TCP connection
3. Establish TLS connection (SSL_connect)
4. Perform WebSocket handshake
5. Enter message loop
6. Handle reconnection on disconnect

### JSON Parsing

**Implementation**: Manual parsing (no external library)

**File**: `src/json_parser.cpp`

**Features**:
- Extract string values from JSON
- Extract numeric values
- Extract boolean values
- Handle nested structures

**Usage**:
```cpp
std::string value = json_parser::extract_string(json, "key");
int number = json_parser::extract_int(json, "number");
bool flag = json_parser::extract_bool(json, "flag");
```

### Input Validation

**File**: `src/input_validator.cpp`

**Validations**:
- Bot token format (`xoxb-...`, 20-200 chars)
- App token format (`xapp-...`, 20-200 chars)
- Channel ID format (`C...`, `D...`, `G...`)
- User ID format (`U...`)
- Message text (non-empty, max length)

## Best Practices

### 1. Always Acknowledge Events

```cpp
if (auto envelope_id = event_parser::extract_envelope_id(message)) {
    ws_client.send_message("{\"envelope_id\":\"" + *envelope_id + "\"}");
}
```

### 2. Validate Input Before API Calls

```cpp
if (!InputValidator::is_valid_channel_id(channel)) {
    Logger::get_instance().error("Invalid channel ID");
    return false;
}
```

### 3. Handle Rate Limits Gracefully

```cpp
if (response.find("rate_limited") != std::string::npos) {
    int retry_after = extract_retry_after(response);
    std::this_thread::sleep_for(std::chrono::seconds(retry_after));
    continue;
}
```

### 4. Use Exponential Backoff for Reconnection

```cpp
int delay = std::min(retry_delay_sec_ * (1 << reconnect_attempts_), 
                     MAX_RECONNECT_DELAY_SEC);
```

### 5. Log Errors Without Exposing Tokens

```cpp
Logger::get_instance().debug("Token: " + token_.substr(0, 15) + "...");
```

## Troubleshooting

### Connection Issues

**Problem**: `403 Forbidden` during WebSocket handshake

**Solution**:
- Verify `SLACK_APP_TOKEN` is set
- Check App-Level Token has `connections:write` scope
- Ensure Socket Mode is enabled in Slack app settings

**Problem**: `apps.connections.open` returns empty response

**Solution**:
- Check network connectivity
- Verify App-Level Token is valid
- Check token has correct scope
- Enable DEBUG logging: `export DEBUG=1`

### API Issues

**Problem**: Rate limit errors

**Solution**:
- Implement rate limiting (already done)
- Reduce request frequency
- Use batch operations where possible

**Problem**: `invalid_auth` errors

**Solution**:
- Verify Bot Token is correct
- Check token hasn't been revoked
- Ensure app is installed in workspace

### WebSocket Issues

**Problem**: Connection closes with code 1002 (Protocol Error)

**Solution**:
- Ensure client frames are masked (RFC 6455 requirement)
- Check frame encoding implementation
- Verify WebSocket handshake headers

**Problem**: Constant reconnections

**Solution**:
- Check network stability
- Verify token validity
- Review error logs for specific issues
- Ensure ping/pong keepalive is working

## References

- [Slack API Documentation](https://api.slack.com/)
- [Socket Mode Guide](https://api.slack.com/apis/connections/socket)
- [WebSocket RFC 6455](https://tools.ietf.org/html/rfc6455)
- [Slack API Methods](https://api.slack.com/methods)
- [OAuth Scopes](https://api.slack.com/scopes)

## Version History

- **v0.0.6**: Socket Mode v2 support, dynamic URL retrieval, frame masking
- **v0.0.5**: Rate limiting, error recovery, enhanced logging
- **v0.0.3**: HTTP API integration, command handling
- **v0.0.2**: Initial Socket Mode implementation

## See Also

- [Slack API Overview](slack-api-overview.md) - Architecture overview
- [Slack API - Socket Mode](slack-api-socket-mode.md) - Socket Mode details
- [Slack API - HTTP Methods](slack-api-http.md) - HTTP API methods
- [Slack API - Authentication](slack-api-authentication.md) - Authentication details
- [Slack API - Event Processing](slack-api-events.md) - Event processing details
