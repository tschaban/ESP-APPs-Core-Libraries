#!/bin/bash

# ESP-APPs Code Formatting Script
# Formats all C/C++ source files using clang-format

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}Error: clang-format is not installed${NC}"
    echo "Install it with:"
    echo "  Ubuntu/Debian: sudo apt-get install clang-format"
    echo "  macOS: brew install clang-format"
    echo "  Windows: Download from LLVM releases"
    exit 1
fi

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo -e "${YELLOW}ESP-APPs Code Formatter${NC}"
echo "Project root: $PROJECT_ROOT"

# Check if .clang-format exists
if [ ! -f "$PROJECT_ROOT/.clang-format" ]; then
    echo -e "${RED}Error: .clang-format file not found in project root${NC}"
    exit 1
fi

# Find all C/C++ files
FILES=$(find "$PROJECT_ROOT/src" -name "*.cpp" -o -name "*.h" -o -name "*.ino" 2>/dev/null)

if [ -z "$FILES" ]; then
    echo -e "${YELLOW}No C/C++ files found to format${NC}"
    exit 0
fi

echo "Found files to format:"
echo "$FILES" | sed 's/^/  /'

# Check mode vs format mode
if [ "$1" = "--check" ]; then
    echo -e "\n${YELLOW}Checking formatting...${NC}"
    
    ISSUES=0
    for file in $FILES; do
        if ! clang-format --dry-run --Werror --style=file "$file" >/dev/null 2>&1; then
            echo -e "${RED}✗${NC} $file"
            ISSUES=$((ISSUES + 1))
        else
            echo -e "${GREEN}✓${NC} $file"
        fi
    done
    
    if [ $ISSUES -gt 0 ]; then
        echo -e "\n${RED}Found $ISSUES formatting issues${NC}"
        echo "Run './scripts/format-code.sh' to fix them"
        exit 1
    else
        echo -e "\n${GREEN}All files are properly formatted${NC}"
    fi
else
    echo -e "\n${YELLOW}Formatting files...${NC}"
    
    for file in $FILES; do
        echo "Formatting: $file"
        clang-format -i --style=file "$file"
    done
    
    echo -e "\n${GREEN}Formatting complete!${NC}"
    
    # Check if there are any changes
    if command -v git &> /dev/null && git rev-parse --git-dir > /dev/null 2>&1; then
        if [ -n "$(git status --porcelain)" ]; then
            echo -e "${YELLOW}Files were modified. Review changes with 'git diff'${NC}"
        else
            echo -e "${GREEN}No changes needed - code was already formatted${NC}"
        fi
    fi
fi