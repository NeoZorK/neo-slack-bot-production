# Changelog

All notable changes to Neo Slack Bot will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.7] - 2026-01-14

### Added
- **Cross-Platform Support**
  - Full Linux support via Docker
  - Windows support via MinGW cross-compilation and Wine testing
  - Platform-specific configuration paths (AppData on Windows, XDG on Linux)
- **Docker Environment**
  - Unified Dockerfile for Linux and Windows builds
  - Integrated `wine32` and `xvfb` for headless Windows testing
  - Pre-configured MinGW toolchain and OpenSSL for Windows
- **Documentation**
  - Production checklists for macOS, Linux, and Windows
  - Detailed Docker setup and usage guides
  - Updated rules and architecture documentation
- **Build System**
  - Static linking for Windows binaries (`-static-libgcc -static-libstdc++`)
  - Automated cross-platform testing targets

### Changed
- ConfigLoader now detects platform-specific paths automatically
- Logger improvements for cross-platform file handling
- CMake configuration refactored for better cross-compilation support

### Fixed
- Fixed "Broken pipe" error in logger tests on Wine
- Resolved missing DLL dependencies for Windows builds
- Fixed configuration path resolution for non-standard environments

## [0.0.6] - 2026-01-14

### Added
- Watchdog timer for hang detection
  - Automatic reconnection if no activity detected for 5 minutes
  - Background thread monitoring application health
  - Activity timestamp tracking for all WebSocket events
  - Configurable timeout (default: 300 seconds)
- Production-ready stability improvements
  - Enhanced error handling and recovery
  - Better resource cleanup on shutdown
  - Improved signal handling
- Architectural improvements
  - Module separation: websocket_connection extracted from websocket_client
  - Test module separation: stress tests split into logical modules
  - Interface abstractions: IHttpClient and IWebSocketClient interfaces
  - Mock framework: MockHttpClient and MockWebSocketClient for testing
- Performance metrics and monitoring system
  - Event counting (messages, notifications, errors)
  - Latency tracking (message processing, notification display)
  - Error rate monitoring
  - Memory usage tracking
  - Metrics summary command (`/metrics`)
- Integration tests
  - WebSocket message flow tests
  - HTTP client mock tests
  - Error handling integration tests
- CI/CD improvements
  - File size checking
  - File registration verification
  - Enhanced static analysis
  - Coverage generation support

### Changed
- Version updated to v0.0.6
- Main application now includes watchdog timer thread
- Activity tracking integrated into message handler
- Improved shutdown sequence with proper cleanup
- WebSocket client refactored for better modularity
- Stress tests reorganized into connection, components, and system modules
- CI/CD pipeline enhanced with additional quality checks

### Fixed
- Fixed missing `<optional>` include in main.cpp
- Improved thread safety in watchdog implementation
- Better error messages and logging
- Fixed file size violations (websocket_client.cpp, test_stress.cpp)

## [0.0.5] - 2024-XX-XX

### Added
- Input validation system
  - Token format validation (BOT_TOKEN_PLACEHOLDER prefix, length checks)
  - Channel ID validation (C/D/G prefix, 9 characters)
  - User ID validation (U prefix, 11 characters)
  - Message text validation (length limits, 4000 chars max)
  - Input sanitization functions
  - Comprehensive validation tests
- Comprehensive logging system
  - Log levels: DEBUG, INFO, WARN, ERROR
  - File logging with automatic rotation
  - Console output with configurable levels
  - Timestamp formatting for all log messages
  - Thread-safe logging operations
- Advanced error recovery system
  - Error categorization (Network, Authentication, API, Parsing, System, Rate Limit)
  - Configurable recovery strategies (Retry, Exponential Backoff, Fail Fast, Ignore)
  - Error counting and tracking
  - Automatic retry with exponential backoff
- Rate limiting handling
  - Automatic rate limit detection from Slack API responses
  - Request throttling (20 requests per minute default)
  - Retry-after header parsing
  - Thread-safe rate limit tracking
- Performance optimizations
  - Move semantics in API requests
  - Improved error handling with retry logic
  - Better resource management

### Changed
- SlackAPI now uses rate limiter and error recovery
  - Automatic retry on rate limit errors
  - Exponential backoff for failed requests
  - Better error categorization and handling
- WebSocketClient integrated with logging system
  - All connection events logged
  - Reconnection attempts logged with details
  - Error messages logged with appropriate levels
- Main application uses logging instead of cout/cerr
  - All output goes through logger
  - Log file location: ~/Library/Logs/neo-slack-bot.log
  - Configurable log levels
- Enhanced security with input validation
  - All API inputs validated before processing
  - Invalid inputs rejected with appropriate error messages
  - Token format validated on startup
  - Channel/user IDs validated before API calls

### Fixed
- Fixed get_channel_info and get_user_info to properly return response data
- Improved error handling in API requests
- Better connection state tracking

## [0.0.4] - 2024-XX-XX

### Added
- User settings management system
  - Settings class for structured configuration
  - Support for notification preferences (enabled, sound, dnd_hours)
  - Connection settings (max_retries, timeout, ping_interval, retry_delay)
  - Event filtering settings (message_types, keyword_filter, user_filter)
- Configuration file support
  - JSON configuration file at `~/Library/Application Support/neo-slack-bot/config.json`
  - Automatic creation of default config file
  - ConfigLoader class for loading and merging settings
  - Environment variable overrides (NEO_SLACK_*)
- Enhanced MessageFilter
  - User filtering support (whitelist/blacklist)
  - Keyword filter modes (include/exclude)
  - Integration with settings system
- Enhanced MacOSNotifications
  - Settings-based configuration
  - DND hours support
  - Notification enable/disable control
- Enhanced WebSocketClient
  - Configurable connection parameters from settings
  - Customizable reconnection attempts and delays
  - Configurable ping interval
- JSON parser utilities
  - Simple JSON parsing without external dependencies
  - Support for strings, booleans, integers, and arrays

### Changed
- MessageFilter now supports user filtering
- MacOSNotifications respects DND hours and enabled settings
- WebSocketClient uses settings for connection parameters
- Main application loads and applies settings on startup
- Configuration priority: environment variables > config file > defaults

### Fixed
- File size compliance (split config_loader into smaller modules)

## [0.0.3] - 2024-XX-XX

### Added
- HTTP API integration using cpp-httplib
- Complete Slack API wrapper implementation
  - `send_message()` - Send messages to channels
  - `get_channel_info()` - Get channel information
  - `get_user_info()` - Get user information
- Command handling system for slash commands
  - Command registration and routing
  - Built-in commands: `/status`, `/help`
  - Extensible command architecture
- Message filtering system
  - Channel whitelist/blacklist filtering
  - Keyword-based filtering
  - Configurable filter rules
- Event type routing for all Socket Mode events
- cpp-httplib library integration (header-only)

### Changed
- HttpClient now uses cpp-httplib for HTTP requests
- SlackAPI methods now fully functional
- Message processing now respects filter rules
- Slash commands are automatically routed to handlers

### Fixed
- JSON escaping in Slack API requests
- HTTP client URL parsing
- Command argument parsing

## [0.0.2] - 2024-XX-XX

### Added
- Automatic WebSocket reconnection with exponential backoff
- Full Socket Mode event support (events_api, interactive, slash_commands)
- Event acknowledgment system using envelope_id
- Improved error handling and recovery mechanisms
- Connection state management
- Reconnection attempt limiting (max 10 attempts)

### Changed
- WebSocket client now automatically reconnects on connection loss
- Event handlers now send acknowledgment responses
- Improved connection lifecycle management
- Enhanced error messages and logging

### Fixed
- Connection cleanup on errors
- SSL context management during reconnection
- Message handling during connection state changes

## [0.0.1] - 2024-XX-XX

### Added
- Initial project structure
- Documentation framework
- Build system (Makefile)
- Basic source code stubs:
  - WebSocket client interface
  - macOS notifications interface
  - HTTP client interface
  - Slack API wrapper interface
- Test framework structure
- Comprehensive documentation:
  - Installation guide
  - Quick start guide
  - Technical documentation
  - Architecture guidelines
- Code quality tools:
  - `.clang-format` configuration
  - `.clang-tidy` configuration
- CI/CD setup (GitHub Actions)
- Contributing guidelines
- License file (MIT)

### Documentation
- Complete documentation in `docs/` directory
- README with professional formatting
- CLAUDE.md for AI assistant instructions
- Rules and guidelines documentation

### Infrastructure
- Git repository initialization
- GitHub repository creation
- Branch structure (v0.0.1)
- `.gitignore` configuration

[Unreleased]: https://github.com/NeoZorK/neo-slack-bot/compare/v0.0.3...HEAD
[0.0.3]: https://github.com/NeoZorK/neo-slack-bot/compare/v0.0.2...v0.0.3
[0.0.2]: https://github.com/NeoZorK/neo-slack-bot/compare/v0.0.1...v0.0.2
[0.0.1]: https://github.com/NeoZorK/neo-slack-bot/releases/tag/v0.0.1
