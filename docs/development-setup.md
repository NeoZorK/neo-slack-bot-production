# Development Setup

## Prerequisites

Install development tools:

```bash
# Install CMake (required, version 3.15+)
brew install cmake

# Install Ninja (recommended for faster builds)
brew install ninja

# Install clang-format
brew install clang-format

# Install clang-tidy (via LLVM)
brew install llvm
export PATH="/usr/local/opt/llvm/bin:$PATH"
```

## Development Workflow

### 1. Format Code

```bash
./scripts/format.sh
```

Or manually:
```bash
clang-format -i src/**/*.{cpp,hpp} tests/**/*.{cpp,hpp}
```

### 2. Lint Code

```bash
./scripts/lint.sh
```

### 3. Check File Sizes

```bash
./scripts/check-file-sizes.sh
```

### 4. Build with CMake

**First-time setup:**
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (uses Ninja if available, otherwise Makefiles)
cmake ..

# Build (parallel build automatically enabled)
cmake --build .
```

**Subsequent builds:**
```bash
cd build
cmake --build .
```

**Build options:**
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Release build (default)
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Specify number of parallel jobs
cmake --build . -j 8
```

**Output location:**
- Executable: `build/bin/neo-slack-bot`
- Test executable: `build/bin/test_neo_slack_bot`

### 5. Test

**Using CMake/CTest:**
```bash
cd build
ctest --output-on-failure
```

**Or run test executable directly:**
```bash
cd build
./bin/test_neo_slack_bot
```

### 6. Generate Coverage Report

**Build with coverage enabled:**
```bash
cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build .
ctest --output-on-failure
```

**Generate coverage report:**
```bash
cd build
cmake --build . --target coverage
```

**View coverage report:**
- HTML report: Open `build/coverage/index.html` in browser
- For Clang: Uses llvm-cov (requires `brew install llvm`)
- For GCC: Uses lcov and genhtml (requires `brew install lcov`)

**Note**: Coverage requires running tests first. The coverage target will merge profiling data and generate HTML reports.

## Pre-commit Checklist

Before committing:

- [ ] Code formatted (`./scripts/format.sh`)
- [ ] No linting errors (`./scripts/lint.sh`)
- [ ] All files under 300 lines (`./scripts/check-file-sizes.sh`)
- [ ] All tests pass locally (`cd build && ctest`)
- [ ] Files registered in `docs/INDEX.md`
- [ ] Documentation updated

## IDE Setup

### VS Code

Install extensions:
- C/C++ (Microsoft)
- clang-format
- clang-tidy

### CLion

- Configure clang-format path
- Enable clang-tidy inspections
- Set code style to use `.clang-format`

## Continuous Integration

### GitHub Actions Workflow

CI runs automatically on:
- **Release creation**: When a new release is created or published
- **Tag push**: When a version tag (e.g., `v0.0.6`) is pushed

**Note**: CI does not run on every push or pull request. It only runs for releases to reduce CI costs and focus on production-ready code.

### CI Jobs

#### Build and Test Job (macOS)

Runs on `macos-latest` runner:

1. **Install Dependencies**
   - OpenSSL via Homebrew (tries `openssl@3` first, then `openssl`)
   - LLVM (for clang-tidy)

2. **Setup Environment**
   - Configure OpenSSL paths with fallback logic:
     - First tries `openssl@3` (common on newer macOS/Apple Silicon)
     - Falls back to `openssl`
     - Falls back to common paths (`/opt/homebrew/opt/openssl@3`, `/opt/homebrew/opt/openssl`, `/usr/local/opt/openssl`)
   - Configure LLVM paths (`brew --prefix llvm`)
   - Set compiler flags (LDFLAGS, CPPFLAGS)
   - Verify OpenSSL paths exist before building

3. **Format Check**
   - Install clang-format
   - Check code formatting (non-blocking)

4. **Lint Check**
   - Install LLVM tools
   - Run clang-tidy (non-blocking)

5. **Static Analysis**
   - Verify compiler version
   - Basic static analysis checks

6. **Build**
   - Configure CMake build system
   - Compile source code using CMake
   - Verify build succeeds

7. **Build Tests**
   - Tests are built together with main executable
   - CMake automatically handles linking

8. **Run Tests**
   - Execute tests using CTest
   - Verify all tests pass

9. **Check File Sizes**
   - Verify files don't exceed 300 lines (non-blocking)

10. **Run Coverage** (optional)
    - Generate code coverage report
    - Continue on error

#### Documentation Check Job (Ubuntu)

Runs on `ubuntu-latest` runner:

1. **Check File Sizes**
   - Verify documentation files don't exceed 300 lines

2. **Check INDEX.md Registration**
   - Verify `docs/INDEX.md` exists and has content

3. **Verify File Registration**
   - Check that source files are mentioned in `docs/INDEX.md`
   - Warn about unregistered files (non-blocking)

### CI Configuration

**Workflow File**: `.github/workflows/ci.yml`

**Triggers**:
- **Release Events**: Runs when a release is created or published
- **Tag Push**: Runs when a version tag (e.g., `v0.0.6`) is pushed

**Note**: CI does not run on every push or pull request. It only runs for releases to reduce CI costs and focus on production-ready code.

**Key Features**:
- Cross-platform testing (macOS for builds, Ubuntu for docs)
- Non-blocking checks for formatting and linting
- Automatic dependency installation
- Clean builds for reproducibility
- Coverage generation (optional)

### Troubleshooting CI Failures

**Build Failures**:
- **OpenSSL Path Issues**: 
  - CI workflow automatically detects `openssl@3` or `openssl`
  - Check CI logs for "OpenSSL prefix" message
  - Verify OpenSSL lib/include directories exist
  - Common paths: `/opt/homebrew/opt/openssl@3` (Apple Silicon), `/usr/local/opt/openssl` (Intel)
- **Compiler Flags**: Verify LDFLAGS and CPPFLAGS are set correctly
- **Missing Dependencies**: Check that `openssl` or `openssl@3` is installed via Homebrew

**Test Failures**:
- Review test output logs
- Check for environment-specific issues
- Verify test data is correct
- Check for macOS-specific test failures (expected in some cases)

**Documentation Failures**:
- Ensure files are registered in `docs/INDEX.md`
- Check file sizes don't exceed 300 lines
- Verify markdown syntax is correct

**Common CI Issues**:
- **"openssl@3 is already installed"**: This is normal, CI continues with existing installation
- **"OpenSSL lib directory not found"**: Check that OpenSSL is installed and path detection is working
- **Build errors with OpenSSL**: Verify that both `lib` and `include` directories exist at detected path
