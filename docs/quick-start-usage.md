# Quick Start Guide - Usage

```bash
export SLACK_BOT_TOKEN=xoxb-your-token
./bin/neo-slack-bot
```

### Issue: "Failed to connect to Slack" or "403 Forbidden"

**Check**:
1. Both tokens are set (`SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN`)
2. Bot Token is correct (starts with `xoxb-`)
3. App-Level Token is correct (starts with `xapp-1-`)
4. App-Level Token has `connections:write` scope
5. Socket Mode is enabled in Slack app settings
6. Internet connection is working
7. No firewall blocking connection

**Solution**: 
- Verify both tokens are set: `echo $SLACK_BOT_TOKEN` and `echo $SLACK_APP_TOKEN`
- Check App-Level Token has `connections:write` scope in Slack app settings
- Verify Socket Mode is enabled in Slack app dashboard
- If tokens are in `~/.zshrc`, make sure to run `source ~/.zshrc` after adding them

### Issue: "No notifications appearing"

**Check**:
1. System Preferences → Notifications → Check app permissions
2. Do Not Disturb is not enabled
3. Notification Center is not disabled

**Solution**: Grant notification permissions in System Preferences.

### Issue: Build errors

**Solution**: See troubleshooting section in `installation.md`.

## Running in Background

### Using Terminal Multiplexer (tmux)

```bash
# Install tmux (if not installed)
brew install tmux

# Start new session
tmux new -s slack-bot

# Run bot
./bin/neo-slack-bot

# Detach: Press Ctrl+B, then D
# Reattach: tmux attach -t slack-bot
```

### Using nohup

```bash
nohup ./bin/neo-slack-bot > bot.log 2>&1 &
```

### Using launchd (macOS Service) - Recommended

**Automatic Installation** (recommended):
```bash
# Make sure tokens are set in ~/.zshrc first
echo 'export SLACK_BOT_TOKEN=xoxb-your-token' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-1-your-token' >> ~/.zshrc
source ~/.zshrc

# Install launchd service
cd /path/to/neo-slack-bot
./scripts/install-launchd.sh

# Load and start the service
launchctl load ~/Library/LaunchAgents/com.neo.slackbot.plist
launchctl start com.neo.slackbot
```

**Manual Installation**:
Create `~/Library/LaunchAgents/com.neo.slackbot.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.neo.slackbot</string>
    <key>ProgramArguments</key>
    <array>
        <string>/path/to/neo-slack-bot/src/bin/neo-slack-bot</string>
    </array>
    <key>WorkingDirectory</key>
    <string>/path/to/neo-slack-bot</string>
    <key>EnvironmentVariables</key>
    <dict>
        <key>PATH</key>
        <string>/usr/local/bin:/opt/homebrew/bin:/usr/bin:/bin:/usr/sbin:/sbin</string>
    </dict>
    <key>StandardOutPath</key>
    <string>~/Library/Logs/neo-slack-bot.log</string>
    <key>StandardErrorPath</key>
    <string>~/Library/Logs/neo-slack-bot-error.log</string>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
```

**Important**: Make sure `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN` are exported in `~/.zshrc` before installing the service.

**Service Management**:
```bash
# Check status
launchctl list | grep com.neo.slackbot

# View logs
tail -f ~/Library/Logs/neo-slack-bot.log
tail -f ~/Library/Logs/neo-slack-bot-error.log

# Stop service
launchctl stop com.neo.slackbot

# Start service
launchctl start com.neo.slackbot

# Uninstall service
./scripts/uninstall-launchd.sh
```

## Stopping the Bot

### Normal Shutdown

Press `Ctrl+C` in the terminal where the bot is running. The bot will gracefully shutdown within 1-2 seconds, closing WebSocket connections and cleaning up resources.

### If Running in Background

```bash
# Find process
ps aux | grep neo-slack-bot

# Kill process
kill <PID>

# Or by name
pkill neo-slack-bot
```

### If Running as Service

```bash
launchctl unload ~/Library/LaunchAgents/com.neo.slackbot.plist
```

## Next Steps

Now that your bot is running:

1. **Read Documentation**: Explore `docs/` directory for detailed guides
2. **Try Examples**: See `docs/examples.md` for example code and usage
3. **Customize Behavior**: See `docs/user-settings-basic.md` for configuration
4. **Develop Features**: Check `docs/rules.md` for development guidelines
5. **Follow Roadmap**: See `docs/roadmap.md` for upcoming features

## Getting Help

- **Documentation**: Check `docs/` directory
- **Getting Slack Token**: See **[Complete Guide: How to Get Slack API Token](getting-slack-token-detailed.md)** for detailed step-by-step instructions
- **Installation Issues**: See `docs/installation.md`
- **Authentication**: See `docs/auth-basic.md`
- **Development**: See `docs/rules.md` and `CLAUDE.md`

## Quick Reference

```bash
# Build (from project root)
cd /path/to/neo-slack-bot
mkdir -p build
cd build
cmake -G Ninja ..
ninja

# Run
export SLACK_BOT_TOKEN=xoxb-token
export SLACK_APP_TOKEN=xapp-1-your-app-token
./bin/neo-slack-bot

# Test
ctest --output-on-failure

# Clean
cd ..
rm -rf build
```

## See Also

- [Quick Start Guide - Setup](quick-start-setup.md) - Prerequisites and setup
- [Quick Start Guide](quick-start.md) - Overview
