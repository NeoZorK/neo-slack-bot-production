# Installation Guide - Build and Verification

## Configuration

### Environment Variables

Set **both** Slack tokens (Bot Token and App-Level Token):

**Temporary setup (for testing):**
```bash
export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
```

**Permanent Setup** (add to `~/.zshrc` or `~/.bash_profile`):

**For zsh (macOS default):**
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

**⚠️ IMPORTANT:** Both tokens are required. Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error.

### Slack App Configuration

1. **Create Slack App**: https://api.slack.com/apps
2. **Enable Socket Mode**: App Settings → Socket Mode → Enable
3. **Create App-Level Token**: App Settings → Socket Mode → App-Level Tokens
   - Token name: `socket-mode-token`
   - Scope: `connections:write`
   - **IMPORTANT:** Copy the token (starts with `xapp-`) — it's shown only once!
4. **Generate Bot Token**: OAuth & Permissions → Install to Workspace
5. **Set Scopes**: OAuth & Permissions → Bot Token Scopes
   - `app_mentions:read`
   - `channels:read`
   - `chat:write`
   - `im:read`
   - `im:write`

**📖 For detailed instructions, see [Complete Guide: How to Get Slack API Token](getting-slack-token-detailed.md)**

## Verification

### 1. Run Tests

**Using CTest (recommended):**
```bash
cd build
ctest --output-on-failure
```

**Or run test executable directly:**
```bash
cd build
./bin/test_neo_slack_bot
```

Expected output:
```
Running Neo Slack Bot tests...
All tests passed!
```

### 2. Test Application

```bash
cd build
./bin/neo-slack-bot
```

Expected behavior:
- Connects to Slack Socket Mode
- Shows connection confirmation
- Processes incoming events
- Displays macOS notifications

### 3. Verify Notifications

- Send a test message in Slack
- Check that macOS notification appears
- Verify notification content is correct

## Troubleshooting

### Build Issues

**Problem**: `clang++: command not found`
```bash
xcode-select --install
```

**Problem**: `openssl/ssl.h: No such file` or OpenSSL not found
```bash
# CMake should auto-detect OpenSSL, but if it fails:
# Ensure OpenSSL is installed
brew install openssl@3  # or openssl

# Clean and reconfigure CMake
cd build
rm -rf *
cmake ..
cmake --build .
```

**Problem**: `ld: library not found for -lssl`
```bash
# CMake handles library paths automatically
# If issues persist, verify OpenSSL installation:
brew --prefix openssl@3  # or openssl

# Clean and reconfigure
cd build
rm -rf *
cmake ..
cmake --build .
```

### Runtime Issues

**Problem**: `SLACK_BOT_TOKEN not set` or `SLACK_APP_TOKEN not set`

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

**Problem**: Connection fails
- Verify token is correct
- Check Socket Mode is enabled in Slack app
- Verify internet connectivity
- Check firewall settings

**Problem**: Notifications not appearing
- Check System Preferences → Notifications
- Verify app has notification permissions
- Check Do Not Disturb settings

## Uninstallation

### Remove Executable

```bash
# Remove build directory
rm -rf build/

# Or clean build artifacts
cd build
cmake --build . --target clean
```

### Remove Dependencies (Optional)

```bash
brew uninstall openssl  # Only if not used by other software
```

### Remove Project

```bash
cd ..
rm -rf neo-slack-bot/
```

## Next Steps

After installation:
1. Read `docs/quick-start.md` for usage instructions
2. Review `docs/auth-basic.md` for authentication setup
3. Check `docs/roadmap.md` for planned features
4. See `docs/rules.md` for development guidelines

## Additional Resources

- **Slack API Documentation**: https://api.slack.com/
- **Socket Mode Guide**: https://api.slack.com/apis/connections/socket
- **cpp-httplib Documentation**: https://github.com/yhirose/cpp-httplib
- **nlohmann/json Documentation**: https://json.nlohmann.me/

## See Also

- [Installation Guide - Requirements and Setup](installation-setup.md) - Requirements and setup
- [Installation Guide](installation.md) - Overview
