# Production Testing Setup Guide

**Version**: v0.0.6  
**Purpose**: Setup instructions for production testing  
**See also**: `docs/production-testing-checklist.md` for test procedures

---

## Pre-Testing Checklist

### ✅ Code Quality
- [x] All source files compile without errors
- [x] All unit tests pass
- [x] All stress tests pass
- [x] Thread safety documentation added
- [x] Atomic operations used for shared state
- [x] No memory leaks detected
- [x] Code follows project rules (300-line limit, snake_case, etc.)

### ✅ Security
- [x] Token validation implemented
- [x] Input validation at boundaries
- [x] No sensitive data in logs
- [x] SSL/TLS encryption enabled
- [x] Security audit completed

### ✅ Documentation
- [x] Thread safety documented for all classes
- [x] API documentation available
- [x] Installation guide complete
- [x] Quick start guide available

---

## Step 1: Get Slack API Token

### 1.1 Create Slack App

1. Go to https://api.slack.com/apps
2. Click **"Create New App"**
3. Choose **"From scratch"**
4. Enter app name: `Neo Slack Bot Test`
5. Select your workspace
6. Click **"Create App"**

### 1.2 Enable Socket Mode

1. Navigate to **"Socket Mode"** (left sidebar)
2. Toggle **"Enable Socket Mode"** to ON

### 1.3 Configure Bot Token Scopes

Navigate to **"OAuth & Permissions"** → **"Bot Token Scopes"**

**Required Scopes**:
- `app_mentions:read` - Receive mentions
- `channels:read` - Read channel info
- `channels:history` - Read messages
- `chat:write` - Send messages
- `im:read` - Read DMs
- `im:write` - Send DMs
- `users:read` - Read user info

### 1.4 Install App to Workspace

1. Scroll to **"OAuth Tokens for Your Workspace"**
2. Click **"Install to Workspace"**
3. Review permissions and click **"Allow"**
4. Copy **Bot User OAuth Token** (starts with `xoxb-`)

### 1.5 Verify Token

```bash
# Check token format
echo $SLACK_BOT_TOKEN | head -c 10
# Should output: xoxb-1234
```

---

## Step 2: Build and Prepare

### 2.1 Build Release Version

```bash
cd src
make clean
make
```

### 2.2 Verify Build

```bash
./bin/neo-slack-bot --version
# Should output: Neo Slack Bot v0.0.6

./bin/neo-slack-bot --help
# Should show help message
```

### 2.3 Set Environment Variable

```bash
export SLACK_BOT_TOKEN=xoxb-your-token-here
```

**For permanent setup**:
```bash
echo 'export SLACK_BOT_TOKEN=xoxb-your-token' >> ~/.zshrc
source ~/.zshrc
```

---

## Step 3: Start Bot

### 3.1 Start Bot

```bash
cd src
./bin/neo-slack-bot
```

### 3.2 Expected Output

```
2026-01-XX XX:XX:XX.XXX [INFO ] Neo Slack Bot v0.0.6 starting...
2026-01-XX XX:XX:XX.XXX [INFO ] Configuration loaded from: ...
2026-01-XX XX:XX:XX.XXX [INFO ] Bot is running. Press Ctrl+C to stop.
2026-01-XX XX:XX:XX.XXX [INFO ] Automatic reconnection enabled.
2026-01-XX XX:XX:XX.XXX [INFO ] Watchdog timer enabled (timeout: 300 seconds).
2026-01-XX XX:XX:XX.XXX [INFO ] Connected to Slack Socket Mode
```

### 3.3 Verify Connection

**Test**: Send a message in any Slack channel

**Expected**:
- ✅ Console shows event processing logs
- ✅ macOS notification appears
- ✅ No errors in console

---

## Troubleshooting

### Problem: Bot doesn't connect

**Check**:
1. Token is correct
2. Socket Mode enabled
3. Internet connection working
4. Firewall not blocking

**Solution**: Verify token and Socket Mode settings

### Problem: Notifications don't appear

**Check**:
1. Notification permissions granted
2. Do Not Disturb disabled
3. Notification Center enabled

**Solution**: Grant notification permissions in System Preferences

### Problem: Commands don't work

**Check**:
1. Bot has required scopes
2. Bot installed in workspace
3. Commands registered correctly

**Solution**: Reinstall app with correct scopes

---

**For test procedures, see**: `docs/production-testing-checklist.md`
