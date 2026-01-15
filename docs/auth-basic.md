# Authentication - Basic Guide

## Overview

Neo Slack Bot uses Slack's Bot User OAuth Token and App-Level Token for authentication. The tokens provide access to Slack's Socket Mode and HTTP APIs with the permissions (scopes) granted during app installation.

## Token Types

### Bot User OAuth Token

**Format**: `xoxb-...` (Bot User OAuth Token)

**Usage**: Primary authentication token for bot operations

**Capabilities**:
- Connect to Socket Mode
- Make HTTP API requests
- Access granted scopes
- Act on behalf of the bot user

**Generation**:
1. Create Slack App
2. Configure OAuth scopes
3. Install app to workspace
4. Copy Bot User OAuth Token

**Security**:
- Never commit to repository
- Store in environment variables
- Rotate periodically

### App-Level Token (Socket Mode v2)

**Format**: `xapp-...` (App-Level Token)

**Usage**: **REQUIRED** for Socket Mode v2 connection

**Capabilities**:
- Enable Socket Mode v2
- Call `apps.connections.open` API to get dynamic WebSocket URL
- Establish WebSocket connection via dynamic URL

**Generation**:
1. Enable Socket Mode in app settings
2. Create App-Level Token
3. Add `connections:write` scope
4. Copy token

**⚠️ IMPORTANT**: Neo Slack Bot uses Socket Mode v2, which **requires** App-Level Token. Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error. Both `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN` must be set as environment variables.

## Token Management

### Environment Variables

**Recommended Method**: Store tokens in environment variables

**Temporary Setup** (for testing):
```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
```

**Permanent Setup** (for regular use):
```bash
# Add to ~/.zshrc or ~/.bash_profile
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.zshrc
source ~/.zshrc
```

**⚠️ IMPORTANT**: Both tokens are required. Without `SLACK_APP_TOKEN`, the application will not be able to connect to Slack.

**Security**:
- Never log token values
- Don't include in error messages
- Use secure environment
- Restrict file permissions

## Token Validation

### Format Validation

**Bot Token Format**:
- Prefix: `xoxb-`
- Length: ~55 characters
- Format: `xoxb-[numbers]-[alphanumeric]`

**App-Level Token Format**:
- Prefix: `xapp-`
- Longer format

**Validation Code**:
```cpp
bool is_valid_bot_token(const std::string& token) {
    return token.starts_with("xoxb-") && token.length() > 20;
}
```

### Connection Validation

**Test Connection**:
- Attempt Socket Mode connection
- Verify authentication response
- Check token validity
- Handle authentication errors

## Socket Mode Setup

### Step 1: Create Slack App

1. Go to https://api.slack.com/apps
2. Click "Create New App"
3. Choose "From scratch"
4. Enter app name and workspace

### Step 2: Enable Socket Mode

1. Navigate to "Socket Mode" in app settings
2. Toggle "Enable Socket Mode" to ON
3. Create App-Level Token with `connections:write` scope

### Step 3: Configure Scopes

**Required Scopes**:
- `app_mentions:read` - Receive mentions
- `channels:read` - Read channel info
- `channels:history` - Read messages
- `chat:write` - Send messages
- `im:read` - Read DMs
- `im:write` - Send DMs
- `users:read` - Read user info

**Optional Scopes**:
- `reactions:read` - Read reactions
- `reactions:write` - Add reactions
- `files:read` - Read files
- `files:write` - Upload files

### Step 4: Install App

1. Navigate to "OAuth & Permissions"
2. Click "Install to Workspace"
3. Authorize requested scopes
4. Copy Bot User OAuth Token

### Step 5: Configure Bot

```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
./bin/neo-slack-bot
```

## Authentication Flow

### Socket Mode v2 Connection

1. **Initialize Client**
   - Load tokens from environment (`SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN`)
   - Validate token formats
   - Prepare HTTP client for `apps.connections.open` call

2. **Obtain Dynamic WebSocket URL**
   - Call `apps.connections.open` API using App-Level Token
   - Parse response to extract `wss://` URL
   - Decode escaped slashes in URL

3. **Establish WebSocket Connection**
   - Parse dynamic URL (host, path, port)
   - Establish TCP connection
   - Establish TLS connection
   - Perform WebSocket handshake
   - Wait for `hello` event

4. **Verify Authentication**
   - Check for `hello` event
   - Verify connection established
   - Handle authentication errors

5. **Maintain Connection**
   - Process events
   - Handle disconnections
   - Reconnect on failure (with exponential backoff)

### HTTP API Requests

1. **Prepare Request**
   - Set Authorization header: `Bearer xoxb-token`
   - Set Content-Type: `application/json`
   - Construct request body

2. **Send Request**
   - Make HTTP POST/GET request
   - Include authentication header
   - Handle rate limits

3. **Process Response**
   - Parse JSON response
   - Check for errors
   - Handle authentication failures

## Troubleshooting

### Common Issues

**Problem**: "SLACK_BOT_TOKEN not set"
- **Solution**: Set environment variable
- **Check**: `echo $SLACK_BOT_TOKEN`

**Problem**: "Invalid token format"
- **Solution**: Verify token starts with `xoxb-` or `xapp-`
- **Check**: Token copied correctly

**Problem**: "Authentication failed"
- **Solution**: Verify token is valid
- **Check**: Token not expired or revoked
- **Check**: Socket Mode enabled

**Problem**: "Insufficient scopes"
- **Solution**: Add required scopes
- **Check**: Reinstall app with new scopes

**Problem**: "403 Forbidden"
- **Solution**: Make sure `SLACK_APP_TOKEN` is set
- **Check**: App-Level Token has `connections:write` scope

### Debugging

**Enable Debug Logging**:
```bash
export DEBUG=1
./bin/neo-slack-bot
```

**Check Token**:
```bash
# Verify token format (don't expose full token)
echo $SLACK_BOT_TOKEN | cut -c1-10
# Should output: xoxb-1234
```

**Test Connection**:
- Check Socket Mode enabled
- Verify scopes configured
- Test with curl (HTTP API)
- Review Slack app logs

## See Also

- [Authentication - Advanced Guide](auth-advanced.md) - Security best practices and advanced topics
