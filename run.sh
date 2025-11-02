#!/bin/bash

# Daily Reminder - Run Script
# Starts the application

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

EXECUTABLE="backend/build/DailyReminder"

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}  Starting Daily Task Reminder${NC}"
echo -e "${BLUE}================================================${NC}"

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}Error: Application not built!${NC}"
    echo -e "${YELLOW}Please run: ./build.sh${NC}"
    exit 1
fi

# Check if executable has permissions
if [ ! -x "$EXECUTABLE" ]; then
    echo -e "${YELLOW}Setting executable permissions...${NC}"
    chmod +x "$EXECUTABLE"
fi

# Run the application
echo -e "${GREEN}Launching application...${NC}\n"
cd backend/build
./DailyReminder
