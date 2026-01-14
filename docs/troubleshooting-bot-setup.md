# Troubleshooting Bot Setup

## Problem 1: "Sending messages to this app has been turned off"

### Solution

1. Go to [Slack API Apps](https://api.slack.com/apps)
2. Select your bot application
3. In the left sidebar, go to **"Interactivity & Shortcuts"**
4. Enable **"Interactivity"** toggle (if disabled)
5. Click **"Save Changes"**

### Additional Checks

- **Event Subscriptions**: If using Event Subscriptions (not Socket Mode), ensure it's enabled and Request URL is configured
- **Socket Mode**: If using Socket Mode (recommended), ensure it's enabled in **"Socket Mode"** section
- **Bot Token Scopes**: Verify your Bot Token has required scopes:
  - `chat:write` - to send messages
  - `channels:join` - to join public channels
  - `groups:write` - to join private channels/groups
  - `channels:read` - to read channel information

## Problem 2: Adding Bot to a Test Channel

### Method 1: Using Utility Script (Recommended)

```bash
cd examples
export SLACK_BOT_TOKEN=xoxb-your-token
make add_bot_to_channel
./bin/add_bot_to_channel C1234567890
```

### Method 2: Manual Invitation via Slack UI

1. Open your test channel in Slack
2. Click on channel name at the top
3. Go to **"Integrations"** tab
4. Click **"Add apps"**
5. Search for your bot and add it

### Method 3: Using Slack Command (Recommended for Quick Setup)

**Step-by-step instructions:**

1. Open Slack in your web browser (web interface)
2. Navigate to your test channel
3. Click on the chat input field at the bottom
4. Type exactly: `/invite @neo_slack_bot` (without quotes, brackets, or any other characters)
5. Press Enter

**Important**: 
- Use web interface (not desktop app) for best compatibility
- Type the command exactly as shown: `/invite @neo_slack_bot`
- Do not add quotes, brackets, or any other characters
- Replace `@neo_slack_bot` with your actual bot's display name if different

### Method 4: Programmatic (via API)

The bot can automatically join channels using the `join_channel()` method:

```cpp
SlackAPI api(bot_token);
if (api.join_channel("C1234567890")) {
    // Successfully joined channel
}
```

### Required Scopes for Channel Access

- `channels:join` - Join public channels
- `groups:write` - Join private channels (if channel is private)
- `channels:read` - Read channel information

### Troubleshooting Channel Access

If automatic join fails:

1. **Check channel type**: Private channels require manual invitation
2. **Verify scopes**: Ensure bot has `channels:join` or `groups:write` scope
3. **Check channel ID**: Verify channel ID is correct (format: `C1234567890`)
4. **Manual invitation**: Use Method 2 or 3 above

## Verifying Bot Setup

After fixing both issues, verify:

1. **Bot can send messages**:
   ```bash
   # Test sending a message
   cd examples
   export SLACK_BOT_TOKEN=xoxb-your-token
   export SLACK_TEST_CHANNEL=C1234567890
   ./bin/http_api_example
   ```

2. **Bot is in channel**:
   - Check channel member list in Slack UI
   - Or use `conversations.info` API to verify bot membership

3. **Bot receives events**:
   - Send a message in channel #nz
   - Check bot logs to verify event reception

## Common Issues

### "channel_not_found" Error

- **Cause**: Channel ID is incorrect or bot is not in channel
- **Solution**: Verify channel ID, manually invite bot if needed

### "missing_scope" Error

- **Cause**: Bot token doesn't have required OAuth scopes
- **Solution**: Reinstall app with required scopes:
  1. Go to [Slack API Apps](https://api.slack.com/apps)
  2. Select your app
  3. Go to **"OAuth & Permissions"**
  4. Add missing scopes
  5. Reinstall app to workspace

### "invalid_auth" Error

- **Cause**: Bot token is invalid or expired
- **Solution**: Generate new Bot Token from app settings

### "not_in_channel" Error

- **Cause**: Bot is not a member of the channel
- **Solution**: Use one of the invitation methods above

### Bots Not Receiving Messages (Multiple Bots)

**Problem**: When running multiple bots (e.g., using `bot_network` example), bots don't receive messages even though they're connected.

**Cause**: All bots are using the same WebSocket URL. Socket Mode v2 requires each connection to have its own unique URL.

**Solution**: Ensure each bot gets its own unique WebSocket URL:

```cpp
// CORRECT: Each bot gets unique URL
for (int i = 0; i < bot_count; ++i) {
    WebSocketClient client(bot_token);
    
    // Get unique URL for this bot
    SlackAPI api(app_token);
    std::string websocket_url;
    if (api.get_socket_mode_url(websocket_url)) {
        client.set_websocket_url(websocket_url);
    }
    
    // Rate limiting delay between API calls
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

**Verification**:
- Check logs for "got unique WebSocket URL" messages
- Each bot should have a different URL (different ticket parameter)
- Hello events should be received by each bot

**Note**: The `bot_network` example has been updated to use unique URLs per bot. If you're using an older version, update to the latest code.
