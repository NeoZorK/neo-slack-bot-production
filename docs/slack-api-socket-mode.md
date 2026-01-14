# Slack API - Socket Mode v2 Integration

## Overview

Socket Mode v2 uses a dynamic WebSocket URL obtained via the `apps.connections.open` API endpoint. This provides better reliability and load balancing compared to Socket Mode v1.

## Connection Flow

1. **Obtain Dynamic URL**
   ```
   POST https://slack.com/api/apps.connections.open
   Authorization: Bearer xapp-...
   Content-Type: application/x-www-form-urlencoded
   
   Response: {"ok":true,"url":"wss://wss-primary.slack.com/link/?ticket=..."}
   ```

2. **Establish WebSocket Connection**
   ```
   GET wss://wss-primary.slack.com/link/?ticket=...
   Upgrade: websocket
   Connection: Upgrade
   Sec-WebSocket-Key: <base64-encoded-key>
   Sec-WebSocket-Version: 13
   ```

3. **Receive Hello Event**
   ```
   {"type":"hello","connection_info":{...}}
   ```

## Multiple Connections

**⚠️ IMPORTANT**: Each WebSocket connection **MUST** have its own unique URL obtained via a separate call to `apps.connections.open`. This is a Socket Mode v2 requirement.

**Why Unique URLs Are Required**:
- Slack uses the URL ticket to route events to the correct connection
- Sharing a URL between multiple connections can cause message delivery issues
- Each connection needs its own ticket for proper event routing

**Best Practice for Multiple Bots**:
```cpp
// CORRECT: Each bot gets its own URL
for (int i = 0; i < bot_count; ++i) {
    WebSocketClient client(bot_token);
    
    // Get unique URL for this bot
    SlackAPI api(app_token);
    std::string websocket_url;
    if (api.get_socket_mode_url(websocket_url)) {
        client.set_websocket_url(websocket_url);
    }
    
    // Add rate limiting delay between API calls
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// INCORRECT: All bots share the same URL
SlackAPI api(app_token);
std::string websocket_url;
api.get_socket_mode_url(websocket_url);  // Single call

for (int i = 0; i < bot_count; ++i) {
    WebSocketClient client(bot_token);
    client.set_websocket_url(websocket_url);  // Same URL for all - WRONG!
}
```

**Rate Limiting**: When creating multiple connections, add a small delay (100ms recommended) between `apps.connections.open` calls to avoid hitting Slack API rate limits.

## Implementation

**File**: `src/slack_api.cpp`

```cpp
bool SlackAPI::get_socket_mode_url(std::string& websocket_url) {
    std::string url = build_api_url("apps.connections.open");
    std::string response;
    
    // Use form-urlencoded for apps.connections.open
    http_client_.set_header("Content-Type", "application/x-www-form-urlencoded");
    bool success = make_api_request(url, "", response, true);
    http_client_.set_header("Content-Type", "application/json"); // Restore
    
    // Parse response and extract URL
    // Decode escaped slashes (\/) in URL
    // Return websocket_url
}
```

**Files**: 
- `src/websocket_client.cpp` - Main client implementation
- `src/websocket_client_connection.cpp` - Connection management
- `src/websocket_client_messaging.cpp` - Message handling

```cpp
void WebSocketClient::set_websocket_url(const std::string& url) {
    websocket_url_ = url;
}

bool WebSocketClient::connect() {
    // Parse dynamic URL: wss://host:port/path
    // Establish TCP connection
    // Establish TLS connection
    // Perform WebSocket handshake (no Authorization header for dynamic URL)
}
```

## WebSocket Frame Handling

**RFC 6455 Compliance**:

- **Client-to-Server Frames**: Must be masked (masking key XOR)
- **Server-to-Client Frames**: Unmasked
- **Frame Types**: Text (0x1), Binary (0x2), Close (0x8), Ping (0x9), Pong (0xA)

**Implementation**: `src/websocket_frame.cpp`

```cpp
std::vector<unsigned char> encode_text_frame(const std::string& message) {
    // Generate random 4-byte masking key
    // Apply XOR masking to payload
    // Set MASK bit (0x80) in payload length byte
    // Return complete frame
}
```

## Event Types

Neo Slack Bot handles the following Socket Mode event types:

1. **hello** - Connection confirmation
   ```json
   {"type":"hello","connection_info":{...}}
   ```

2. **events_api** - Slack events (messages, mentions, etc.)
   ```json
   {
     "envelope_id": "xxx",
     "type": "events_api",
     "payload": {
       "event": {
         "type": "message",
         "text": "Hello",
         "channel": "C1234567890",
         "user": "U1234567890"
       }
     }
   }
   ```

3. **interactive** - Interactive component events
   ```json
   {
     "envelope_id": "xxx",
     "type": "interactive",
     "payload": {...}
   }
   ```

4. **slash_commands** - Slash command events
   ```json
   {
     "envelope_id": "xxx",
     "type": "slash_commands",
     "payload": {
       "command": "/status",
       "channel_id": "C1234567890",
       "user_id": "U1234567890"
     }
   }
   ```

5. **disconnect** - Disconnection notification
   ```json
   {"type":"disconnect","reason":"..."}
   ```

## Event Acknowledgment

All events with `envelope_id` must be acknowledged:

```cpp
if (auto envelope_id = event_parser::extract_envelope_id(message)) {
    std::string ack = "{\"envelope_id\":\"" + *envelope_id + "\"}";
    ws_client.send_message(ack);
}
```

## See Also

- [Slack API Overview](slack-api-overview.md) - Architecture overview
- [Slack API - Event Processing](slack-api-events.md) - Event processing details
- [Slack API - Implementation](slack-api-implementation.md) - Implementation details
