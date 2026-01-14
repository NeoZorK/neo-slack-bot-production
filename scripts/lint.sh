#!/bin/bash
# Lint C++ source files using clang-tidy

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# Check if clang-tidy is installed
if ! command -v clang-tidy &> /dev/null; then
    echo "Error: clang-tidy not found. Install with: brew install llvm"
    exit 1
fi

echo "Linting C++ files..."

# Create compile_commands.json if it doesn't exist
if [ ! -f compile_commands.json ]; then
    echo "Warning: compile_commands.json not found. Creating basic one..."
    # This is a placeholder - in real project, generate with bear or similar
    echo '[]' > compile_commands.json
fi

# Lint source files
find src -name "*.cpp" -o -name "*.hpp" | while read file; do
    echo "Linting $file"
    clang-tidy "$file" -p . || true
done

# Lint test files
find tests -name "*.cpp" -o -name "*.hpp" | while read file; do
    echo "Linting $file"
    clang-tidy "$file" -p . || true
done

echo "Linting complete!"
