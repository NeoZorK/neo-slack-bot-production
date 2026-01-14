#!/bin/bash
# Uninstall script for macOS launchd service

set -e

PLIST_NAME="com.neo.slackbot.plist"
PLIST_PATH="$HOME/Library/LaunchAgents/$PLIST_NAME"

if [ ! -f "$PLIST_PATH" ]; then
    echo "Error: Plist file not found at $PLIST_PATH"
    echo "Service may not be installed."
    exit 1
fi

# Stop and unload the service
echo "Stopping service..."
launchctl stop com.neo.slackbot 2>/dev/null || true
launchctl unload "$PLIST_PATH" 2>/dev/null || true

# Remove plist file
echo "Removing plist file..."
rm -f "$PLIST_PATH"

echo "Service uninstalled successfully."
