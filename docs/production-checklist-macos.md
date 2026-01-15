## macOS Production Checklist (v0.0.7)

### Environment
- [ ] macOS 12+ or 13+ installed (Intel or Apple Silicon)
- [ ] Xcode command line tools installed
- [ ] Homebrew installed
- [ ] CMake 3.15+ and Ninja installed
- [ ] OpenSSL installed via Homebrew (`openssl@3`)

### Build and Tests
- [ ] Clean `build` directory recreated with CMake + Ninja
- [ ] Configure succeeds with generator **Ninja**
- [ ] `neo-slack-bot` builds in **Release** mode
- [ ] `test_neo_slack_bot` builds successfully
- [ ] `ctest --output-on-failure` passes locally

### Slack Configuration
- [ ] Slack app created with required scopes
- [ ] Socket Mode enabled
- [ ] Bot token set via `SLACK_BOT_TOKEN`
- [ ] App-level token set via `SLACK_APP_TOKEN`
- [ ] Test channel ID configured (e.g. `SLACK_TEST_CHANNEL`)

### Functional Checks
- [ ] Bot starts without errors
- [ ] Connects to Slack Socket Mode
- [ ] Receives messages from test channel
- [ ] Sends responses successfully
- [ ] Command handling works for basic commands

### macOS Integration
- [ ] Notifications appear in Notification Center
- [ ] Notification content (title/body) is correct
- [ ] Do Not Disturb behavior respected
- [ ] App has notification permissions granted

### Reliability
- [ ] Handles temporary network loss
- [ ] Automatically reconnects after disconnect
- [ ] No crashes during extended run (30+ minutes)

### Security
- [ ] Tokens only set via environment variables
- [ ] No tokens logged to console or files
- [ ] SSL/TLS enabled for Slack HTTP API

### Documentation
- [ ] Installation and setup docs verified on macOS
- [ ] User configuration docs match actual behavior

