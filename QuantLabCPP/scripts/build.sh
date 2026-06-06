#!/bin/bash
# ============================================================================
# QuantLabCPP Build Script
# ============================================================================
# This script automates the build process for QuantLabCPP
# Usage: ./scripts/build.sh [debug|release]
# ============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default build type
BUILD_TYPE=${1:-Release}

echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN}   QuantLabCPP Build Script${NC}"
echo -e "${GREEN}============================================${NC}"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

echo -e "\n${YELLOW}Project directory:${NC} $PROJECT_DIR"
echo -e "${YELLOW}Build directory:${NC} $BUILD_DIR"
echo -e "${YELLOW}Build type:${NC} $BUILD_TYPE"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo -e "\n${YELLOW}Configuring with CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build
echo -e "\n${YELLOW}Building project...${NC}"
cmake --build . -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo -e "\n${GREEN}============================================${NC}"
    echo -e "${GREEN}   Build Successful!${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo -e "\nExecutable location: ${YELLOW}$BUILD_DIR/quantlab${NC}"
    echo -e "\nTo run the application:"
    echo -e "  ${GREEN}cd $BUILD_DIR && ./quantlab --demo${NC}"
else
    echo -e "\n${RED}============================================${NC}"
    echo -e "${RED}   Build Failed!${NC}"
    echo -e "${RED}============================================${NC}"
    exit 1
fi
