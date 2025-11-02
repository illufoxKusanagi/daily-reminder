#!/bin/bash

# Daily Reminder - Build Script
# Usage: ./build.sh [debug|release]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default build type
BUILD_TYPE="${1:-release}"

# Validate build type
if [[ "$BUILD_TYPE" != "debug" && "$BUILD_TYPE" != "release" ]]; then
    echo -e "${RED}Error: Build type must be 'debug' or 'release'${NC}"
    echo "Usage: ./build.sh [debug|release]"
    exit 1
fi

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}  Daily Task Reminder - Build Script${NC}"
echo -e "${BLUE}  Build Type: ${GREEN}${BUILD_TYPE}${NC}"
echo -e "${BLUE}================================================${NC}"

# Step 1: Install dependencies
echo -e "\n${YELLOW}[1/5]${NC} Installing dependencies..."
if [ ! -d "node_modules" ]; then
    npm install
else
    echo -e "${GREEN}✓${NC} Dependencies already installed"
fi

# Step 2: Build Frontend
echo -e "\n${YELLOW}[2/5]${NC} Building Next.js frontend..."
cd frontend
npm run build
cd ..
echo -e "${GREEN}✓${NC} Frontend build complete"

# Step 3: Build Qt Backend
echo -e "\n${YELLOW}[3/5]${NC} Building Qt backend (${BUILD_TYPE})..."
cd backend
mkdir -p build
cd build

if [ "$BUILD_TYPE" = "debug" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug ..
else
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi

cmake --build . --config $(echo "$BUILD_TYPE" | sed 's/.*/\u&/')
cd ../..
echo -e "${GREEN}✓${NC} Backend build complete"

# Step 4: Copy frontend to backend
echo -e "\n${YELLOW}[4/5]${NC} Copying frontend to backend..."
mkdir -p backend/build/frontend
cp -r frontend/out/* backend/build/frontend/
echo -e "${GREEN}✓${NC} Frontend copied"

# Step 5: Set executable permissions
echo -e "\n${YELLOW}[5/5]${NC} Setting permissions..."
chmod +x backend/build/DailyReminder
echo -e "${GREEN}✓${NC} Permissions set"

# Summary
echo -e "\n${BLUE}================================================${NC}"
echo -e "${GREEN}✓ Build Complete!${NC}"
echo -e "${BLUE}================================================${NC}"
echo -e "Build type: ${GREEN}${BUILD_TYPE}${NC}"
echo -e "Executable: ${GREEN}backend/build/DailyReminder${NC}"
echo -e "Data will be stored in: ${YELLOW}~/.local/share/DailyReminder/${NC}"
echo -e "\n${YELLOW}To run the application:${NC}"
echo -e "  ./run.sh"
echo -e "\n${YELLOW}To package for distribution:${NC}"
echo -e "  ./package.sh linux"
echo -e "${BLUE}================================================${NC}"
