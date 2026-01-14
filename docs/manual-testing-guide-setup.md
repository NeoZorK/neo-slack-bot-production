# Manual Testing Guide - Setup

## Overview

This guide provides step-by-step instructions for manually testing Neo Slack Bot functionality, including message sending/receiving, notifications, channels, and multi-user scenarios.

## Prerequisites

Before testing, ensure:

1. ✅ Bot is built and executable exists: `src/bin/neo-slack-bot`
2. ✅ Both tokens are set: `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN`
3. ✅ Bot is installed in your Slack workspace
4. ✅ Bot is invited to test channels
5. ✅ macOS notification permissions are granted

## Quick Test Setup

### 1. Prepare Test Environment

```bash
# Set tokens
export SLACK_BOT_TOKEN=xoxb-your-token
export SLACK_APP_TOKEN=xapp-1-your-token

# Optional: Set test channel ID
export SLACK_TEST_CHANNEL=C12345678

# Verify tokens
echo "Bot Token: ${SLACK_BOT_TOKEN:0:20}..."
echo "App Token: ${SLACK_APP_TOKEN:0:20}..."
```

### 2. Start the Bot

```bash
cd /path/to/neo-slack-bot/src
./bin/neo-slack-bot
```

**Expected Output**:
```
Starting Neo Slack Bot...
Connecting to Slack Socket Mode...
Connected to Slack Socket Mode
Bot is running. Press Ctrl+C to stop.
```

## See Also

- [Manual Testing Guide - Test Scenarios](manual-testing-guide-scenarios.md) - Detailed test scenarios
- [Manual Testing Guide - Advanced](manual-testing-guide-advanced.md) - CLI commands, troubleshooting, advanced testing
- [User Guide](user-guide.md) - Complete user guide
