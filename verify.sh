#!/bin/bash

# Daily Reminder - Integration Verification Script
# Checks that all components are properly configured

echo "🔍 Daily Reminder - Integration Verification"
echo "==========================================="
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Track status
ALL_PASSED=true

# Check 1: Backend binary exists
echo -n "📦 Checking backend binary... "
if [ -f "backend/build/backend" ]; then
    echo -e "${GREEN}✓ Found${NC}"
else
    echo -e "${RED}✗ Not found${NC}"
    echo "   Run: cd backend && cmake -B build -G Ninja && cmake --build build"
    ALL_PASSED=false
fi

# Check 2: Frontend dependencies
echo -n "📦 Checking frontend dependencies... "
if [ -d "node_modules" ] || [ -d "frontend/node_modules" ]; then
    echo -e "${GREEN}✓ Installed${NC}"
else
    echo -e "${RED}✗ Missing${NC}"
    echo "   Run: npm install"
    ALL_PASSED=false
fi

# Check 3: tsx runtime
echo -n "📦 Checking tsx runtime... "
if [ -d "node_modules/tsx" ]; then
    echo -e "${GREEN}✓ Installed${NC}"
else
    echo -e "${RED}✗ Missing${NC}"
    echo "   Run: npm install"
    ALL_PASSED=false
fi

# Check 4: Seed script exists
echo -n "📝 Checking seed script... "
if [ -f "scripts/seed-events.ts" ]; then
    echo -e "${GREEN}✓ Found${NC}"
else
    echo -e "${RED}✗ Not found${NC}"
    ALL_PASSED=false
fi

# Check 5: Calendar module structure
echo -n "📅 Checking calendar module... "
if [ -d "frontend/src/modules/calendar" ]; then
    FILES_COUNT=$(find frontend/src/modules/calendar -name "*.tsx" -o -name "*.ts" | wc -l)
    echo -e "${GREEN}✓ Found ($FILES_COUNT files)${NC}"
else
    echo -e "${RED}✗ Not found${NC}"
    ALL_PASSED=false
fi

# Check 6: Key files exist
echo ""
echo "🔎 Verifying key files:"

FILES=(
    "backend/database.cpp:Database implementation"
    "backend/activitymanager.cpp:Activity manager"
    "backend/httpserver.cpp:HTTP server"
    "frontend/src/modules/calendar/interfaces.ts:IEvent interface"
    "frontend/src/modules/calendar/schemas.ts:Zod schemas"
    "frontend/src/modules/calendar/requests.ts:API client"
    "frontend/src/modules/calendar/calendar.tsx:Calendar component"
)

for entry in "${FILES[@]}"; do
    FILE="${entry%%:*}"
    DESC="${entry#*:}"
    echo -n "   $DESC... "
    if [ -f "$FILE" ]; then
        echo -e "${GREEN}✓${NC}"
    else
        echo -e "${RED}✗${NC}"
        ALL_PASSED=false
    fi
done

# Check 7: Schema compatibility
echo ""
echo "🔎 Verifying schema compatibility:"

echo -n "   IEvent interface fields... "
if grep -q "opdName.*string" frontend/src/modules/calendar/interfaces.ts && \
   grep -q "startDate.*string" frontend/src/modules/calendar/interfaces.ts && \
   grep -q "endDate.*string" frontend/src/modules/calendar/interfaces.ts && \
   grep -q "color.*TEventColor" frontend/src/modules/calendar/interfaces.ts; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    ALL_PASSED=false
fi

echo -n "   Backend events table... "
if grep -q "CREATE TABLE.*events" backend/database.cpp && \
   grep -q "opd_name TEXT" backend/database.cpp && \
   grep -q "start_date TEXT" backend/database.cpp && \
   grep -q "end_date TEXT" backend/database.cpp; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    ALL_PASSED=false
fi

echo -n "   API endpoint /api/event... "
if grep -q '"/api/event"' backend/httpserver.cpp; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    ALL_PASSED=false
fi

echo -n "   UUID support in ActivityManager... "
if grep -q "QUuid::createUuid" backend/activitymanager.cpp; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    ALL_PASSED=false
fi

# Check 8: Package.json scripts
echo ""
echo "🔎 Verifying npm scripts:"

SCRIPTS=("dev" "build" "run" "clean" "seed")
for script in "${SCRIPTS[@]}"; do
    echo -n "   npm run $script... "
    if grep -q "\"$script\":" package.json; then
        echo -e "${GREEN}✓${NC}"
    else
        echo -e "${RED}✗${NC}"
        ALL_PASSED=false
    fi
done

# Summary
echo ""
echo "==========================================="
if [ "$ALL_PASSED" = true ]; then
    echo -e "${GREEN}✅ All checks passed!${NC}"
    echo ""
    echo "🚀 Ready to run:"
    echo "   1. Start application: npm run dev"
    echo "   2. Seed sample data: npm run seed"
    echo "   3. Open calendar in Qt window"
    exit 0
else
    echo -e "${RED}❌ Some checks failed${NC}"
    echo ""
    echo "Please fix the issues above before running the application."
    exit 1
fi
