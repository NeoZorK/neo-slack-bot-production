# Testing Reference - Test Execution

**Purpose**: Set up common test environment

**Usage** (future):
```cpp
class WebSocketTestFixture {
public:
    WebSocketClient client;
    
    void SetUp() {
        client = WebSocketClient("test-token");
    }
    
    void TearDown() {
        client.disconnect();
    }
};
```

## Mocking

### Mock Objects

**Purpose**: Isolate units under test

**Use Cases**:
- External API calls
- System calls
- Network operations
- File I/O operations

**Example** (future):
```cpp
class MockHttpClient : public HttpClient {
public:
    MOCK_METHOD(bool, get, (const std::string&, std::string&));
    MOCK_METHOD(bool, post, (const std::string&, const std::string&, std::string&));
};
```

### Mock Frameworks

**Future Options**:
- **Google Mock**: C++ mocking framework
- **FakeIt**: Simple mocking library
- **Trompeloeil**: Header-only mocking

## Continuous Integration

### CI Pipeline

**Current Implementation**: GitHub Actions

**Stages**:
1. **Install Dependencies**: OpenSSL, LLVM tools
2. **Format Check**: Verify code formatting (clang-format)
3. **Lint Check**: Run static analysis (clang-tidy)
4. **Build**: Compile source code
5. **Build Tests**: Compile test suite
6. **Run Tests**: Execute test suite
7. **Coverage**: Generate coverage report (optional)
8. **Documentation Check**: Verify documentation quality

### CI Configuration

**Workflow File**: `.github/workflows/ci.yml`

**Runners**:
- **macOS Latest**: For build and test jobs
- **Ubuntu Latest**: For documentation checks

**Triggers**:
- Release creation (`release` event with types `created` or `published`)
- Tag push (tags matching `v*` pattern, e.g., `v0.0.6`)

**Note**: CI runs only for releases, not on every push or pull request. This reduces CI costs and focuses testing on production-ready code.

### CI Features

**Build Configuration**:
- Automatic OpenSSL path detection (`brew --prefix openssl`)
- LLVM tools setup for linting
- Clean builds for reproducibility
- Debug and release builds supported

**Test Execution**:
- Full test suite execution
- Test output capture
- Non-blocking test failures (with `|| true` for optional checks)

**Code Quality**:
- Format checking (non-blocking)
- Linting (non-blocking)
- File size verification
- Documentation checks

**Coverage**:
- Optional coverage generation
- LLVM profiling support
- Coverage reports (future)

### CI Best Practices

1. **Keep Tests Fast**: Tests should complete quickly
2. **Isolated Tests**: Tests should not depend on external services
3. **Reproducible Builds**: Use consistent dependency versions
4. **Clear Error Messages**: Tests should provide clear failure information
5. **Non-Blocking Checks**: Optional checks shouldn't block CI

### Troubleshooting CI

**Common Issues**:
- **OpenSSL Path**: Verify `brew --prefix openssl` works
- **LLVM Path**: Check `brew --prefix llvm` is correct
- **Build Failures**: Review compiler flags and dependencies
- **Test Failures**: Check test output and environment

**Debugging**:
- Review GitHub Actions logs
- Test locally with same environment
- Check dependency versions
- Verify file paths and permissions

## Troubleshooting

### Common Issues

**Problem**: Tests fail intermittently
- **Solution**: Check for race conditions, use proper synchronization

**Problem**: Tests are slow
- **Solution**: Optimize test setup, use mocks for slow operations

**Problem**: Tests depend on external services
- **Solution**: Use mocks, create test fixtures

**Problem**: Coverage is low
- **Solution**: Add tests for untested code paths

## Future Enhancements

### Planned Features

- Test framework integration (Catch2/Google Test)
- Mock framework integration
- Coverage reporting
- Performance benchmarks
- Property-based testing
- Fuzz testing

### Test Infrastructure

- CI/CD integration
- Automated test runs
- Coverage tracking
- Test result reporting
- Performance regression testing

## See Also

- [Testing Guide](testing-guide.md) - Getting started with testing

## See Also

- [Testing Reference - Test Categories](testing-reference-categories.md) - Test categories
- [Testing Reference](testing-reference.md) - Overview
