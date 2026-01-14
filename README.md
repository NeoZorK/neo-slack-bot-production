# Neo Slack Bot

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Version](https://img.shields.io/badge/version-v0.0.7-brightgreen.svg)](CHANGELOG.md)
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey.svg)](docs/docker-setup.md)
[![License](https://img.shields.io/badge/license-Private-red.svg)](LICENSE)

A high-performance, production-ready Slack bot built with C++17. Uses Slack's Socket Mode for real-time event processing and integrates with native notifications on macOS and console/log logging on Linux/Windows.

## Overview

Neo Slack Bot is a lightweight, efficient Slack bot implementation designed for cross-platform usage. It leverages WebSocket-based Socket Mode for real-time communication with Slack's API.

### Key Highlights

- **Real-time Processing**: WebSocket-based Socket Mode for instant event handling
- **Native Integration**: macOS system notifications for seamless user experience
- **High Performance**: C++17 implementation with minimal resource footprint
- **Robust Architecture**: Clean, modular design following SOLID principles
- **Production Ready**: Comprehensive error handling and reconnection logic

## Features

### Core Capabilities
- ✅ **Socket Mode Connection**: WebSocket-based real-time Slack connection
- ✅ **Event Processing**: Handle messages, mentions, and other Slack events
- ✅ **macOS Notifications**: Native system notification integration
- ✅ **HTTP API Support**: Full Slack API HTTP request capabilities
- ✅ **Error Recovery**: Automatic reconnection and error handling
- ✅ **Command Handling**: Slash command processing with built-in commands
- ✅ **Message Filtering**: Channel whitelist/blacklist and keyword filtering

### Advanced Features
- ✅ **User Preferences**: Comprehensive settings management
- ✅ **Event History**: Logging system with file rotation
- ✅ **Configuration Files**: JSON-based configuration support
- ✅ **Message Filtering**: Channel and keyword filtering
- ✅ **Command System**: Extensible slash command handling
- ✅ **Multi-platform support**: macOS, Linux (Docker), Windows (MinGW/Wine)

## Requirements

### Supported Platforms
- **macOS**: 12+ (Intel and Apple Silicon)
- **Linux**: Ubuntu 20.04+ (and compatible distros, via Docker)
- **Windows**: Windows 10+ (via MinGW build and tests under Wine/Docker)

### Runtime Requirements
- C++17-capable compiler (Clang, GCC, MinGW)
- CMake 3.15+ and Ninja
- OpenSSL development libraries

### Key Dependencies
- `cpp-httplib` – HTTP client/server
- `nlohmann/json` – JSON parsing
- `OpenSSL` – TLS for Slack API

Details and exact version recommendations are documented in the files under `docs/`.

## Installation

### macOS (native)

1. Install dependencies (via Homebrew):
   ```bash
   brew install openssl@3 cmake ninja
   ```
2. Build:
   ```bash
   mkdir -p build
   cd build
   cmake -G Ninja ..
   cmake --build .
   ```
3. Run tests:
   ```bash
   ctest --output-on-failure
   ```

### Linux & Windows (via Docker)

For consistent cross-platform builds, use the provided Dockerfile:

```bash
docker build -t neo-slack-bot:v0.0.7 .

# Run Linux tests inside container
docker run --rm neo-slack-bot:v0.0.7

# Run Windows tests under Wine
docker run --rm neo-slack-bot:v0.0.7 /bin/bash -c \
  "mkdir -p build_win && cd build_win && \
   cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake .. && \
   ninja && ninja test_windows_wine"
```

The main executable is available as `build/bin/neo-slack-bot` (Linux/macOS) and `build_win/bin/neo-slack-bot.exe` (Windows).

## Quick Start

### 1. Configure Slack App

1. Create a Slack App at [api.slack.com/apps](https://api.slack.com/apps)
2. Enable **Socket Mode** in your app settings
3. Create an **App-Level Token** with `connections:write` scope (required for Socket Mode v2)
4. Generate a **Bot Token** (starts with `xoxb-`) by installing the app to your workspace
5. Add necessary OAuth scopes for your use case

**⚠️ IMPORTANT**: Both Bot Token and App-Level Token are required. See [docs/getting-slack-token-detailed.md](docs/getting-slack-token-detailed.md) for detailed instructions.

### 2. Set Environment Variables

**Temporary Setup** (for testing):
```bash
export SLACK_BOT_TOKEN=xoxb-your-bot-token-here
export SLACK_APP_TOKEN=xapp-1-your-app-token-here
```

**Permanent Setup** (add to `~/.zshrc`):
```bash
echo 'export SLACK_BOT_TOKEN=xoxb-your-token' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-1-your-token' >> ~/.zshrc
source ~/.zshrc
```

### 3. Run the Bot
```bash
# From project root
./build/bin/neo-slack-bot
```

The bot will connect to Slack via Socket Mode v2 and start processing events. You should see connection confirmation in the console.

### 4. Verify Operation

- Send a test message in Slack
- Check console output for event processing
- Verify macOS notification appears

## Usage

### Basic Operation

The bot runs as a console application and processes Slack events in real-time:

- **Messages**: Processes incoming messages and triggers notifications
- **Mentions**: Handles direct mentions and @channel notifications
- **Events**: Processes various Slack events (reactions, channel updates, etc.)

### Configuration

Configuration is primarily done via environment variables:

- `SLACK_BOT_TOKEN`: Your Slack bot token (required, format: `xoxb-...`)
- `SLACK_APP_TOKEN`: Your App-Level token (required, format: `xapp-...`)

**⚠️ IMPORTANT**: Both tokens are required. Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error.

Additional configuration (paths, filters, timeouts) is available via JSON config files; see [docs/user-settings-advanced-config.md](docs/user-settings-advanced-config.md) for details.

### Stopping the Bot

Press `Ctrl+C` to gracefully shutdown the bot. The application will:
- Close WebSocket connection
- Clean up resources
- Exit cleanly

## Architecture

### Project Structure
```
neo-slack-bot/
├── docs/          # Documentation
│   ├── INDEX.md   # File registry
│   └── ...
├── src/           # Source code
│   ├── main.cpp
│   ├── websocket_client.*
│   ├── macos_notifications.*
│   └── ...
├── tests/         # Unit tests
└── README.md      # This file
```

### Key Components

- **WebSocketClient**: Manages Socket Mode connection and event streaming
- **MacOSNotifications**: Handles native macOS notification display
- **HttpClient**: Provides HTTP API client functionality
- **SlackAPI**: High-level Slack API wrapper

### Design Principles

- **Separation of Concerns**: Each module has a single, well-defined responsibility
- **RAII**: Resource management through smart pointers and RAII patterns
- **Error Handling**: Comprehensive error handling with appropriate recovery
- **Thread Safety**: Safe concurrent operations where needed

## Development

### Code Style

- **Naming**: snake_case for variables, functions, and classes
- **Standard**: C++17 minimum
- **Memory**: Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- **File Size**: Maximum 300 lines per file

See `docs/rules.md` for complete coding standards and architectural guidelines.

### Building & Testing (Developers)

The authoritative build instructions for contributors are in `docs/installation-build.md` and `docs/testing-guide.md`. In short:

```bash
mkdir -p build
cd build
cmake -G Ninja ..
cmake --build .
ctest --output-on-failure
```

Coverage and additional targets are described in [docs/coverage.md](docs/coverage.md).

## Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[INDEX.md](docs/INDEX.md)**: Complete file registry
- **[user-guide.md](docs/user-guide.md)**: Complete user guide with usage instructions
- **[examples.md](docs/examples.md)**: Examples guide - how to build and run example code
- **[manual-testing-guide.md](docs/manual-testing-guide.md)**: Step-by-step manual testing guide
- **[codebase-review.md](docs/codebase-review.md)**: Comprehensive codebase review report
- **[quick-start.md](docs/quick-start.md)**: Detailed quick start guide
- **[installation.md](docs/installation.md)**: Installation instructions
- **[tech-stack.md](docs/tech-stack.md)**: Technology stack details
- **[slack-api.md](docs/slack-api.md)**: Complete Slack API integration documentation
- **[roadmap.md](docs/roadmap.md)**: Project roadmap and version plan
- **[testing.md](docs/testing.md)**: Testing guidelines
- **[rules.md](docs/rules.md)**: Coding standards and architecture rules

## Contributing

This is a private project. For internal contributors:

1. Follow the coding standards in `docs/rules.md`
2. Ensure 100% test coverage for new features
3. Register all new files in `docs/INDEX.md`
4. Keep files under 300 lines
5. Write clear commit messages

## Troubleshooting

### Connection Issues

- **Verify Tokens**: Ensure both `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN` are set correctly
- **Check Socket Mode**: Verify Socket Mode is enabled in Slack app settings
- **Check App-Level Token**: Ensure App-Level Token has `connections:write` scope
- **Network**: Check internet connectivity and firewall settings
- **Logs**: Review console output for detailed error messages
- **Debug Mode**: Enable debug logging: `export DEBUG=1 && ./bin/neo-slack-bot`

### Build Issues

- **Compiler**: Ensure clang++ supports C++17 (`clang++ --version`)
- **OpenSSL**: Verify OpenSSL is installed (`brew list openssl`)
- **Dependencies**: Check that header-only libraries are accessible

### Runtime Issues

- **Permissions**: Ensure notification permissions are granted in System Preferences
- **Token Expiry**: Bot tokens don't expire, but verify token is valid
- **Rate Limits**: Be aware of Slack API rate limits

## Roadmap

See [docs/roadmap.md](docs/roadmap.md) for detailed version plan:

- **v0.0.1**: Initial setup and basic structure
- **v0.0.2**: WebSocket implementation and event handling
- **v0.0.3**: Command system and API integration
- **v0.0.4**: Settings management and filtering
- **v0.0.5**: Error recovery and performance optimization
- **v0.0.6**: Socket Mode v2, frame masking, metrics system, production-ready stability

## License

This is a private project. All rights reserved.

## Support

For issues and questions:
- Check documentation in `docs/` directory
- Review `docs/rules.md` for development guidelines
- Consult `CLAUDE.md` for AI assistant instructions

---

**Built with ❤️ using C++17**
