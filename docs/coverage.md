# Code Coverage Guide

## Overview

Code coverage reporting is integrated into the CMake build system. It supports both Clang (llvm-cov) and GCC (gcov/lcov) toolchains.

## Prerequisites

### For Clang (macOS default)

```bash
brew install llvm
```

This installs:
- `llvm-cov` - Coverage reporting tool
- `llvm-profdata` - Profile data merging tool

### For GCC (Linux)

```bash
# Ubuntu/Debian
sudo apt-get install lcov

# Or via package manager
sudo apt-get install gcov lcov genhtml
```

## Building with Coverage

### Step 1: Configure CMake with Coverage

```bash
cd build
cmake -G Ninja -DENABLE_COVERAGE=ON ..
```

This enables:
- `-fprofile-instr-generate -fcoverage-mapping` (Clang)
- `--coverage` (GCC)

### Step 2: Build Tests

```bash
cmake --build .
```

### Step 3: Run Tests

```bash
ctest --output-on-failure
# Or directly:
./bin/test_neo_slack_bot
```

Running tests generates profiling data:
- Clang: `default.profraw` files
- GCC: `.gcda` and `.gcno` files

### Step 4: Generate Coverage Report

```bash
cmake --build . --target coverage
```

This generates:
- HTML report in `build/coverage/index.html`
- Summary statistics in console output

## Viewing Coverage Reports

### HTML Report

Open `build/coverage/index.html` in your browser to view:
- Line-by-line coverage
- File-level statistics
- Overall coverage percentage
- Uncovered lines highlighted

### Console Output

The coverage target also prints summary statistics:
```
File Coverage:
  src/main.cpp: 95.5%
  src/websocket_client.cpp: 98.2%
  ...
Overall Coverage: 96.8%
```

## Coverage Targets

### Clang (llvm-cov)

The coverage target:
1. Merges `default.profraw` files into `default.profdata`
2. Generates HTML report using `llvm-cov show`
3. Prints summary using `llvm-cov report`

**Excluded patterns**:
- `.*test.*` - Test files
- `.*/usr/.*` - System headers
- `.*/opt/.*` - System libraries

### GCC (gcov/lcov)

The coverage target:
1. Captures coverage data using `lcov`
2. Filters out test files and system headers
3. Generates HTML report using `genhtml`

## Coverage Goals

**Target**: 100% code coverage

**Current Status**: Check coverage reports to identify gaps.

## Integration with CI/CD

Coverage can be enabled in CI/CD:

```yaml
- name: Build with coverage
  run: |
    cd build
    cmake -DENABLE_COVERAGE=ON ..
    cmake --build .
    
- name: Run tests
  run: |
    cd build
    ctest --output-on-failure
    
- name: Generate coverage
  run: |
    cd build
    cmake --build . --target coverage
  continue-on-error: true
```

## Troubleshooting

### Coverage Not Generated

**Issue**: `llvm-cov` or `llvm-profdata` not found

**Solution**:
```bash
# Check if installed
which llvm-cov
which llvm-profdata

# Install if missing
brew install llvm

# Verify paths
ls /opt/homebrew/opt/llvm/bin/llvm-cov
ls /opt/homebrew/opt/llvm/bin/llvm-profdata
```

### No Profiling Data

**Issue**: `default.profraw` files not generated

**Solution**:
- Ensure `ENABLE_COVERAGE=ON` is set
- Run tests after building
- Check that tests actually execute code paths

### Coverage Report Empty

**Issue**: Report shows 0% coverage

**Solution**:
- Verify tests are running
- Check that profiling data files exist
- Ensure source files are compiled with coverage flags

### Wrong Coverage Numbers

**Issue**: Coverage seems incorrect

**Solution**:
- Clean build: `rm -rf build && mkdir build`
- Rebuild with coverage enabled
- Run all tests
- Check excluded patterns match your expectations

## Best Practices

1. **Run Full Test Suite**: Coverage only reflects executed code paths
2. **Clean Builds**: Start with clean build directory for accurate results
3. **Regular Checks**: Generate coverage reports regularly to track progress
4. **Focus on Gaps**: Use reports to identify untested code paths
5. **CI Integration**: Automate coverage generation in CI/CD pipeline

## See Also

- [Development Setup](development-setup.md) - Development environment setup
- [Testing Guide](testing-guide.md) - Testing best practices
- [Testing Reference](testing-reference.md) - Test execution details
