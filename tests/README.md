# Test Suite

## Overview

The Neo Slack Bot test suite is organized by test categories for better maintainability and clarity.

## Test Structure

```
tests/
├── unit/              # Unit tests for individual components
├── integration/       # Integration tests with mocks
├── security/          # Security and input validation tests
├── performance/       # Performance and latency tests
├── stress/            # Stress and load tests
├── benchmark/         # Benchmark tests
├── e2e/               # End-to-end Slack API tests
├── mocks/             # Mock objects for testing
├── test_main.cpp      # Test runner
└── CMakeLists.txt     # Build configuration
```

## Test Categories

### Unit Tests (`unit/`)

Tests for individual components in isolation:

- `test_websocket.cpp` - WebSocket client tests
- `test_macos_notifications.cpp` - macOS notification tests
- `test_http_client.cpp` - HTTP client tests
- `test_slack_api.cpp` - Slack API wrapper tests
- `test_event_parser.cpp` - Event parser tests
- `test_settings.cpp` - Settings management tests
- `test_json_parser.cpp` - JSON parser tests
- `test_config_loader.cpp` - Configuration loader tests
- `test_message_filter_extended.cpp` - Message filter tests
- `test_logger.cpp` - Logger tests
- `test_error_recovery.cpp` - Error recovery tests
- `test_rate_limiter.cpp` - Rate limiter tests
- `test_input_validator.cpp` - Input validator tests
- `test_metrics.cpp` - Metrics system tests
- `test_command_handler.cpp` - Command handler tests

### Integration Tests (`integration/`)

Tests for component interactions:

- `test_integration.cpp` - General integration tests
- `test_slack_integration.cpp` - Slack API integration tests

### Security Tests (`security/`)

Security and input validation tests:

- `test_security.cpp` - Core security tests
- `test_security_input.cpp` - Input validation security tests

### Performance Tests (`performance/`)

Performance and latency tests:

- `test_performance.cpp` - Basic performance tests
- `test_performance_latency.cpp` - Latency tests with statistics
- `test_performance_throughput.cpp` - Throughput tests
- `test_performance_advanced.cpp` - Advanced performance tests

### Stress Tests (`stress/`)

Stress and load tests:

- `test_stress.cpp` - Stress test coordinator
- `test_stress_connection.cpp` - Connection stress tests
- `test_stress_components.cpp` - Component stress tests
- `test_stress_system.cpp` - System stress tests
- `test_stress_concurrent.cpp` - Concurrent stress tests
- `test_stress_advanced.cpp` - Advanced stress tests

### Benchmark Tests (`benchmark/`)

Benchmark tests:

- `test_benchmark.cpp` - Benchmark tests

### End-to-End Tests (`e2e/`)

Real Slack API integration tests:

- `test_slack_e2e.cpp` - End-to-end Slack tests

## Running Tests

### Run All Tests

```bash
cd build
ctest --output-on-failure
```

### Run Specific Test Categories

To run specific tests, you can filter by name:

```bash
cd build
# Run only unit tests
ctest -R "Unit" --output-on-failure

# Run only specific test file
ctest -R "test_rate_limiter" --output-on-failure
```

```bash
# Unit tests only
make test-unit
./bin/test_neo_slack_bot --unit

# Integration tests only
make test-integration
./bin/test_neo_slack_bot --integration

# Security tests only
make test-security
./bin/test_neo_slack_bot --security

# Performance tests only
make test-performance
./bin/test_neo_slack_bot --performance

# Stress tests only
make test-stress
./bin/test_neo_slack_bot --stress

# Benchmark tests only
make test-benchmark
./bin/test_neo_slack_bot --benchmark

# E2E tests only
make test-e2e
./bin/test_neo_slack_bot --e2e
```

### Run Performance Tests (includes stress and benchmark)

```bash
make test-perf
./bin/test_neo_slack_bot --perf
```

### Run with Coverage

```bash
make test-cov
```

## Test Requirements

### Unit Tests

- ✅ Test individual components in isolation
- ✅ Use mocks where appropriate
- ✅ Fast execution (< 1 second per test)
- ✅ No external dependencies

### Integration Tests

- ✅ Test component interactions
- ✅ Use mock objects for external services
- ✅ Verify correct data flow
- ✅ Test error propagation

### Security Tests

- ✅ Test input validation
- ✅ Test injection attack prevention
- ✅ Test buffer overflow prevention
- ✅ Test rate limiting
- ✅ Test sanitization

### Performance Tests

- ✅ Measure latency (min, max, mean, median, P95, P99)
- ✅ Measure throughput (operations/sec)
- ✅ Measure memory footprint
- ✅ Measure CPU utilization
- ✅ Set performance thresholds

### Stress Tests

- ✅ Test under high load
- ✅ Test concurrent operations
- ✅ Test memory usage under load
- ✅ Test error handling under stress
- ✅ Verify no resource leaks

### Benchmark Tests

- ✅ Compare performance metrics
- ✅ Identify bottlenecks
- ✅ Measure resource usage
- ✅ Track performance over time

### E2E Tests

- ✅ Test real Slack API integration
- ✅ Require valid tokens (skip if not set)
- ✅ Test message sending/receiving
- ✅ Test command handling
- ✅ Test filtering and notifications

## Coverage

Target: **100% code coverage**

Check coverage:
```bash
make test-cov
# Coverage reports generated in default.profdata
```

## Adding New Tests

1. **Choose the right category**:
   - Unit test → `unit/`
   - Integration test → `integration/`
   - Security test → `security/`
   - Performance test → `performance/`
   - Stress test → `stress/`
   - Benchmark test → `benchmark/`
   - E2E test → `e2e/`

2. **Follow naming convention**:
   - File: `test_<component>.cpp`
   - Function: `test_<component>_<scenario>()`

3. **Add to test_main.cpp**:
   - Add function declaration
   - Add function call in appropriate section

4. **Update Makefile** (if needed):
   - New files are automatically included via wildcards

5. **Update documentation**:
   - Add to `docs/testing.md`
   - Add to `docs/INDEX.md`

## Test Best Practices

1. **Isolation**: Each test should be independent
2. **Clarity**: Test names should describe what they test
3. **Assertions**: Use clear assertions with meaningful messages
4. **Cleanup**: Clean up resources after tests
5. **Speed**: Keep tests fast (< 1 second per test when possible)
6. **Coverage**: Aim for 100% code coverage
7. **Documentation**: Document complex test scenarios

## Troubleshooting

### Tests Fail to Compile

- Check file paths in Makefile
- Verify all dependencies are included
- Check for missing function declarations

### Tests Fail at Runtime

- Check test data and mocks
- Verify environment variables (for E2E tests)
- Check for resource leaks

### Coverage Not 100%

- Identify uncovered code paths
- Add tests for edge cases
- Add tests for error paths
- Verify all branches are tested

---

For more information, see:
- **Testing Guide**: `docs/testing.md`
- **Manual Testing**: `docs/manual-testing-guide.md`
- **User Guide**: `docs/user-guide.md`
