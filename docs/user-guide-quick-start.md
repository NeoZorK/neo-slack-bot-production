# User Guide - Quick Start and Basic Usage

## Quick Start

### Prerequisites

Before using Neo Slack Bot, ensure you have:

1. ✅ macOS 10.14+ installed
2. ✅ Slack App created with Socket Mode enabled
3. ✅ Bot Token (`xoxb-...`) generated
4. ✅ App-Level Token (`xapp-...`) with `connections:write` scope
5. ✅ Bot built and executable available

### Initial Setup

1. **Set Environment Variables**:
   ```bash
   export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
   export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
   ```

2. **Verify Tokens**:
   ```bash
   echo $SLACK_BOT_TOKEN  # Should show: xoxb-...
   echo $SLACK_APP_TOKEN   # Should show: xapp-1-...
   ```

3. **Run the Bot**:
   ```bash
   cd src
   ./bin/neo-slack-bot
   ```

4. **Expected Output**:
   ```
   Starting Neo Slack Bot...
   Connecting to Slack Socket Mode...
   Connected to Slack Socket Mode
   ```

## Basic Usage

### Starting the Bot

**Foreground Mode** (for testing):
```bash
cd /path/to/neo-slack-bot/src
./bin/neo-slack-bot
```

**Background Mode** (using launchd):
```bash
./scripts/install-launchd.sh
launchctl start com.neo.slackbot
```

**View Logs**:
```bash
tail -f ~/Library/Logs/neo-slack-bot.log
```

### Stopping the Bot

**Foreground Mode**:
- Press `Ctrl+C` in the terminal
- The bot will gracefully shutdown within 1-2 seconds, closing WebSocket connections and cleaning up resources
- You should see "Application shutdown complete" message in the logs

**Background Mode**:
```bash
launchctl stop com.neo.slackbot
# Or uninstall:
./scripts/uninstall-launchd.sh
```

## Verifying Bot Operation

### 1. Check Connection Status

When the bot starts, you should see:
```
Connected to Slack Socket Mode
```

If you see connection errors, check:
- Both tokens are set correctly
- Socket Mode is enabled in Slack app settings
- Internet connection is working

### 2. Check Console Output

The bot logs all events to console. You should see:
```
[INFO] Event received: message
[INFO] Processing message from channel: C12345678
[INFO] Notification displayed
```

### 3. Check macOS Notifications

When a message arrives:
1. macOS notification should appear
2. Notification shows message text
3. Notification can be clicked to open Slack

**If notifications don't appear**:
- Check System Preferences → Notifications → Allow notifications
- Verify Do Not Disturb is not enabled
- Check notification permissions in System Preferences

## See Also

- [User Guide - Testing](user-guide-testing.md) - Testing message sending/receiving and notifications
- [User Guide - Configuration](user-guide-configuration.md) - Configuration and troubleshooting
- [User Guide - Advanced Usage](user-guide-advanced.md) - Advanced usage and best practices
- [Quick Start Guide](quick-start.md) - Quick start guide
