#!/bin/bash
# Check that all files are under 300 lines

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

MAX_LINES=300
ERRORS=0

echo "Checking file sizes (max $MAX_LINES lines)..."

# Check source files
find src -type f \( -name "*.cpp" -o -name "*.hpp" \) | while read file; do
    lines=$(wc -l < "$file")
    if [ "$lines" -gt "$MAX_LINES" ]; then
        echo "ERROR: $file exceeds $MAX_LINES lines ($lines lines)"
        ERRORS=$((ERRORS + 1))
    fi
done

# Check test files
find tests -type f \( -name "*.cpp" -o -name "*.hpp" \) | while read file; do
    lines=$(wc -l < "$file")
    if [ "$lines" -gt "$MAX_LINES" ]; then
        echo "ERROR: $file exceeds $MAX_LINES lines ($lines lines)"
        ERRORS=$((ERRORS + 1))
    fi
done

# Check documentation files
find docs -type f -name "*.md" | while read file; do
    lines=$(wc -l < "$file")
    if [ "$lines" -gt "$MAX_LINES" ]; then
        echo "ERROR: $file exceeds $MAX_LINES lines ($lines lines)"
        ERRORS=$((ERRORS + 1))
    fi
done

if [ $ERRORS -gt 0 ]; then
    echo ""
    echo "Found $ERRORS file(s) exceeding $MAX_LINES lines"
    exit 1
else
    echo "All files are within $MAX_LINES lines"
    exit 0
fi
