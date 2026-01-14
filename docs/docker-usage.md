# Docker Guide - Usage and Examples

  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build Docker image
        run: docker build -t neo-slack-bot:test .
      - name: Run tests
        run: docker run --rm neo-slack-bot:test
```

### GitLab CI Example

```yaml
test:
  image: docker:latest
  services:
    - docker:dind
  script:
    - docker build -t neo-slack-bot:test .
    - docker run --rm neo-slack-bot:test
```

## Production Considerations (v0.0.7)

### ⚠️ Important Notes

**Docker is NOT recommended for production deployment** because:

1. **macOS Notifications**: Docker containers cannot access macOS notification system (`macos_notifications.cpp` uses macOS-specific APIs)
2. **Native Integration**: The bot requires macOS-specific frameworks (Foundation, AppKit) which are not available in Linux containers
3. **System Integration**: Background execution and system services require macOS
4. **Build Limitations**: Docker build uses Linux-compatible flags, excluding macOS frameworks (`-framework Foundation -framework AppKit`)
5. **Incomplete Testing**: Some features (notifications, macOS-specific APIs) cannot be tested in Docker

### Build Limitations in Docker

The Dockerfile builds with Linux-compatible flags:
- **OpenSSL**: Uses system OpenSSL (`/usr/lib/x86_64-linux-gnu`) instead of Homebrew paths
- **Frameworks**: Excludes macOS frameworks (`-framework Foundation -framework AppKit`)
- **macOS APIs**: macOS-specific code paths may be disabled or stubbed

**Result**: Docker builds are suitable for testing core functionality (WebSocket, HTTP API, parsing) but **not** for testing macOS-specific features.

### Use Cases for Docker (Test and CI Matrix)

✅ **Recommended For**:
- Automated testing in CI/CD pipelines (Linux runners)
- Cross-platform development testing (Linux compatibility for core features)
- Code quality checks (linting, formatting)
- Test isolation and reproducibility
- Continuous integration
- Testing core functionality (WebSocket, HTTP API, JSON parsing, event handling)
- Testing without macOS dependencies

❌ **NOT Recommended For**:
- Production deployment (requires macOS)
- macOS notification testing (not available in containers)
- Native macOS feature testing (frameworks not available)
- User-facing applications
- Full feature testing (macOS-specific features disabled)

## Troubleshooting

### Build Issues

**Problem**: `libssl-dev` not found
```bash
# Update package list
docker run --rm -it ubuntu:22.04 bash -c "apt-get update && apt-get install -y libssl-dev"
```

**Problem**: Build fails with OpenSSL errors
```bash
# Check OpenSSL installation
docker run --rm neo-slack-bot:latest openssl version

# Verify OpenSSL paths
docker run --rm neo-slack-bot:latest ls -la /usr/lib/x86_64-linux-gnu/libssl*
```

**Problem**: Build fails with "framework not found" errors
```bash
# This is expected - Dockerfile uses Linux-compatible flags
# macOS frameworks are excluded intentionally
# Check Dockerfile for correct LDFLAGS
```

**Problem**: Tests fail due to missing macOS APIs
```bash
# This is expected - macOS-specific features are not available in Docker
# Tests that require macOS APIs will fail or be skipped
# Run full test suite on macOS for complete coverage
```

### Runtime Issues

**Problem**: Tests fail in Docker but pass locally
- **Expected**: macOS-specific tests (notifications) will fail in Docker
- Check if test requires macOS APIs (`macos_notifications.cpp`)
- Verify test environment matches Docker environment (Linux vs macOS)
- Check file permissions and paths
- Review test output for macOS-specific error messages

**Problem**: Permission denied errors
```bash
# Run with proper user
docker run --rm --user $(id -u):$(id -g) neo-slack-bot:latest
```

**Problem**: Missing environment variables
```bash
# Both tokens are required (even for testing)
docker run --rm \
  -e SLACK_BOT_TOKEN=test-token \
  -e SLACK_APP_TOKEN=test-app-token \
  neo-slack-bot:latest
```

**Problem**: macOS notification tests fail
- **Expected behavior**: macOS notifications are not available in Docker
- These tests should be skipped or mocked in Docker environment
- Run notification tests on macOS for full coverage

## Advanced Usage

### Multi-stage Build

For optimized production images (if needed):

```dockerfile
FROM ubuntu:22.04 AS builder
# ... build steps ...

FROM ubuntu:22.04 AS runtime
COPY --from=builder /app/bin/neo-slack-bot /usr/local/bin/
CMD ["neo-slack-bot"]
```

### Custom Dockerfile

Create `Dockerfile.dev` for development:

```dockerfile
FROM neo-slack-bot:latest
RUN apt-get update && apt-get install -y vim gdb
WORKDIR /app
```

Build and run:
```bash
docker build -f Dockerfile.dev -t neo-slack-bot:dev .
docker run --rm -it neo-slack-bot:dev bash
```

### Run Performance/Stress Tests (Optional)

```bash
docker run --rm neo-slack-bot:latest bash -c "cd tests && make test-perf"
```

## Running Windows Tests in Docker

The Docker image includes MinGW and Wine for cross-platform testing.

### Run Windows Tests (Wine)

```bash
docker run --rm neo-slack-bot:latest /bin/bash -c "mkdir -p build_win && cd build_win && cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake .. && ninja && ninja test_windows_wine"
```

> **Note**: This uses `xvfb-run` internally to support headless execution of Windows binaries.

## Best Practices (Aligned with v0.0.7 Plan)

1. **Use .dockerignore**: Exclude unnecessary files (build artifacts, macOS-specific files, logs)
2. **Layer Caching**: Order Dockerfile commands for optimal caching
3. **Security**: Don't include secrets in images (use environment variables)
4. **Size**: Keep images small by removing build dependencies
5. **Tags**: Use semantic versioning for tags
6. **Environment Variables**: Always set both `SLACK_BOT_TOKEN` and `SLACK_APP_TOKEN`
7. **Platform Awareness**: Remember that Docker tests Linux compatibility, not macOS features
8. **Test Coverage**: Run full test suite on macOS for complete coverage including notifications

## Additional Resources

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Documentation](https://docs.docker.com/compose/)
- [CI/CD Integration](docs/testing-guide.md)

## See Also

- [Docker Guide - Overview and Setup](docker-setup.md) - Overview and setup
- [Docker Guide](docker.md) - Overview
