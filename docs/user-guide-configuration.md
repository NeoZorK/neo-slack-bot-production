# User Guide - Configuration and Troubleshooting

## Configuration

### Configuration File Location

**Default**: `~/.config/neo-slack-bot/config.json`

**macOS**: `~/Library/Application Support/neo-slack-bot/config.json`

### Basic Configuration

**Minimal Config**:
```json
{
  "notifications": {
    "enabled": true,
    "sound_enabled": true
  }
}
```

**Full Config Example**:
```json
{
  "notifications": {
    "enabled": true,
    "sound_enabled": true,
    "dnd_hours": {
      "enabled": false
    },
    "channel_filter": {
      "mode": "all"
    }
  },
  "connection": {
    "max_retries": 10,
    "timeout_seconds": 30
  },
  "events": {
    "keyword_filter": {
      "enabled": false
    }
  }
}
```

### Environment Variables

**Required**:
```bash
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token
```

**Optional**:
```bash
export SLACK_TEST_CHANNEL=C12345678  # For testing
export DEBUG=1                        # Enable debug logging
```

## Manual Testing Checklist

### Pre-Flight Checks

- [ ] Bot token is set (`echo $SLACK_BOT_TOKEN`)
- [ ] App token is set (`echo $SLACK_APP_TOKEN`)
- [ ] Bot executable exists (`ls -la src/bin/neo-slack-bot`)
- [ ] Socket Mode is enabled in Slack app settings
- [ ] Bot is installed in workspace

### Connection Tests

- [ ] Bot starts without errors
- [ ] Console shows "Connected to Slack Socket Mode"
- [ ] No reconnection loops
- [ ] Logs show successful connection

### Message Receiving Tests

- [ ] Send message → Bot receives it
- [ ] Send mention → Bot receives mention event
- [ ] Send in different channels → Bot receives all
- [ ] Send from different users → Bot receives all
- [ ] Check console logs for each message
- [ ] Check macOS notifications appear

### Message Sending Tests

- [ ] `/status` command → Bot responds
- [ ] `/help` command → Bot responds
- [ ] Bot can send messages programmatically
- [ ] Messages appear in Slack channels
- [ ] Special characters handled correctly

### Filtering Tests

- [ ] Channel whitelist works
- [ ] Channel blacklist works
- [ ] Keyword filtering (include) works
- [ ] Keyword filtering (exclude) works
- [ ] Multiple filters work together

### Notification Tests

- [ ] Notifications appear for filtered messages
- [ ] Notifications don't appear for filtered-out messages
- [ ] Notification content is correct
- [ ] Notification sound plays (if enabled)
- [ ] Do Not Disturb is respected
- [ ] Notifications appear in Notification Center

### Multi-User Tests

- [ ] Multiple users can send messages
- [ ] Bot processes all messages correctly
- [ ] Notifications show correct user info
- [ ] No message loss or duplication

### Stress Tests

- [ ] Send 10 messages rapidly → All processed
- [ ] Send 100 messages → All processed
- [ ] Multiple channels → All processed
- [ ] Long-running bot → No memory leaks
- [ ] Reconnection after network issue → Works

## CLI Testing Commands

### Test Message Sending

```bash
# Set test channel
export SLACK_TEST_CHANNEL=C12345678

# Run E2E tests
cd tests
./bin/test_neo_slack_bot

# Or test specific functionality
curl -X POST https://slack.com/api/chat.postMessage \
  -H "Authorization: Bearer $SLACK_BOT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"channel":"'$SLACK_TEST_CHANNEL'","text":"Test message"}'
```

### Test Message Receiving

1. Start bot in one terminal:
   ```bash
   ./bin/neo-slack-bot
   ```

2. Send message from Slack (or another terminal):
   ```bash
   # Use Slack web interface or API
   ```

3. Check bot console for received message logs

### Test Notifications

```bash
# Enable debug logging
export DEBUG=1
./bin/neo-slack-bot

# Send test message in Slack
# Check console for notification logs
# Verify macOS notification appears
```

### Test Filtering

```bash
# Create test config
mkdir -p ~/.config/neo-slack-bot
cat > ~/.config/neo-slack-bot/config.json << EOF
{
  "events": {
    "keyword_filter": {
      "enabled": true,
      "mode": "include",
      "keywords": ["test", "urgent"]
    }
  }
}
EOF

# Restart bot
./bin/neo-slack-bot

# Send messages with/without keywords
# Verify filtering works
```

## Troubleshooting

### Bot Not Receiving Messages

**Check**:
1. Bot is connected (check console for "Connected")
2. Bot is in the channel (invite bot to channel)
3. Event subscriptions are enabled in Slack app settings
4. Bot token has correct scopes (`channels:history`, `im:history`)

**Solution**:
```bash
# Check connection status
tail -f ~/Library/Logs/neo-slack-bot.log | grep -i "connected\|error"

# Verify bot is in channel
# In Slack: /invite @Neo Slack Bot to #channel-name
```

### Notifications Not Appearing

**Check**:
1. macOS notification permissions
2. Do Not Disturb status
3. Notification settings in config
4. Message filtering settings

**Solution**:
```bash
# Check notification permissions
# System Preferences → Notifications → Check app permissions

# Disable Do Not Disturb
# System Preferences → Focus → Do Not Disturb → Off

# Check config
cat ~/.config/neo-slack-bot/config.json | grep -i notification
```

### Messages Not Sending

**Check**:
1. Bot token has `chat:write` scope
2. Bot is in the target channel
3. Channel ID is correct format (`C12345678`)
4. Message text is not empty

**Solution**:
```bash
# Test API directly
curl -X POST https://slack.com/api/chat.postMessage \
  -H "Authorization: Bearer $SLACK_BOT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"channel":"C12345678","text":"Test"}'

# Check bot logs
tail -f ~/Library/Logs/neo-slack-bot.log | grep -i "send\|error"
```

### Connection Issues

**Check**:
1. Both tokens are set
2. Internet connection
3. Firewall settings
4. Slack API status

**Solution**:
```bash
# Verify tokens
echo $SLACK_BOT_TOKEN | grep -q "xoxb-" && echo "Bot token OK" || echo "Bot token missing"
echo $SLACK_APP_TOKEN | grep -q "xapp-" && echo "App token OK" || echo "App token missing"

# Test connectivity
curl -I https://slack.com/api/api.test

# Check logs
tail -f ~/Library/Logs/neo-slack-bot.log
```


## See Also

- [User Guide - Quick Start](user-guide-quick-start.md) - Quick start and basic usage
- [User Guide - Testing](user-guide-testing.md) - Testing message sending/receiving
- [User Guide - Advanced Usage](user-guide-advanced.md) - Advanced usage and best practices
