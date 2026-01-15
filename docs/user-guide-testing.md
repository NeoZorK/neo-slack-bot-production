# User Guide - Testing

## Testing Message Sending/Receiving

### Test 1: Receive Messages

**Steps**:
1. Start the bot (see [Basic Usage](#basic-usage))
2. In Slack, send a message to any channel where the bot is present
3. Check console output - you should see event processing logs
4. Check macOS notifications - notification should appear

**Expected Console Output**:
```
[INFO] Event received: events_api
[INFO] Message received in channel: C12345678
[INFO] Message text: Hello, bot!
[INFO] Notification displayed
```

**Expected Notification**:
- Title: "Slack Message" (or channel name)
- Body: Message text
- Sound: Default notification sound (if enabled)

### Test 2: Send Messages (via Commands)

The bot can send messages in response to commands:

**Test `/status` command**:
1. In Slack, type: `/status`
2. Bot should respond with status information
3. Check console for command processing logs

**Test `/help` command**:
1. In Slack, type: `/help`
2. Bot should respond with available commands
3. Check console for command processing logs

**Expected Console Output**:
```
[INFO] Command received: /status
[INFO] Processing command: status
[INFO] Sending response to channel: C12345678
```

### Test 3: Send Messages Programmatically

You can test message sending using the test suite:

```bash
cd tests
export SLACK_TEST_CHANNEL=C12345678  # Your test channel ID
./bin/test_neo_slack_bot  # Run E2E tests
```

Or use the Slack API directly:
```bash
curl -X POST https://slack.com/api/chat.postMessage \
  -H "Authorization: Bearer $SLACK_BOT_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"channel":"C12345678","text":"Test message"}'
```

## Testing with Multiple Users

### Test Scenario: Multiple Users Sending Messages

**Setup**:
1. Create a test channel in Slack
2. Invite multiple users to the channel
3. Start the bot

**Test Steps**:
1. **User 1** sends: "Hello from User 1"
2. **User 2** sends: "Hello from User 2"
3. **User 3** sends: "Hello from User 3"

**Expected Results**:
- Bot receives all 3 messages
- 3 notifications appear (one per message)
- Console shows 3 event processing logs
- Each notification shows correct user and message

**Verify in Console**:
```
[INFO] Message received from user: U1234567890
[INFO] Message text: Hello from User 1
[INFO] Notification displayed
[INFO] Message received from user: U0987654321
[INFO] Message text: Hello from User 2
[INFO] Notification displayed
[INFO] Message received from user: U1122334455
[INFO] Message text: Hello from User 3
[INFO] Notification displayed
```

### Test Scenario: Bot Mentions

**Test Steps**:
1. **User 1** mentions bot: `@Neo Slack Bot hello`
2. Bot should receive `app_mention` event
3. Check console for mention processing

**Expected Console Output**:
```
[INFO] App mention received
[INFO] Mention text: hello
[INFO] Notification displayed
```

## Testing Channels and Notifications

### Test 1: Channel Filtering (Whitelist)

**Setup**:
1. Create configuration file: `~/.config/neo-slack-bot/config.json`
2. Add channel whitelist:
   ```json
   {
     "notifications": {
       "channel_filter": {
         "mode": "whitelist",
         "channels": ["C12345678"]
       }
     }
   }
   ```
3. Restart bot

**Test Steps**:
1. Send message to whitelisted channel (`C12345678`)
2. Send message to non-whitelisted channel (`C99999999`)

**Expected Results**:
- ✅ Notification appears for whitelisted channel
- ❌ No notification for non-whitelisted channel
- Console shows filtering logs

### Test 2: Channel Filtering (Blacklist)

**Setup**:
```json
{
  "notifications": {
    "channel_filter": {
      "mode": "blacklist",
      "channels": ["C12345678"]
    }
  }
}
```

**Test Steps**:
1. Send message to blacklisted channel (`C12345678`)
2. Send message to other channel (`C99999999`)

**Expected Results**:
- ❌ No notification for blacklisted channel
- ✅ Notification appears for other channel

### Test 3: Keyword Filtering

**Setup**:
```json
{
  "events": {
    "keyword_filter": {
      "enabled": true,
      "mode": "include",
      "keywords": ["urgent", "important", "critical"]
    }
  }
}
```

**Test Steps**:
1. Send message: "This is an urgent message"
2. Send message: "This is a normal message"
3. Send message: "Important update here"

**Expected Results**:
- ✅ Notification for "urgent" message
- ❌ No notification for normal message
- ✅ Notification for "important" message

**Verify in Console**:
```
[INFO] Message filtered by keyword: urgent
[INFO] Notification displayed
[INFO] Message filtered out (no keywords)
[INFO] Message filtered by keyword: important
[INFO] Notification displayed
```

### Test 4: macOS Notification Behavior

**Test Notification Display**:
1. Send message in Slack
2. Notification should appear in top-right corner (macOS)
3. Notification should show:
   - Channel name or "Slack Message"
   - Message text (truncated if long)
   - Timestamp

**Test Notification Actions**:
1. Click notification → Should open Slack app
2. Notification should disappear after timeout
3. Notification should appear in Notification Center

**Test Do Not Disturb**:
1. Enable macOS Do Not Disturb
2. Send message in Slack
3. Notification should be suppressed (respects macOS DND)


## See Also

- [User Guide - Quick Start](user-guide-quick-start.md) - Quick start and basic usage
- [User Guide - Configuration](user-guide-configuration.md) - Configuration and troubleshooting
- [User Guide - Advanced Usage](user-guide-advanced.md) - Advanced usage and best practices
