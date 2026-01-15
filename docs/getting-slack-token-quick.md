# Quick Guide: Getting Slack Tokens

## Overview

Neo Slack Bot requires **two tokens**:
1. **SLACK_BOT_TOKEN** (`xoxb-...`) - Bot User OAuth Token
2. **SLACK_APP_TOKEN** (`xapp-...`) - App-Level Token with `connections:write` scope

**⚠️ IMPORTANT:** Both tokens are required. Without `SLACK_APP_TOKEN`, the application will fail to connect.

## Quick Steps

### 1. Create Slack App

1. Go to **https://api.slack.com/apps**
2. Click **"Create New App"** → **"From scratch"**
3. Enter app name: `Neo Slack Bot`
4. Select your workspace
5. Click **"Create App"**

### 2. Enable Socket Mode

1. In left sidebar, click **"Socket Mode"**
2. Toggle **"Enable Socket Mode"** to ON
3. Scroll to **"App-Level Tokens"**
4. Click **"Generate Token"**
5. Enter name: `socket-mode-token`
6. Add scope: `connections:write`
7. Click **"Generate"**
8. **Copy the token** (`xapp-...`) - shown only once!

### 3. Configure Scopes

1. In left sidebar, click **"OAuth & Permissions"**
2. Scroll to **"Bot Token Scopes"**
3. Click **"Add an OAuth Scope"** and add:
   - `app_mentions:read`
   - `channels:read`
   - `channels:history`
   - `chat:write`
   - `im:read`
   - `im:write`
   - `users:read`

### 4. Install to Workspace

1. Scroll up to **"OAuth Tokens for Your Workspace"**
2. Click **"Install to Workspace"**
3. Review permissions and click **"Allow"**
4. Copy **"Bot User OAuth Token"** (`xoxb-...`)

### 5. Set Environment Variables

**Temporary (for testing):**
```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
```

**Permanent (for regular use):**
```bash
# For zsh (macOS default)
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.zshrc
source ~/.zshrc

# For bash
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.bash_profile
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.bash_profile
source ~/.bash_profile
```

**Verify:**
```bash
echo $SLACK_BOT_TOKEN
echo $SLACK_APP_TOKEN
```

### 6. Start Bot

```bash
cd /path/to/neo-slack-bot/src
./bin/neo-slack-bot
```

## What You Need

### ✅ REQUIRED

- Socket Mode enabled
- Bot Token (`xoxb-...`)
- App-Level Token (`xapp-...`) with `connections:write` scope
- 7 required scopes added
- App installed to workspace

### ❌ NOT NEEDED

- Event Subscriptions
- Webhook URL
- OAuth Redirect URL

## Token Format

**Bot Token (example):** `xoxb-<workspace-id>-<random-id>-<secret-part>`
- Must start with `xoxb-`
- Length between 20 and 200 characters

**App-Level Token (example):** `xapp-1-<workspace-id>-<random-id>-<secret-part>`
- Must start with `xapp-`
- Longer format

## Security

- Never commit tokens to git
- Never share tokens publicly
- Store tokens securely (password manager)
- Use environment variables
- Rotate tokens if compromised

## Troubleshooting

**"Invalid token format"**
- Check token starts with `xoxb-` or `xapp-`
- Remove extra spaces
- Copy token again

**"Authentication failed"**
- Verify Socket Mode is enabled
- Verify app is installed to workspace
- Reinstall app if needed

**"403 Forbidden"**
- Make sure `SLACK_APP_TOKEN` is set
- Verify App-Level Token has `connections:write` scope

**Bot doesn't receive messages**
- Check Socket Mode enabled
- Add bot to channel: `/invite @Neo Slack Bot`

## See Also

- [Detailed Guide](getting-slack-token-detailed.md) - Complete step-by-step instructions with troubleshooting
