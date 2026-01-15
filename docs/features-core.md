# Features - Core Implementation Details

## Overview

This document provides detailed technical information about core Neo Slack Bot features, including implementation details and architecture.

## Core Features - Technical Details

### Socket Mode v2 Connection

**Technical Details**:
- Socket Mode v2 with dynamic URL retrieval
- RFC 6455 WebSocket protocol compliance
- Frame masking for client-sent frames (required by RFC 6455)
- SSL/TLS encryption
- Automatic reconnection on failure with exponential backoff (1s → 60s)
- Ping/pong keepalive mechanism (30s interval)
- Connection state management
- Proper handling of close frames (code 1002 protocol errors)

**Connection Flow**:
1. Call `apps.connections.open` API using App-Level Token
2. Parse response to extract `wss://` URL
3. Decode escaped slashes in URL
4. Establish TCP connection
5. Establish TLS connection
6. Perform WebSocket handshake (no Authorization header - ticket in URL)
7. Wait for `hello` event
8. Process events

**Reconnection Strategy**:
- Exponential backoff: 1s → 2s → 4s → 8s → 16s → 32s → 60s (capped)
- Infinite attempts with reset on successful connection
- Connection state tracking
- Graceful degradation

### Event Processing

**Event Types Supported**:
- `events_api` - Slack events (messages, app_mentions, etc.)
- `interactive` - Interactive component events
- `slash_commands` - Slash command events
- `hello` - Connection confirmation
- `disconnect` - Disconnection events

**Processing Pipeline**:
1. Receive WebSocket frame
2. Parse JSON payload
3. Extract event type
4. Route to appropriate handler
5. Process event
6. Send acknowledgment (if required)
7. Handle errors

**Future Enhancements**:
- Custom event handlers
- Event filtering rules
- Event history logging
- Event analytics

### macOS Notifications

**Implementation**:
- UserNotifications framework (macOS 10.14+)
- Native notification display
- System Notification Center integration
- Do Not Disturb compatibility

**Notification Features**:
- Custom notification content
- Sound and badge customization
- Action buttons (future)
- Rich media support (future)

**Integration**:
- System Notification Center
- Do Not Disturb compatibility
- Focus mode integration
- Notification grouping

### HTTP API Support

**API Methods**:
- `chat.postMessage` - Send messages ✅
- `conversations.info` - Get channel info ✅
- `users.info` - Get user info ✅
- `conversations.list` - List channels (future)
- `users.list` - List users (future)

**Implementation**:
- RESTful API wrapper using cpp-httplib
- Request/response handling
- Error handling and retries
- JSON escaping for safe data transmission
- URL parsing and header management

**Usage Examples**:
- Send messages programmatically
- Query Slack data
- Interact with Slack features
- Extend bot capabilities

## Advanced Features - Technical Details

### Message Filtering and Routing

**Implementation**:
- Channel whitelist/blacklist support
- Case-insensitive keyword matching
- Integrated into message processing pipeline
- User filtering support

**Filter Types**:
- Channel filter (whitelist/blacklist/all)
- Keyword filter (include/exclude)
- User filter (whitelist/blacklist/all)

**Future Enhancements**:
- Regex pattern matching
- Custom filter rules
- Advanced filtering logic

### Command Handling System

**Implementation**:
- CommandHandler class for command management
- Automatic routing of slash_commands events
- Extensible command registration system
- Built-in commands respond via Slack API

**Command Registration**:
```cpp
command_handler.register_command("status", [](const std::string& channel, 
                                                const std::vector<std::string>& args) {
    // Command implementation
});
```

**Future Enhancements**:
- Command permissions
- Command aliases
- Command help system expansion

### User Preferences Management

**Storage**:
- JSON configuration file
- Environment variable overrides
- Default values fallback

**Configuration Structure**:
- Notification preferences
- Filter rules
- Connection settings
- Display options (future)
- Advanced options (future)

**Future Enhancements**:
- Per-workspace settings
- Sync across devices
- Hot reload configuration

### Event History and Logging

**Implementation**:
- Logger singleton with thread-safe operations
- Automatic log file rotation (10 MB default)
- Timestamp formatting with milliseconds
- Configurable log levels and output destinations
- Log file location: ~/Library/Logs/neo-slack-bot.log

**Logging Options**:
- Console output ✅
- File logging ✅
- Remote logging (future)
- Structured logging (future)

**Log Levels**:
- DEBUG - Verbose logging
- INFO - Standard logging
- WARN - Warnings only
- ERROR - Errors only

### Error Recovery

**Implementation**:
- ErrorRecovery class tracks error counts and strategies
- Automatic retry with configurable delays
- Error state tracking per error type
- Integration with logging system

**Error Types**:
- Network errors
- Authentication errors
- API errors
- Parsing errors
- System errors
- Rate limit errors

**Recovery Strategies**:
- Retry with fixed delay
- Exponential backoff
- Fail fast (for auth errors)
- Ignore (for parsing errors)

### Rate Limiting

**Implementation**:
- Automatic rate limit detection from Slack API responses
- Request throttling (20 requests per minute default)
- Retry-after header parsing
- Thread-safe rate limit tracking

**Rate Limit Handling**:
- Detect rate limit responses
- Extract retry-after header
- Wait for specified duration
- Retry request automatically

## See Also

- [Features Overview](features-overview.md) - Quick overview of all features
- [Features - Technical Details](features-technical.md) - Technical implementation details
- [Features - Platform Details](features-platform.md) - Platform-specific features
