# Examples

This directory contains example code demonstrating how to use Neo Slack Bot.

## Building Examples

Examples are built automatically when you build the main project using CMake.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

The example binaries will be located in `build/bin/`.

## Running Examples

Run the examples from the project root directory.

### Basic Bot

Simple bot that connects and displays notifications:

```bash
export SLACK_BOT_TOKEN=xoxb-your-token
./build/bin/basic_bot
```

### Custom Handler

Bot with custom message filtering:

```bash
export SLACK_BOT_TOKEN=xoxb-your-token
./build/bin/custom_handler
```

### HTTP API Example

Send a message via HTTP API:

```bash
export SLACK_BOT_TOKEN=xoxb-your-token
./build/bin/http_api_example C1234567890 "Hello from bot!"
```

### Two Bots with Keychain

Two bots communicating in a test channel, with channel ID stored in macOS Keychain:

```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C1234567890
./build/bin/two_bots_keychain
```

## Example Descriptions

### basic_bot.cpp

Demonstrates:
- WebSocket client initialization
- Connection to Slack Socket Mode
- Basic message handling
- Notification display

### custom_handler.cpp

Demonstrates:
- Custom message filtering
- Conditional notification display
- Message processing logic

### http_api_example.cpp

Demonstrates:
- Slack API HTTP client usage
- Sending messages programmatically
- Error handling

### bot_network (Module)

Demonstrates:
- Creating network of multiple bots (default: 100 bots)
- **Unique WebSocket URLs per bot** (Socket Mode v2 requirement)
- Sending thousands of messages between bots via HTTP API
- Receiving messages via WebSocket (each bot has its own connection)
- Monitoring CPU, Memory, Network, I/O usage
- Stress testing the system
- Configurable via environment variables
- **Refactored structure**: Modular implementation in `examples/bot_network_module/`

**⚠️ IMPORTANT**: Each bot receives its own unique WebSocket URL through a separate call to `apps.connections.open`. This is required by Socket Mode v2 to ensure proper message delivery. Without unique URLs, bots may not receive messages correctly.

**Usage**:
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token  # Required for WebSocket URLs
export SLACK_TEST_CHANNEL=C12345678
export BOT_NETWORK_DURATION=300  # 5 minutes (optional, default: 60)
export BOT_NETWORK_MAX_MESSAGES=1000  # Max messages per bot (optional, default: 100)
./build/bin/bot_network [bot_count]  # Optional: number of bots (default: 100)
```

**WebSocket Connection Details**:
- Each bot calls `apps.connections.open` separately to get a unique URL
- Rate limiting delay (100ms) is applied between API calls
- Message handlers are set up before WebSocket connection
- Each bot can receive messages independently

**Monitoring**:
- CPU usage percentage
- Memory usage (MB)
- Messages sent/received counts
- Real-time statistics every 10 seconds

### channel_management.cpp

Demonstrates:
- Channel filtering (whitelist/blacklist)
- Keyword filtering (include/exclude)
- Message sending with different content types
- Command handling and registration
- Bot capabilities demonstration

**Usage**:
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C12345678  # Optional
./bin/channel_management
```

## Requirements

All examples require:
- `SLACK_BOT_TOKEN` environment variable set
- `SLACK_APP_TOKEN` environment variable set (for Socket Mode v2)
- Slack app with Socket Mode enabled (for WebSocket examples)
- Proper OAuth scopes configured
- For `bot_network`: `SLACK_TEST_CHANNEL` environment variable
