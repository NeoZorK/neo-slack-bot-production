# Examples Guide - Example Descriptions

- **Automatic backoff**: Exponential backoff on rate limit errors
- **Staggered start**: Bots start with small delays to avoid thundering herd
- **Thread-safe**: Uses mutex and condition_variable for efficient coordination

**Monitoring**:
- CPU usage percentage
- Memory usage (MB)
- Messages sent/received counts
- Rate limit error counts
- Real-time statistics every 10 seconds

**Usage Example**:
```bash
# Run network of 50 bots for 5 minutes
export SLACK_TEST_CHANNEL=C12345678
export BOT_NETWORK_DURATION=300
./bin/bot_network 50

# Run with fewer bots to reduce rate limiting
./bin/bot_network 10

# Run network of 10 bots with max 500 messages per bot
export SLACK_TEST_CHANNEL=C12345678
export BOT_NETWORK_MAX_MESSAGES=500
./bin/bot_network 10
```

### 5. Channel Management

Example of channel management with filtering and command handling.

**Run**:
```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C12345678  # Optional
./bin/channel_management
```

**Demonstrates**:
- Channel filtering (whitelist/blacklist)
- Keyword filtering (include/exclude)
- Message sending with different content types
- Command handling and registration
- Bot capabilities demonstration

## Example Descriptions

### basic_bot.cpp

**Purpose**: Basic example of using Neo Slack Bot.

**Demonstrates**:
- WebSocket client initialization
- Connection to Slack Socket Mode
- Basic message handling
- Notification display

**Usage**: Perfect for getting started with the library.

### custom_handler.cpp

**Purpose**: Example of creating custom message handler.

**Demonstrates**:
- Custom message filtering
- Conditional notification display
- Message processing logic

**Usage**: For learning advanced message processing.

### http_api_example.cpp

**Purpose**: Example of using Slack HTTP API.

**Demonstrates**:
- Slack API HTTP client usage
- Programmatic message sending
- Error handling

**Usage**: For sending messages without WebSocket connection.

### two_bots_keychain.cpp

**Purpose**: Example of two bots communicating in a test channel with channel ID stored securely in macOS Keychain.

**Demonstrates**:
- Using `MacOSKeychainImpl` for secure storage
- Loading channel ID from Keychain with `SLACK_TEST_CHANNEL` fallback
- Running two `WebSocketClient` instances in parallel
- Sending messages via HTTP API and receiving via Socket Mode

**Usage**:
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C1234567890
./build/bin/two_bots_keychain
```

**Notes**:
- On first run, the example will read `SLACK_TEST_CHANNEL` and store the channel ID in the macOS Keychain under the `neo-slack-bot` service.
- Subsequent runs will reuse the stored channel ID, even if `SLACK_TEST_CHANNEL` is not set.
- Bot 1 sends a small number of messages (`Hello from Bot 1 (...)`) via `SlackAPI`, and Bot 2 listens on WebSocket and logs any received messages containing this text.
- Make sure that:
  - The bot is invited to the test channel.
  - The bot token has `chat:write` scope.
  - Socket Mode is enabled and the app token has `connections:write` scope.

### bot_network.cpp

**Purpose**: Stress testing the system with multiple bot connections.

**Demonstrates**:
- Creating network of multiple bots
- **Unique WebSocket URLs per bot** (Socket Mode v2 requirement)
- Sending thousands of messages via HTTP API
- Receiving messages via WebSocket (each bot has its own connection)
- System resource monitoring
- Configuration via environment variables
- Global rate limiting coordination

**Key Implementation Details**:
- Each bot receives its own unique WebSocket URL through separate `apps.connections.open` calls
- This ensures proper message delivery in Socket Mode v2
- Rate limiting delay (100ms) between URL retrieval calls prevents API throttling
- Message handlers are configured before WebSocket connection establishment

**Usage**: For testing performance, stability, and message receiving capabilities.

### channel_management.cpp

**Purpose**: Example of channel management and filtering.

**Demonstrates**:
- Channel filtering (whitelist/blacklist)
- Keyword filtering
- Message sending
- Command handling

**Usage**: For learning filtering and channel management capabilities.

## Troubleshooting

### Error: "SLACK_BOT_TOKEN not set"

**Solution**:
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
```

**Verify**:
```bash
echo $SLACK_BOT_TOKEN  # Should show: xoxb-...
echo $SLACK_APP_TOKEN   # Should show: xapp-1-...
```

### Build Error: "Cannot find object files"

**Cause**: Project was not configured/built with CMake.

**Solution**:
```bash
cd /path/to/neo-slack-bot
mkdir -p build
cd build
cmake -G Ninja ..
ninja
```

### Error: "Failed to connect to Slack" or "403 Forbidden"

**Check**:
1. Both tokens are set (`SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN`)
2. Bot Token is correct (starts with `xoxb-`)
3. App-Level Token is correct (starts with `xapp-1-`)
4. App-Level Token has `connections:write` scope
5. Socket Mode is enabled in Slack app settings
6. Internet connection is working

**Solution**: See [getting-slack-token-detailed.md](getting-slack-token-detailed.md) for detailed instructions.

### Error: "No such file or directory: bin/basic_bot"

**Cause**: Examples are not built.

**Solution**:
```bash
cd /path/to/neo-slack-bot
mkdir -p build
cd build
cmake -G Ninja ..
ninja
```

### Compilation Error: "OpenSSL not found"

**Solution**:
```bash
# For Apple Silicon (M1/M2/M3)
brew install openssl@3

# For Intel Macs
brew install openssl
```

The Makefile will automatically detect installed OpenSSL.

### Error: "Permission denied"

**Solution**:
```bash
chmod +x examples/bin/*
```

## Additional Information

- **Example Source Code**: `examples/`
- **Examples README**: `examples/README.md`
- **Quick Start**: [quick-start.md](quick-start.md)
- **Getting Tokens**: [getting-slack-token-quick.md](getting-slack-token-quick.md)
- **User Guide**: [user-guide.md](user-guide.md)

## Next Steps

After studying the examples:

1. **Study Source Code**: Look at implementation in `src/`
2. **Create Your Bot**: Use examples as a foundation
3. **Read Documentation**: See other files in `docs/`
4. **Run Tests**: See [testing-guide.md](testing-guide.md)

## See Also

- [Examples Guide - Building and Running](examples-building.md) - Building and running examples
- [Examples Guide](examples.md) - Overview
