# Manual Testing Guide - Advanced Test Scenarios

- ✅ Notification for "important" message
- ✅ Notification for "critical" message

**Console Output**:
```
[INFO] Message filtered by keyword: urgent
[INFO] Notification displayed
[INFO] Message filtered out (no keywords)
[INFO] Message filtered by keyword: important
[INFO] Notification displayed
[INFO] Message filtered by keyword: critical
[INFO] Notification displayed
```

### Test 8: macOS Notification Behavior

**Purpose**: Verify macOS notifications work correctly

**Test Steps**:
1. Send message in Slack
2. Observe notification appearance
3. Click notification
4. Check Notification Center

**Expected Results**:
- ✅ Notification appears in top-right corner
- ✅ Notification shows channel name or "Slack Message"
- ✅ Notification shows message text (truncated if long)
- ✅ Clicking notification opens Slack app
- ✅ Notification appears in Notification Center
- ✅ Notification respects Do Not Disturb

**Test Do Not Disturb**:
1. Enable macOS Do Not Disturb
2. Send message in Slack
3. Notification should be suppressed

### Test 9: Multiple Messages Rapidly

**Purpose**: Verify bot handles rapid message bursts

**Test Steps**:
1. Send 10 messages rapidly (within 5 seconds)
2. Observe console output
3. Count notifications

**Expected Results**:
- ✅ All 10 messages are received
- ✅ All 10 notifications appear
- ✅ No message loss
- ✅ Console shows all 10 events
- ✅ No performance degradation

### Test 10: Long-Running Bot

**Purpose**: Verify bot stability over time

**Test Steps**:
1. Start bot
2. Let it run for 30+ minutes
3. Send periodic test messages
4. Monitor memory usage
5. Check for reconnections

**Expected Results**:
- ✅ Bot stays connected
- ✅ No memory leaks (check metrics)
- ✅ All messages processed correctly
- ✅ No unexpected reconnections
- ✅ Metrics remain accurate

**Check Metrics**:
```bash
# In Slack, type: /metrics
# Bot responds with metrics summary
```

### Test 11: Multiple Bot Instances (Simulation)

**Purpose**: Verify multiple bots can process messages concurrently

**Note**: This requires multiple Slack apps or workspaces

**Setup**:
1. Create 2-3 Slack apps (or use different workspaces)
2. Set up tokens for each bot instance

**Test Steps**:
1. Start Bot Instance 1:
   ```bash
   export SLACK_BOT_TOKEN=xoxb-token-1
   export SLACK_APP_TOKEN=xapp-token-1
   ./bin/neo-slack-bot
   ```

2. Start Bot Instance 2 (in another terminal):
   ```bash
   export SLACK_BOT_TOKEN=xoxb-token-2
   export SLACK_APP_TOKEN=xapp-token-2
   ./bin/neo-slack-bot
   ```

3. Send messages to both workspaces

**Expected Results**:
- ✅ Each bot instance processes its own messages
- ✅ No interference between instances
- ✅ Each bot shows correct notifications
- ✅ Console logs are separate

### Test 12: Message Sending (Programmatic)

**Purpose**: Verify bot can send messages via API

**Test via Command**:
1. Type `/status` in Slack
2. Bot responds with status message
3. Verify message appears in Slack

**Test via API** (using curl):
```bash
curl -X POST https://slack.com/api/chat.postMessage \
  -H "Authorization: Bearer $SLACK_BOT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"channel":"C12345678","text":"Test message from API"}'
```

**Expected Results**:
- ✅ Message appears in Slack channel
- ✅ Message is sent by bot user
- ✅ Console shows API call logs

### Test 13: Special Characters in Messages

**Purpose**: Verify bot handles special characters correctly

**Test Messages**:
1. Send: `Hello "world" with quotes`
2. Send: `Message with\nnewlines`
3. Send: `Special chars: !@#$%^&*()`
4. Send: `Unicode: тест 测试 🚀`

**Expected Results**:
- ✅ All messages received correctly
- ✅ Special characters preserved
- ✅ Notifications show correct text
- ✅ No parsing errors

### Test 14: Empty and Edge Case Messages

**Purpose**: Verify bot handles edge cases

**Test Cases**:
1. Very long message (near 4000 char limit)
2. Message with only whitespace
3. Message with emoji only
4. Message with URLs

**Expected Results**:
- ✅ Long messages handled correctly
- ✅ Edge cases don't crash bot
- ✅ Appropriate validation errors logged


## See Also

- [Manual Testing Guide - Setup](manual-testing-guide-setup.md) - Prerequisites and setup
- [Manual Testing Guide - Advanced](manual-testing-guide-advanced.md) - CLI commands, troubleshooting, advanced testing

## See Also

- [Manual Testing Guide - Basic Test Scenarios](manual-testing-guide-scenarios-basic.md) - Basic message and command tests
- [Manual Testing Guide - Setup](manual-testing-guide-setup.md) - Prerequisites and setup
