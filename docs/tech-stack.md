# Tech Stack

## Overview

Neo Slack Bot is built using modern C++17 with a carefully selected set of libraries that provide high performance, reliability, and ease of integration. The technology choices prioritize header-only libraries where possible to minimize build complexity.

## Core Language

### C++17 Standard
- **Version**: C++17 (ISO/IEC 14882:2017)
- **Compiler**: clang++ with full C++17 support
- **Rationale**: Modern C++ features provide better performance, safety, and expressiveness

**Key C++17 Features Used**:
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Lambda expressions and closures
- `auto` type deduction
- Range-based for loops
- `constexpr` for compile-time evaluation
- Structured bindings
- `std::optional` for nullable types
- `std::variant` for type-safe unions
- Filesystem library (`std::filesystem`)

**Benefits**:
- Memory safety through RAII and smart pointers
- Type safety with strong typing
- Performance optimization opportunities
- Modern, maintainable code patterns

## HTTP Library

### cpp-httplib
- **Version**: Latest stable (header-only)
- **License**: MIT
- **Type**: Header-only library
- **Repository**: https://github.com/yhirose/cpp-httplib

**Features**:
- Lightweight HTTP client/server implementation
- SSL/TLS support via OpenSSL
- Simple, intuitive API
- No external dependencies (except OpenSSL)
- Cross-platform compatibility

**Usage in Project**:
- Slack API HTTP requests (`chat.postMessage`, `conversations.info`, etc.)
- RESTful API interactions
- Webhook handling (future)
- File uploads/downloads (future)

**Integration**:
- Included as header file in project
- No build configuration needed
- Minimal overhead

## JSON Processing

### nlohmann/json
- **Version**: Latest stable (v3.x)
- **License**: MIT
- **Type**: Header-only library
- **Repository**: https://github.com/nlohmann/json

**Features**:
- Modern C++ API with intuitive syntax
- High performance parsing
- Type-safe conversions
- Support for custom types
- Exception-based error handling
- JSON Schema validation support

**Usage in Project**:
- Parsing Slack API responses
- Constructing API request payloads
- Configuration file parsing (future)
- Event data serialization/deserialization

**Example Usage**:
```cpp
json j = json::parse(slack_response);
std::string text = j["message"]["text"];
```

**Benefits**:
- Type-safe JSON handling
- Easy integration with C++ types
- Excellent error messages
- Comprehensive documentation

## WebSocket Implementation

### Socket Mode (Slack Protocol)
- **Protocol**: RFC 6455 (WebSocket)
- **Connection**: `wss://slack.com/socket-mode`
- **Format**: JSON message encoding
- **Authentication**: Bot token in connection handshake

**Features**:
- Real-time bidirectional communication
- Event streaming from Slack
- Automatic reconnection support
- Ping/pong keepalive mechanism
- Message acknowledgment system

**Implementation Approach**:
- Custom WebSocket client using OpenSSL
- Event-driven message processing
- Thread-safe message queue
- Connection state management

**Message Types**:
- `events_api` - Slack events
- `interactive` - Interactive components
- `slash_commands` - Slash commands
- `hello` - Connection confirmation
- `disconnect` - Disconnection notice

## Security

### OpenSSL
- **Version**: System-provided (1.1.x or 3.x)
- **License**: Apache 2.0 / OpenSSL License
- **Type**: System library
- **Installation**: Via Homebrew (`brew install openssl`)

**Usage**:
- SSL/TLS encryption for WebSocket connections
- Certificate validation
- Secure HTTP (HTTPS) connections
- Cryptographic operations (if needed)

**Security Features**:
- TLS 1.2+ support
- Certificate chain validation
- Secure random number generation
- Cryptographic hash functions

**Configuration**:
- System certificate store integration
- Custom CA bundle support (if needed)
- TLS version negotiation

## Build System

### Make
- **Version**: GNU Make or BSD Make
- **Purpose**: Build automation and dependency management
- **Configuration**: `src/Makefile`

**Features**:
- Dependency tracking
- Parallel builds support
- Debug and release configurations
- Clean targets
- Test integration

**Build Targets**:
- `make` - Release build with optimizations
- `make debug` - Debug build with symbols
- `make clean` - Remove build artifacts
- `make test` - Run test suite

### Compiler: clang++
- **Version**: clang 10.0+ with C++17 support
- **Flags**: `-std=c++17 -Wall -Wextra -O2`
- **Debug Flags**: `-g -DDEBUG`

**Compiler Features Used**:
- Link-time optimization (LTO) support
- Sanitizers (AddressSanitizer, ThreadSanitizer)
- Static analysis capabilities
- Modern warning diagnostics

## Platform

### macOS
- **Minimum Version**: macOS 10.14 (Mojave)
- **Target Version**: macOS 11+ (Big Sur and later)
- **Architecture**: x86_64 and ARM64 (Apple Silicon)

**macOS-Specific Features**:
- Native notification system (UserNotifications framework)
- System integration (menu bar, dock)
- Background execution support
- Keychain integration (for secure token storage - future)

**Notification System**:
- UserNotifications framework (macOS 10.14+)
- Rich notifications with actions
- Sound and badge customization
- Do Not Disturb integration

## Development Tools

### Version Control
- **Git**: Distributed version control
- **GitHub**: Repository hosting
- **Workflow**: Feature branches, semantic versioning

### Code Quality
- **Linting**: clang-tidy (future)
- **Formatting**: clang-format (future)
- **Static Analysis**: Built-in compiler warnings
- **Memory Checking**: Valgrind / AddressSanitizer

## Dependency Management Strategy

### Header-Only Libraries
- Included directly in project
- No build configuration needed
- Version controlled with project
- Easy to update

### System Libraries
- Managed via package manager (Homebrew)
- Documented in installation guide
- Version requirements specified
- Fallback options considered

### Future Considerations
- Dependency version pinning
- Submodule or package manager integration
- Automated dependency updates
- License compliance tracking

## Performance Characteristics

### Memory Usage
- Minimal overhead from header-only libraries
- Smart pointer usage prevents leaks
- Efficient container usage
- RAII ensures proper cleanup

### CPU Usage
- Event-driven architecture minimizes polling
- Efficient JSON parsing
- Optimized HTTP client
- Low background CPU usage

### Network Efficiency
- WebSocket reduces HTTP overhead
- Persistent connections
- Efficient message batching (future)
- Compression support (future)
