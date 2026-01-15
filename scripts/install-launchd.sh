#!/bin/bash
# Install script for macOS launchd service
# This script creates a launchd plist file for auto-starting Neo Slack Bot

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BOT_BINARY="$PROJECT_ROOT/src/bin/neo-slack-bot"
PLIST_NAME="com.neo.slackbot.plist"
PLIST_PATH="$HOME/Library/LaunchAgents/$PLIST_NAME"

# Check if binary exists
if [ ! -f "$BOT_BINARY" ]; then
    echo "Error: Bot binary not found at $BOT_BINARY"
    echo "Please build the project first: cd src && make"
    exit 1
fi

# Check if tokens are set
if [ -z "$SLACK_BOT_TOKEN" ] || [ -z "$SLACK_APP_TOKEN" ]; then
    echo "Warning: SLACK_BOT_TOKEN or SLACK_APP_TOKEN not set in current environment"
    echo "The plist will use environment variables from ~/.zshrc"
    echo "Make sure both tokens are exported in ~/.zshrc"
    echo ""
fi

# Create LaunchAgents directory if it doesn't exist
mkdir -p "$HOME/Library/LaunchAgents"

# Create plist file
cat > "$PLIST_PATH" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.neo.slackbot</string>
    <key>ProgramArguments</key>
    <array>
        <string>$BOT_BINARY</string>
    </array>
    <key>WorkingDirectory</key>
    <string>$PROJECT_ROOT</string>
    <key>EnvironmentVariables</key>
    <dict>
        <key>PATH</key>
        <string>/usr/local/bin:/opt/homebrew/bin:/usr/bin:/bin:/usr/sbin:/sbin</string>
    </dict>
    <key>StandardOutPath</key>
    <string>$HOME/Library/Logs/neo-slack-bot.log</string>
    <key>StandardErrorPath</key>
    <string>$HOME/Library/Logs/neo-slack-bot-error.log</string>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
    <key>ProcessType</key>
    <string>Background</string>
</dict>
</plist>
EOF

echo "Created plist file at: $PLIST_PATH"
echo ""
echo "To load the service (start on login):"
echo "  launchctl load $PLIST_PATH"
echo ""
echo "To start immediately:"
echo "  launchctl start com.neo.slackbot"
echo ""
echo "To stop the service:"
echo "  launchctl stop com.neo.slackbot"
echo ""
echo "To unload the service (disable auto-start):"
echo "  launchctl unload $PLIST_PATH"
echo ""
echo "To check service status:"
echo "  launchctl list | grep com.neo.slackbot"
echo ""
echo "Logs will be written to:"
echo "  $HOME/Library/Logs/neo-slack-bot.log"
echo "  $HOME/Library/Logs/neo-slack-bot-error.log"
echo ""
echo "Note: Make sure SLACK_BOT_TOKEN and SLACK_APP_TOKEN are exported in ~/.zshrc"
