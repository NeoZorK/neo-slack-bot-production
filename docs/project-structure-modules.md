# Project Structure - Module Details

## Overview

This document provides detailed information about project modules, their organization, and responsibilities.

## Module Structure Details

### WebSocket Client Module

**Files**: 
- `websocket_client.hpp` - Main WebSocket client interface
- `websocket_client.cpp` - Main WebSocket client implementation
- `websocket_client_connection.cpp` - Connection management logic
- `websocket_client_messaging.cpp` - Message sending/receiving logic
- `websocket_connection.hpp` - Connection abstraction
- `websocket_connection.cpp` - Connection implementation
- `websocket_frame.hpp` - Frame handling
- `websocket_frame.cpp` - Frame implementation

**Responsibilities**:
- WebSocket connection management
- Message sending/receiving
- Connection state handling
- Reconnection logic
- Frame encoding/decoding

**Module Structure**:
- **Main Client** (`websocket_client.cpp`): Core class implementation, event loop, settings
- **Connection** (`websocket_client_connection.cpp`): URL parsing, TCP/TLS connection, handshake
- **Messaging** (`websocket_client_messaging.cpp`): Message encoding/decoding, frame handling

**Dependencies**: OpenSSL, system sockets

### macOS Notifications Module

**Files**: `macos_notifications.hpp`, `macos_notifications.cpp`

**Responsibilities**:
- Native notification display
- Notification configuration
- System integration
- Permission handling

**Dependencies**: macOS UserNotifications framework

### HTTP Client Module

**Files**: `http_client.hpp`, `http_client.cpp`

**Responsibilities**:
- HTTP request/response handling
- SSL/TLS support
- Header management
- Error handling

**Dependencies**: cpp-httplib, OpenSSL

### Slack API Module

**Files**: `slack_api.hpp`, `slack_api.cpp`

**Responsibilities**:
- Slack API wrapper
- Request construction
- Response parsing
- Error handling

**Dependencies**: HTTP client, nlohmann/json

### Event Parser Module

**Files**: `event_parser.hpp`, `event_parser.cpp`

**Responsibilities**:
- Event parsing and validation
- Type-safe event handling
- Event routing

### Message Filter Module

**Files**: `message_filter.hpp`, `message_filter.cpp`

**Responsibilities**:
- Channel filtering
- Keyword filtering
- User filtering
- Filter rule application

### Command Handler Module

**Files**: `command_handler.hpp`, `command_handler.cpp`

**Responsibilities**:
- Command registration
- Command routing
- Argument parsing
- Command execution

### Settings Module

**Files**: `settings.hpp`, `settings.cpp`

**Responsibilities**:
- Settings management
- Configuration loading
- Default values

### Config Loader Module

**Files**: `config_loader.hpp`, `config_loader.cpp`

**Responsibilities**:
- Configuration file loading
- Environment variable parsing
- Configuration merging

### Logger Module

**Files**: `logger.hpp`, `logger.cpp`

**Responsibilities**:
- Logging operations
- Log file management
- Log rotation
- Thread-safe logging

### Error Recovery Module

**Files**: `error_recovery.hpp`, `error_recovery.cpp`

**Responsibilities**:
- Error categorization
- Recovery strategies
- Error state tracking
- Retry logic

### Rate Limiter Module

**Files**: `rate_limiter.hpp`, `rate_limiter.cpp`

**Responsibilities**:
- Rate limit detection
- Request throttling
- Thread-safe rate limiting

### Metrics Module

**Files**: `metrics.hpp`, `metrics.cpp`

**Responsibilities**:
- Performance metrics
- Event counting
- Latency tracking
- Memory usage tracking

### Input Validator Module

**Files**: `input_validator.hpp`, `input_validator.cpp`

**Responsibilities**:
- Input validation
- Format checking
- Sanitization
- Security validation

### JSON Parser Module

**Files**: `json_parser.hpp`, `json_parser.cpp`

**Responsibilities**:
- JSON parsing utilities
- Type conversions
- Error handling

**Dependencies**: nlohmann/json

## See Also

- [Project Structure Overview](project-structure-overview.md) - Quick overview of project structure
- [Project Structure - Build Details](project-structure-build.md) - Build artifacts and organization
- [Project Structure - Dependencies](project-structure-dependencies.md) - Dependency management
