## Windows Production Checklist (v0.0.7)

### Environment
- [ ] Windows 10/11 x64 selected as target
- [ ] MSVC or MinGW-w64 toolchain installed
- [ ] CMake 3.15+ available
- [ ] OpenSSL for Windows installed

### Build and Tests (Native Windows)
- [ ] `build` directory configured with CMake for Windows
- [ ] `neo-slack-bot` builds in **Release** mode
- [ ] `test_neo_slack_bot` builds successfully
- [ ] Core unit tests run on Windows

### Cross-Build and Wine (Docker Support)
- [ ] Docker image built with `wine32`, `xvfb`, and MinGW toolchain
- [ ] Static linking enabled (`-static-libgcc -static-libstdc++`) for MinGW
- [ ] `WINEPATH` configured to include DLL paths (MinGW runtime, OpenSSL)
- [ ] Tests run headless using `xvfb-run`
- [ ] Windows `.exe` binaries verified in Docker/Wine environment

### Slack Configuration
- [ ] Slack app configured as for other platforms
- [ ] `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN` set
- [ ] Test channel ID available if tests require it

### Functional Checks
- [ ] Bot starts on Windows without crashes
- [ ] Connects to Slack and processes minimal event flow
- [ ] Console-based notifications appear where expected

### Current Limitations (v0.0.7)
- [ ] Native Windows notifications still stubbed (console only)
- [ ] System metrics may use placeholder values
- [ ] Wine-based tests require `xvfb` for display emulation

### Reliability
- [ ] Short manual run on Windows shows stable behavior
- [ ] No critical crashes during basic scenarios

### Security
- [ ] Tokens configured only via environment or secure store
- [ ] No sensitive data written to Windows event logs

### Documentation
- [ ] `windows-support.md` status and roadmap reviewed
- [ ] Known limitations documented for Windows deployment

