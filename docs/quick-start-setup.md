# Quick Start Guide - Setup

# Quick Start Guide

Get Neo Slack Bot up and running in minutes. This guide assumes you have completed the installation steps from `installation.md`.

## Prerequisites Checklist

Before starting, ensure you have:
- ✅ macOS 10.14+ installed
- ✅ Xcode Command Line Tools installed
- ✅ OpenSSL installed via Homebrew
- ✅ Project cloned and built
- ✅ Slack App created with Socket Mode enabled
- ✅ Bot token generated

## Step-by-Step Setup

### Step 1: Create Slack App

**📖 For detailed instructions, see [Complete Guide: How to Get Slack API Token](getting-slack-token-detailed.md)**

Quick version:

1. Go to https://api.slack.com/apps
2. Click **"Create New App"**
3. Choose **"From scratch"**
4. Enter app name: `Neo Slack Bot` (or your preferred name)
5. Select your workspace
6. Click **"Create App"**

### Step 2: Enable Socket Mode

1. In your app settings, navigate to **"Socket Mode"** (left sidebar)
2. Toggle **"Enable Socket Mode"** to ON
3. **⚠️ IMPORTANT**: Create an App-Level Token:
   - Scroll down to **"App-Level Tokens"** section
   - Click **"Generate Token"**
   - Enter token name: `socket-mode-token`
   - Add scope: `connections:write`
   - Click **"Generate"**
   - **Copy the token** (starts with `xapp-`) - it's shown only once!

**📖 More details:** See "Socket Mode" section in [detailed guide](getting-slack-token-detailed.md#socket-mode)

### Step 3: Configure Bot Token Scopes

1. Navigate to **"OAuth & Permissions"** (left sidebar)
2. Scroll to **"Scopes"** → **"Bot Token Scopes"**
3. Click **"Add an OAuth Scope"** and add the following scopes:
   - `app_mentions:read` - Receive mentions
   - `channels:read` - Read channel information
   - `channels:history` - Read channel messages
   - `chat:write` - Send messages
   - `im:read` - Read direct messages
   - `im:write` - Send direct messages
   - `users:read` - Read user information

**📖 More details:** See "Configure Bot Token Scopes" section in [detailed guide](getting-slack-token-detailed.md#step-3-configure-bot-token-scopes-permissions)

### Step 4: Install App to Workspace

1. Scroll up to **"OAuth Tokens for Your Workspace"**
2. Click **"Install to Workspace"**
3. Review permissions and click **"Allow"**
4. Copy the **Bot User OAuth Token** (starts with `xoxb-`) - **THIS IS YOUR TOKEN**

**📖 More details:** See "Install App to Workspace" section in [detailed guide](getting-slack-token-detailed.md#step-4-install-app-to-workspace)

### Step 5: Create App-Level Token

**⚠️ IMPORTANT:** Neo Slack Bot requires App-Level Token for Socket Mode v2 connection.

1. In your app settings, navigate to **"Socket Mode"** (left sidebar)
2. Scroll down to **"App-Level Tokens"** section
3. Click **"Generate Token"**
4. Enter token name: `socket-mode-token`
5. Add scope: `connections:write`
6. Click **"Generate"**
7. **IMPORTANT:** Copy the token (starts with `xapp-`) — it's shown only once!

**📖 More details:** See "Create App-Level Token" section in [detailed guide](getting-slack-token-detailed.md#23-create-app-level-token-required)

### Step 6: Set Environment Variables

Open your terminal and set **both tokens**:

**Temporary setup (for testing):**
```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
```

**Permanent setup (recommended):**

**For zsh (default on macOS):**
```bash
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.zshrc
source ~/.zshrc
```

**For bash:**
```bash
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.bash_profile
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.bash_profile
source ~/.bash_profile
```

**Verify tokens are set:**
```bash
echo $SLACK_BOT_TOKEN  # Should show: xoxb-...
echo $SLACK_APP_TOKEN   # Should show: xapp-1-...
```

**⚠️ IMPORTANT:** Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error and will continuously attempt to reconnect. Make sure both tokens are set before running the bot.

### Step 7: Build the Project

```bash
cd /path/to/neo-slack-bot/src
make
```

Wait for compilation to complete. You should see:
```
clang++ -std=c++17 -Wall -Wextra -O2 -c main.cpp -o obj/main.o
...
clang++ obj/*.o -o bin/neo-slack-bot -lssl -lcrypto -lpthread
```

### Step 8: Run the Bot

```bash
./bin/neo-slack-bot
```

**Expected Output**:
```
Starting Neo Slack Bot...
Connecting to Slack Socket Mode...
Connected to Slack Socket Mode
```

The bot is now running and connected!

### Step 9: Test the Connection

1. **Send a test message** in any Slack channel
2. **Check the console** - you should see event processing logs
3. **Check macOS notifications** - a notification should appear

### Step 10: Test Bot Mention

1. In Slack, mention your bot: `@Neo Slack Bot hello`
2. Verify the bot receives the mention event
3. Check console output for event details

## Verification Checklist

After completing all steps, verify:

- [ ] Bot connects successfully (no errors in console)
- [ ] Console shows "Connected to Slack Socket Mode"
- [ ] Sending a message triggers console output
- [ ] macOS notification appears for messages
- [ ] Bot responds to mentions (when implemented)

## Common First-Run Issues

### Issue: "SLACK_BOT_TOKEN not set" or "SLACK_APP_TOKEN not set"

**Solution**:
```bash
# Set both tokens
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER

# Or add to ~/.zshrc for permanent setup
echo 'export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER' >> ~/.zshrc
echo 'export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER' >> ~/.zshrc
source ~/.zshrc

# Verify tokens are set
echo $SLACK_BOT_TOKEN
echo $SLACK_APP_TOKEN
```

**⚠️ IMPORTANT:** Both tokens are required. Without `SLACK_APP_TOKEN`, you will see `403 Forbidden` errors.

**Solution**:

## See Also

- [Quick Start Guide - Usage](quick-start-usage.md) - Bot usage and testing
- [Quick Start Guide](quick-start.md) - Overview
