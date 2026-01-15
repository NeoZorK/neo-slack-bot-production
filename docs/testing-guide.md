# Testing Guide

## Overview

Neo Slack Bot follows a comprehensive testing strategy with a target of 100% code coverage. All tests are located in the `tests/` directory and follow a structure that mirrors the source code organization.

## Testing Philosophy

### Principles

1. **Test Everything**: All public APIs must have tests
2. **Test Early**: Write tests alongside code
3. **Test Often**: Run tests frequently during development
4. **Test Independently**: Tests should not depend on each other
5. **Test Realistically**: Use realistic test data and scenarios

### Goals

- **100% Code Coverage**: All code paths tested
- **Fast Execution**: Tests run quickly (< 1 second)
- **Reliable**: Tests are deterministic and stable
- **Maintainable**: Tests are easy to understand and update

## Test Structure

### Directory Organization

```
tests/
├── Makefile                 # Test build configuration
├── test_main.cpp           # Test runner and main
├── unit/                   # Unit tests
├── integration/           # Integration tests
├── security/              # Security tests
├── performance/           # Performance tests
├── stress/                # Stress tests
├── benchmark/             # Benchmark tests
├── e2e/                   # End-to-end tests
└── mocks/                 # Mock objects
```

### Test File Naming

**Convention**: `test_<module>.cpp` in appropriate category directory

**Examples**:
- `unit/test_websocket.cpp` - Unit tests for `websocket_client`
- `integration/test_integration.cpp` - Integration tests
- `security/test_security.cpp` - Security tests

### Test Function Naming

**Convention**: `test_<component>_<scenario>_<expected_result>`

**Examples**:
- `test_websocket_connection_success`
- `test_notification_display_failure`
- `test_http_client_get_timeout`

## Running Tests

### Build Tests

```bash
cd tests
make test
```

**Output**:
```
Running Neo Slack Bot tests...
test_websocket_client_creation... PASSED
test_macos_notifications_display... PASSED
test_http_client_get... PASSED
All tests passed!
```

### Run Specific Test

**Future** (with test framework):
```bash
./bin/test_neo_slack_bot --test-name test_websocket_connection
```

### Debug Tests

**Build with Debug Symbols**:
```bash
cd tests
make debug
gdb ./bin/test_neo_slack_bot
```

## Test Coverage

### Coverage Target

**Goal**: 100% code coverage

**Metrics**:
- Line coverage: All lines executed
- Branch coverage: All branches tested
- Function coverage: All functions called
- Statement coverage: All statements executed

### Coverage Tools

**Future Options**:
- **gcov**: GCC coverage tool
- **llvm-cov**: LLVM coverage tool

### Coverage Reports

**Generate Report** (future):
```bash
make coverage
# Generates coverage.html
```

**View Report**:
- Open `coverage.html` in browser
- See line-by-line coverage
- Identify untested code
- Track coverage trends

## Best Practices

### Test Organization

1. **One Test Per Scenario**: Each test verifies one thing
2. **Clear Test Names**: Descriptive test function names
3. **Arrange-Act-Assert**: Structure tests clearly
4. **Isolated Tests**: Tests don't depend on each other
5. **Fast Tests**: Keep tests fast (< 100ms each)

### Test Data

1. **Realistic Data**: Use realistic test data
2. **Edge Cases**: Test boundary conditions
3. **Error Cases**: Test error handling
4. **Invalid Input**: Test input validation

### Test Maintenance

1. **Keep Tests Updated**: Update tests with code changes
2. **Remove Dead Tests**: Delete obsolete tests
3. **Refactor Tests**: Improve test code quality
4. **Document Tests**: Comment complex test logic

### Test Quality

1. **Readable**: Tests should be easy to understand
2. **Maintainable**: Easy to modify and extend
3. **Reliable**: Tests should be stable
4. **Fast**: Quick execution for rapid feedback

## Test Checklist

### Before Committing

- [ ] All tests pass
- [ ] New code has tests
- [ ] Tests are fast (< 1 second total)
- [ ] Tests are isolated
- [ ] Test names are descriptive
- [ ] Coverage is maintained

### Code Review

- [ ] Tests cover new functionality
- [ ] Edge cases are tested
- [ ] Error handling is tested
- [ ] Tests are readable
- [ ] No test duplication

## See Also

- [Testing Reference](testing-reference.md) - Detailed test information and advanced topics
