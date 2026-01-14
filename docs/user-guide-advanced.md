# User Guide - Advanced Usage

## Advanced Usage

### Running Multiple Bot Instances

**⚠️ IMPORTANT - Socket Mode v2 Requirement**: When running multiple bot instances (e.g., using `bot_network` example), each bot **MUST** receive its own unique WebSocket URL via a separate call to `apps.connections.open`. Sharing the same URL between multiple connections will cause message delivery issues.

**Note**: Each bot instance normally requires its own tokens and should connect to different workspaces or use different app configurations. For quick local experiments, you can also run multiple logical bots inside a single process using the `two_bots_keychain` or `bot_network` examples.

**Setup for Different Workspaces**:
```bash
# Terminal 1 - Bot instance 1
export SLACK_BOT_TOKEN=xoxb-token-1
export SLACK_APP_TOKEN=xapp-token-1
./bin/neo-slack-bot

# Terminal 2 - Bot instance 2
export SLACK_BOT_TOKEN=xoxb-token-2
export SLACK_APP_TOKEN=xapp-token-2
./bin/neo-slack-bot
```

**Setup for Multiple Bots in Same Workspace** (using `bot_network` example):
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C12345678
./bin/bot_network 5  # 5 bots, each gets unique URL automatically
```

**Quick Two-Bot Demo** (using `two_bots_keychain` example):
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
export SLACK_TEST_CHANNEL=C1234567890  # Temporary test channel ID
./bin/two_bots_keychain
```

In this demo:
- Bot 1 sends a few messages like `Hello from Bot 1 (msg N)` to the test channel via HTTP API.
- Bot 2 listens on WebSocket and prints any messages containing `Hello from Bot 1`.
- The channel ID is cached in macOS Keychain for future runs.

**Key Points**:
- Each bot automatically gets its own unique WebSocket URL
- Rate limiting delay (100ms) is applied between URL retrieval calls
- Each bot can receive messages independently
- See [examples.md](examples.md) for more details on `bot_network` example

### Monitoring Bot Activity

**View Real-time Logs**:
```bash
tail -f ~/Library/Logs/neo-slack-bot.log
```

**Filter Logs**:
```bash
# Only errors
tail -f ~/Library/Logs/neo-slack-bot.log | grep ERROR

# Only messages
tail -f ~/Library/Logs/neo-slack-bot.log | grep "Message received"

# Only commands
tail -f ~/Library/Logs/neo-slack-bot.log | grep "Command"
```

### Performance Monitoring

**Check Metrics**:
```bash
# Bot shows metrics summary on shutdown (Ctrl+C)
# Or check logs for periodic metrics updates
```

**Monitor Resource Usage**:
```bash
# CPU and memory usage
top -pid $(pgrep neo-slack-bot)

# Or use Activity Monitor GUI
open -a "Activity Monitor"
```

## Best Practices

1. **Test in Development Workspace First**: Use a test Slack workspace before production
2. **Monitor Logs**: Regularly check logs for errors or issues
3. **Use Channel Filtering**: Reduce notification noise with filters
4. **Set Up Auto-Start**: Use launchd for production deployment
5. **Keep Tokens Secure**: Never commit tokens to version control
6. **Regular Updates**: Keep bot updated with latest version
7. **Backup Configuration**: Save your config file

## Getting Help

- **Documentation**: See `docs/` directory
- **Quick Start**: See `docs/quick-start.md`
- **Installation**: See `docs/installation.md`
- **Configuration**: See `docs/user-settings-basic.md` and `docs/user-settings-advanced.md`
- **API Documentation**: See `docs/slack-api.md`
- **Troubleshooting**: See troubleshooting section above

---

**Happy Botting! 🤖**

## See Also

- [User Guide - Quick Start](user-guide-quick-start.md) - Quick start and basic usage
- [User Guide - Testing](user-guide-testing.md) - Testing message sending/receiving
- [User Guide - Configuration](user-guide-configuration.md) - Configuration and troubleshooting
