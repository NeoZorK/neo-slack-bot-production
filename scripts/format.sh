#!/bin/bash
# Format all C++ source files using clang-format

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found. Install with: brew install clang-format"
    exit 1
fi

echo "Formatting C++ files..."

# Format source files
find src -name "*.cpp" -o -name "*.hpp" | while read file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

# Format test files
find tests -name "*.cpp" -o -name "*.hpp" | while read file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "Formatting complete!"
