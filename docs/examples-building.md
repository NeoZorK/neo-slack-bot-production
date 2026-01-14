# Examples Guide - Building and Running

# Examples Guide

This guide describes how to build and run Neo Slack Bot examples.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Building Examples](#building-examples)
3. [Running Examples](#running-examples)
4. [Example Descriptions](#example-descriptions)
5. [Troubleshooting](#troubleshooting)

## Prerequisites

Before building and running examples, ensure that:

- ✅ Main project is built (build completed in `src/`)
- ✅ OpenSSL is installed (via Homebrew)
- ✅ clang++ compiler with C++17 support is installed
- ✅ Slack tokens are configured (see [getting-slack-token-quick.md](getting-slack-token-quick.md))

### Setting Up Tokens

All examples require environment variables to be set:

```bash
export SLACK_BOT_TOKEN=xoxb-your-bot-token-here
export SLACK_APP_TOKEN=xapp-1-your-app-token-here
```

**⚠️ IMPORTANT**: Both tokens are required. Without `SLACK_APP_TOKEN`, connection will fail.

## Building Examples

### Step 1: Configure Project with CMake + Ninja

Examples are built together with the main project from the project root:

```bash
cd /path/to/neo-slack-bot
mkdir -p build
cd build
cmake -G Ninja ..
```

### Step 2: Build All Targets (Including Examples)

```bash
ninja
```

All example binaries will be created in `build/bin/`:
- `build/bin/basic_bot`
- `build/bin/custom_handler`
- `build/bin/http_api_example`
- `build/bin/channel_management`
- `build/bin/add_bot_to_channel`
- `build/bin/bot_network`
- `build/bin/two_bots_keychain`

### Cleaning Build Artifacts

To clean compiled files, you can remove the build directory:

```bash
cd /path/to/neo-slack-bot
rm -rf build
```

## Running Examples

All executables are located in `examples/bin/`.

### 1. Basic Bot

Simplest example of connecting to Slack and displaying notifications.

**Run**:
```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
./bin/basic_bot
```

**Demonstrates**:
- WebSocket client initialization
- Connection to Slack Socket Mode
- Basic message handling
- macOS notification display

**Expected Behavior**:
- Bot connects to Slack
- On receiving a message, prints it to console
- Shows macOS notification

### 2. Custom Handler

Example with custom message filtering.

**Run**:
```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
./bin/custom_handler
```

**Demonstrates**:
- Custom message filtering
- Conditional notification display
- Message processing logic

### 3. HTTP API Example

Example of sending messages via Slack HTTP API.

**Run**:
```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
./bin/http_api_example C1234567890 "Hello from bot!"
```

**Parameters**:
- First argument: Channel ID (e.g., `C1234567890`)
- Second argument: Message text

**Demonstrates**:
- Slack API HTTP client usage
- Programmatic message sending
- Error handling

**Usage Example**:
```bash
# Send message to channel
./bin/http_api_example C1234567890 "Test message"

# Send message to different channel
./bin/http_api_example C9876543210 "Another test"
```

### 4. Bot Network

Example of creating a network of multiple bots for stress testing with **global rate limiting** to prevent exceeding Slack API limits.

**⚠️ IMPORTANT - Socket Mode v2 Requirement**: Each bot in the network receives its own **unique WebSocket URL** via a separate call to `apps.connections.open`. This is required by Socket Mode v2 to ensure proper message delivery. Without unique URLs, bots may not receive messages correctly.

**Run**:
```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C12345678
export BOT_NETWORK_DURATION=300  # Optional: duration in seconds (default: 60)
export BOT_NETWORK_MAX_MESSAGES=1000  # Optional: max messages per bot (default: 100)
./bin/bot_network [bot_count]  # Optional: number of bots (default: 100)
```

**Environment Variables**:
- `SLACK_TEST_CHANNEL` (required): Test channel ID
- `SLACK_APP_TOKEN` (required): App-Level Token for getting WebSocket URLs
- `BOT_NETWORK_DURATION` (optional): Test duration in seconds (default: 60)
- `BOT_NETWORK_MAX_MESSAGES` (optional): Max messages per bot (default: 100)

**Command Line Parameters**:
- `bot_count` (optional): Number of bots in network (default: 100)

**Demonstrates**:
- Creating network of multiple bots (default: 100 bots)
- **Unique WebSocket URLs** - each bot gets its own URL via `apps.connections.open` (Socket Mode v2 requirement)
- **Global rate limiting** - coordinated rate limiting across all bots
- Sending messages while respecting Slack API rate limits (~1 msg/sec per channel)
- Receiving messages via WebSocket (each bot has its own connection)
- Monitoring CPU, Memory, Network, I/O usage
- Stress testing the system
- Configuration via environment variables
- Automatic handling of rate limit errors with exponential backoff

**WebSocket Connection Details**:
- Each bot calls `apps.connections.open` separately to get a unique WebSocket URL
- Rate limiting delay (100ms) is applied between API calls to avoid hitting Slack limits
- Message handlers are set up before WebSocket connection to ensure proper event processing
- Each bot can receive messages independently through its own WebSocket connection

**Rate Limiting Features**:
- **Global coordination**: All bots share a single rate limiter to prevent exceeding limits
- **Per-channel tracking**: Rate limits tracked per channel independently

## See Also

- [Examples Guide - Descriptions](examples-descriptions.md) - Example descriptions
- [Examples Guide](examples.md) - Overview
