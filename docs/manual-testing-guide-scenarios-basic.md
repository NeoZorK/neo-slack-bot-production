# Manual Testing Guide - Basic Test Scenarios

# Manual Testing Guide - Test Scenarios

## Test Scenarios

### Test 1: Basic Message Receiving

**Purpose**: Verify bot receives messages and displays notifications

**Steps**:
1. Start bot (see above)
2. In Slack, send a message to any channel where bot is present
3. Observe console output
4. Check macOS notification

**Expected Results**:
- ✅ Console shows: `[INFO] Message received in channel: C12345678`
- ✅ Console shows: `[INFO] Notification sent: <message text>`
- ✅ macOS notification appears with message text
- ✅ Notification shows correct channel/user info

**Console Output Example**:
```
[INFO] Event received: events_api
[INFO] Message received in channel: C12345678
[INFO] Message text: Hello, bot!
[INFO] Notification displayed
```

### Test 2: Bot Commands

**Purpose**: Verify bot responds to slash commands

**Test `/status` Command**:
1. In Slack, type: `/status`
2. Bot should respond: "Bot is running and connected to Slack Socket Mode"
3. Check console for command processing logs

**Test `/help` Command**:
1. In Slack, type: `/help`
2. Bot should respond: "Available commands: /status, /help, /metrics"
3. Check console for command processing logs

**Test `/metrics` Command**:
1. In Slack, type: `/metrics`
2. Bot should respond with metrics summary (events, errors, latencies, memory)
3. Check console for command processing logs

**Expected Console Output**:
```
[INFO] Command received: /status
[INFO] Processing command: status
[INFO] Sending response to channel: C12345678
[INFO] Command processed: /status
```

### Test 3: Multiple Users Sending Messages

**Purpose**: Verify bot handles messages from multiple users correctly

**Setup**:
1. Create a test channel: `#test-bot`
2. Invite bot to channel: `/invite @Neo Slack Bot`
3. Invite 2-3 other users to channel

**Test Steps**:
1. **User 1** sends: "Hello from User 1"
2. **User 2** sends: "Hello from User 2"
3. **User 3** sends: "Hello from User 3"

**Expected Results**:
- ✅ Bot receives all 3 messages
- ✅ 3 notifications appear (one per message)
- ✅ Each notification shows correct user and message
- ✅ Console shows 3 separate event processing logs
- ✅ No message loss or duplication

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

### Test 4: Bot Mentions

**Purpose**: Verify bot handles app mentions correctly

**Test Steps**:
1. In Slack, mention bot: `@Neo Slack Bot hello`
2. Check console for mention processing
3. Check notification (should show "Slack Mention" title)

**Expected Results**:
- ✅ Bot receives `app_mention` event
- ✅ Console shows: `[INFO] App mention received`
- ✅ Notification title is "Slack Mention" (not "Slack Message")
- ✅ Notification contains mention text

**Console Output**:
```
[INFO] Event received: events_api
[INFO] App mention received
[INFO] Mention text: hello
[INFO] Notification displayed (title: Slack Mention)
```

### Test 5: Channel Filtering (Whitelist)

**Purpose**: Verify channel whitelist filtering works

**Setup**:
1. Create config file: `~/.config/neo-slack-bot/config.json`
2. Add whitelist configuration:
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
- ✅ Console shows filtering logs

**Console Output**:
```
[INFO] Message received in channel: C12345678
[INFO] Channel is whitelisted, processing message
[INFO] Notification displayed

[INFO] Message received in channel: C99999999
[INFO] Channel not in whitelist, filtering out message
```

### Test 6: Channel Filtering (Blacklist)

**Purpose**: Verify channel blacklist filtering works

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

### Test 7: Keyword Filtering

**Purpose**: Verify keyword filtering works correctly

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
4. Send message: "Critical issue detected"

**Expected Results**:
- ✅ Notification for "urgent" message
- ❌ No notification for normal message

## See Also

- [Manual Testing Guide - Advanced Test Scenarios](manual-testing-guide-scenarios-advanced.md) - Filtering and advanced scenarios
- [Manual Testing Guide - Setup](manual-testing-guide-setup.md) - Prerequisites and setup
