## Linux Production Checklist (v0.0.7)

### Environment
- [ ] Reference distro selected (e.g. Ubuntu 22.04)
- [ ] CMake 3.15+ and Ninja installed
- [ ] GCC or Clang installed
- [ ] OpenSSL development package installed
- [ ] `libnotify` and `pkg-config` installed

### Build and Tests (Host or Docker)
- [ ] `build` directory configured with CMake + Ninja
- [ ] `neo-slack-bot` builds in **Release** mode
- [ ] `test_neo_slack_bot` builds successfully
- [ ] `ctest --output-on-failure` passes on Linux
- [ ] Docker image builds from project root
- [ ] Docker tests pass (`docker run neo-slack-bot:test`)

### Slack Configuration
- [ ] Slack app configured with required scopes
- [ ] Socket Mode enabled
- [ ] `SLACK_BOT_TOKEN` set inside environment
- [ ] `SLACK_APP_TOKEN` set inside environment
- [ ] Test channel ID exported if needed

### Functional Checks
- [ ] Bot connects to Slack from Linux environment
- [ ] Messages from test channel are processed
- [ ] Replies are delivered correctly
- [ ] Filters and basic routing behave as expected

### Linux Integration
- [ ] Desktop notifications verified on at least one Linux desktop session
- [ ] libnotify backend produces visible notifications
- [ ] System metrics (CPU/memory) return reasonable non-zero values

### Reliability
- [ ] Bot survives short network outages
- [ ] Automatic reconnection works on Linux
- [ ] Long-running test (30+ minutes) shows no crashes

### Security
- [ ] Tokens provided only via environment
- [ ] No secrets written to logs or config files
- [ ] SSL/TLS works for all HTTP API calls

### Documentation
- [ ] `linux-support.md` acceptance checklist reviewed
- [ ] Docker docs (`docker-setup.md`, `docker-usage.md`) validated

