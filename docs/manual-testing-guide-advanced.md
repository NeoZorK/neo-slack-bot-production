# Manual Testing Guide - Advanced Testing

## CLI Testing Commands

### Test Message Sending

```bash
# Set test channel
export SLACK_TEST_CHANNEL=C12345678

# Run E2E tests
cd tests
./bin/test_neo_slack_bot

# Test specific functionality
curl -X POST https://slack.com/api/chat.postMessage \
  -H "Authorization: Bearer $SLACK_BOT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"channel":"'$SLACK_TEST_CHANNEL'","text":"Test message"}'
```

### Test Message Receiving

```bash
# Start bot in one terminal
./bin/neo-slack-bot

# Send message from Slack (or use API)
# Check bot console for received message logs
```

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

## Verification Checklist

### Pre-Testing Checklist

- [ ] Bot token is set (`echo $SLACK_BOT_TOKEN`)
- [ ] App token is set (`echo $SLACK_APP_TOKEN`)
- [ ] Bot executable exists (`ls -la src/bin/neo-slack-bot`)
- [ ] Socket Mode is enabled in Slack app settings
- [ ] Bot is installed in workspace
- [ ] Bot is invited to test channels
- [ ] macOS notification permissions granted

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
- [ ] `/metrics` command → Bot responds
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

## Advanced Testing

### Testing with Multiple Workspaces

If you have access to multiple Slack workspaces:

1. Create separate Slack apps for each workspace
2. Set up tokens for each workspace
3. Run multiple bot instances
4. Test cross-workspace functionality

### Performance Testing

Monitor bot performance during testing:

```bash
# Monitor CPU and memory
top -pid $(pgrep neo-slack-bot)

# Check metrics
# In Slack: /metrics
```

### Load Testing

Test bot under load:

1. Send 100+ messages rapidly
2. Monitor console for errors
3. Check notification delivery
4. Verify no message loss

## Test Results Template

Use this template to document test results:

```
Test Date: ___________
Tester: ___________
Bot Version: ___________

Test Results:
- [ ] Basic message receiving: PASS / FAIL
- [ ] Bot commands: PASS / FAIL
- [ ] Multiple users: PASS / FAIL
- [ ] Bot mentions: PASS / FAIL
- [ ] Channel filtering: PASS / FAIL
- [ ] Keyword filtering: PASS / FAIL
- [ ] macOS notifications: PASS / FAIL
- [ ] Message sending: PASS / FAIL
- [ ] Special characters: PASS / FAIL
- [ ] Long-running: PASS / FAIL

Issues Found:
1. ___________
2. ___________

Notes:
___________
```

---

For more information, see:
- **User Guide**: `docs/user-guide.md`
- **Quick Start**: `docs/quick-start.md`
- **Configuration**: `docs/user-settings-basic.md`

## See Also

- [Manual Testing Guide - Setup](manual-testing-guide-setup.md) - Prerequisites and setup
- [Manual Testing Guide - Test Scenarios](manual-testing-guide-scenarios.md) - Detailed test scenarios
