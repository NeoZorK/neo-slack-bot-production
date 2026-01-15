# Slack API - Event Processing

## Event Flow

```
WebSocket Message
    │
    ├─► EventParser::parse_type()
    │   └─► Returns event_type (hello, events_api, interactive, slash_commands, disconnect)
    │
    ├─► Extract envelope_id (if present)
    │   └─► Send acknowledgment
    │
    └─► Route to Handler
        ├─► hello → Update connection state
        ├─► events_api → Process Slack event (message, mention, etc.)
        ├─► interactive → Handle interactive component
        ├─► slash_commands → Execute command handler
        └─► disconnect → Cleanup and reconnect
```

## Event Parsing

**File**: `src/event_parser.cpp`

```cpp
event_type parse_type(const std::string& message) {
    // Parse JSON and extract "type" field
    // Return enum value
}

std::optional<std::string> extract_envelope_id(const std::string& message) {
    // Extract "envelope_id" from JSON
    // Return optional string
}

bool is_message_event(const std::string& message) {
    // Check if event type is "message"
}

std::optional<std::string> extract_message_text(const std::string& message) {
    // Extract message text from nested JSON structure
}
```

## Message Processing

**File**: `src/main.cpp`

```cpp
ws_client.set_message_handler([&](const std::string& message) {
    auto event_type = event_parser::parse_type(message);
    
    if (event_type == event_parser::event_type::hello) {
        // Connection established
        metrics.increment_event_count("hello");
        return;
    }
    
    if (event_type == event_parser::event_type::events_api) {
        // Acknowledge event
        auto envelope_id = event_parser::extract_envelope_id(message);
        if (envelope_id) {
            ws_client.send_message("{\"envelope_id\":\"" + *envelope_id + "\"}");
        }
        
        // Process message event
        if (event_parser::is_message_event(message)) {
            auto text = event_parser::extract_message_text(message);
            auto channel = event_parser::extract_channel_id(message);
            // ... process message
        }
    }
    
    // ... handle other event types
});
```

## See Also

- [Slack API Overview](slack-api-overview.md) - Architecture overview
- [Slack API - Socket Mode](slack-api-socket-mode.md) - Socket Mode event types
- [Slack API - Implementation](slack-api-implementation.md) - Implementation details
