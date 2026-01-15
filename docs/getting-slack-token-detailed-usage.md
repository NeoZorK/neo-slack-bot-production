# Getting Slack Token - Detailed Usage Guide

**Token format (example):** `xoxb-<workspace-id>-<random-id>-<secret-part>`

**Length:** Usually about 55-60 characters

### Step 5: Verify Settings

#### 5.1. Verify Socket Mode

1. Return to the **"Socket Mode"** section (left sidebar)
2. Make sure the **"Enable Socket Mode"** toggle is ON

#### 5.2. Verify Scopes

1. Return to the **"OAuth & Permissions"** section
2. Make sure all required scopes are added in **"Bot Token Scopes"**
3. Make sure the app is installed in workspace (Bot User OAuth Token exists)

#### 5.3. Verify Token

1. Make sure the token starts with `xoxb-`
2. Make sure the token is copied completely (no spaces at the beginning/end)
3. Save the token in a secure place

## What You Need and What You Don't for Neo Slack Bot

### ✅ REQUIRED (Mandatory)

1. **Socket Mode** — must be enabled
2. **Bot Token (Bot User OAuth Token)** — required for API calls and authentication
3. **App-Level Token** — **REQUIRED** for Socket Mode v2 (must have `connections:write` scope)
4. **Bot Token Scopes** — add all 7 required scopes
5. **Install to Workspace** — install app in workspace

**⚠️ IMPORTANT:** Both `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN` must be set as environment variables. Without `SLACK_APP_TOKEN`, the application will fail to connect.

### ❌ NOT NEEDED (Can be ignored)

1. **Event Subscriptions** — not needed, we use Socket Mode
2. **Slash Commands** — optional, can be added later
3. **Interactive Components** — optional
4. **OAuth Redirect URL** — not needed
5. **Webhook URL** — not needed

### ⚠️ OPTIONAL (Can be added later)

1. **Slash Commands** — if you plan to use commands like `/status`
2. **Reactions scopes** — if you want to work with reactions

## Token Verification and Usage

### Token Format Verification

The token must:
- Start with `xoxb-` (Bot Token) or `xapp-` (App-Level Token)
- Have length from 20 to 200 characters
- Contain only letters, numbers, hyphens, underscores, and dots

**Example of correct Bot Token (placeholder):**
```
xoxb-BOT_TOKEN_PLACEHOLDER
```

**Example of correct App-Level Token (placeholder):**
```
xapp-APP_TOKEN_PLACEHOLDER
```

### Using Tokens in Neo Slack Bot

Neo Slack Bot requires **two tokens**:
1. **SLACK_BOT_TOKEN** (`xoxb-...`) - Bot User OAuth Token
2. **SLACK_APP_TOKEN** (`xapp-...`) - App-Level Token with `connections:write` scope

**⚠️ IMPORTANT:** Both tokens are required. Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error.

#### Option 1: Temporary Setup (for testing)

```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
cd /path/to/neo-slack-bot/src
./bin/neo-slack-bot
```

#### Option 2: Permanent Setup (for regular use)

**For zsh (macOS default):**
```bash
# Add both tokens to ~/.zshrc
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.zshrc
source ~/.zshrc
```

**For bash:**
```bash
# Add both tokens to ~/.bash_profile
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.bash_profile
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.bash_profile
source ~/.bash_profile
```

**Verify tokens are set:**
```bash
echo $SLACK_BOT_TOKEN
echo $SLACK_APP_TOKEN
```

Both commands should output your tokens (starting with `xoxb-` and `xapp-` respectively).

#### Option 3: Using in Script

Create a file `run-bot.sh`:
```bash
#!/bin/bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
cd /path/to/neo-slack-bot/src
./bin/neo-slack-bot
```

Make it executable:
```bash
chmod +x run-bot.sh
./run-bot.sh
```

### Verifying Bot Operation

1. Start the bot (see above)
2. Open Slack workspace
3. Send a message in any channel
4. Check bot console — logs of event processing should appear
5. Check macOS notifications — a notification about the message should appear

### Testing Commands

If you configured slash commands:

1. In Slack, type `/status` in any channel
2. Bot should respond with a status message

## Token Security

### ⚠️ IMPORTANT RULES

1. **Never commit token to git**
   - Check `.gitignore` — it should have `.env` or similar
   - Don't add token to code
   - Don't add token to public documentation

2. **Never share token publicly**
   - Don't send token in chats, email, messages
   - Don't publish token in issues, pull requests

3. **Store token securely**
   - Use password manager
   - Use environment variables
   - Don't store token in plain text files

4. **Rotate token regularly**
   - If token is compromised, revoke it in app settings
   - Create new token via "Reinstall App"

5. **Limit access permissions**
   - Add only necessary scopes
   - Don't add unnecessary permissions

### Revoking Token

If token is compromised:

1. Go to **"OAuth & Permissions"**
2. Find **"OAuth Tokens for Your Workspace"** section
3. Click **"Revoke Token"** next to the token
4. Reinstall app to get new token

## Troubleshooting

### Problem: "Invalid token format"

**Causes:**
- Token doesn't start with `xoxb-` or `xapp-`
- Token was truncated when copying
- Extra spaces present

**Solution:**
1. Check token format
2. Copy token again from app settings
3. Make sure there are no spaces at the beginning/end

### Problem: "Authentication failed"

**Causes:**
- Socket Mode not enabled
- Token is invalid
- App not installed in workspace

**Solution:**
1. Check that Socket Mode is enabled
2. Check that app is installed in workspace
3. Reinstall app if needed

### Problem: "Insufficient scopes"

**Causes:**
- Not all required scopes added
- App not reinstalled after adding scopes

**Solution:**
1. Add missing scopes
2. Reinstall app in workspace
3. Copy new token

### Problem: "403 Forbidden"

**Causes:**
- `SLACK_APP_TOKEN` not set
- App-Level Token missing `connections:write` scope
- Token is invalid

**Solution:**
1. Make sure `SLACK_APP_TOKEN` environment variable is set
2. Verify App-Level Token has `connections:write` scope
3. Regenerate App-Level Token if needed

### Problem: Bot doesn't receive messages

**Causes:**
- Socket Mode not enabled
- Wrong token
- Bot not added to channel

**Solution:**
1. Check Socket Mode
2. Check token
3. Add bot to channel: `/invite @Neo Slack Bot`

### Problem: Bot doesn't send messages

**Causes:**
- Missing `chat:write` scope
- Bot not installed in workspace
- Wrong channel ID

**Solution:**
1. Add `chat:write` scope
2. Reinstall app
3. Check channel ID format (should start with `C`, `D`, or `G`)

## Additional Resources

### Official Slack Documentation

- [Slack API Documentation](https://api.slack.com/)
- [Socket Mode Guide](https://api.slack.com/apis/connections/socket)
- [OAuth Scopes Reference](https://api.slack.com/scopes)
- [Bot Token Guide](https://api.slack.com/authentication/token-types#bot)

### Neo Slack Bot Documentation

- [Authentication Guide](auth-basic.md) — detailed authentication guide
- [Quick Start Guide](quick-start.md) — quick start
- [Security Audit](security-audit.md) — security information

### Useful Links

- [Slack App Management](https://api.slack.com/apps) — app management
- [Slack API Methods](https://api.slack.com/methods) — API methods list
- [Slack Events API](https://api.slack.com/events-api) — Events API

## See Also

- [Quick Guide](getting-slack-token-quick.md) - Quick reference for getting started

## See Also

- [Getting Slack Token - Detailed Setup](getting-slack-token-detailed-setup.md) - App creation and token generation
- [Getting Slack Token - Quick Guide](getting-slack-token-quick.md) - Quick reference
