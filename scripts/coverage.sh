#!/bin/bash
# Generate code coverage report using llvm-cov

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# Check if llvm-cov is available
LLVM_COV=$(which llvm-cov 2>/dev/null || \
          find /opt/homebrew/opt/llvm/bin -name "llvm-cov" 2>/dev/null | head -1 || \
          find /usr/local/opt/llvm/bin -name "llvm-cov" 2>/dev/null | head -1 || \
          echo "")

if [ -z "$LLVM_COV" ]; then
    echo "Error: llvm-cov not found. Install with: brew install llvm"
    echo "After installation, llvm-cov will be at: /opt/homebrew/opt/llvm/bin/llvm-cov"
    exit 1
fi

LLVM_PROFDATA=$(which llvm-profdata 2>/dev/null || \
               find /opt/homebrew/opt/llvm/bin -name "llvm-profdata" 2>/dev/null | head -1 || \
               find /usr/local/opt/llvm/bin -name "llvm-profdata" 2>/dev/null | head -1 || \
               echo "")

if [ -z "$LLVM_PROFDATA" ]; then
    echo "Error: llvm-profdata not found. Install with: brew install llvm"
    exit 1
fi

echo "Generating coverage report (CMake + Ninja)..."

BUILD_DIR="$PROJECT_ROOT/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -G Ninja -DENABLE_COVERAGE=ON .. >/dev/null

echo "Building tests with coverage flags..."
ninja test_neo_slack_bot

echo "Running tests with coverage instrumentation..."
ctest --output-on-failure

echo "Running coverage target..."
ninja coverage

echo "Coverage report generated in $BUILD_DIR/coverage/"
echo "Open $BUILD_DIR/coverage/index.html in your browser to view"
