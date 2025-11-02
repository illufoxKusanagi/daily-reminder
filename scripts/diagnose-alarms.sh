#!/bin/bash

# Test script to verify alarm system is working

echo "🔍 Daily Reminder - Alarm System Diagnostic"
echo "============================================"
echo ""

# 1. Check if backend is running
echo "1️⃣  Checking if Qt backend is running..."
if ps aux | grep -E "./backend$" | grep -v grep > /dev/null; then
    BACKEND_PID=$(ps aux | grep -E "./backend$" | grep -v grep | awk '{print $2}')
    echo "✅ Backend is running (PID: $BACKEND_PID)"
else
    echo "❌ Backend is NOT running!"
    echo "   Start it with: cd backend/build && ./backend"
    exit 1
fi
echo ""

# 2. Check if backend API is responding
echo "2️⃣  Checking if backend API is responding..."
if curl -s http://localhost:8080/status > /dev/null 2>&1; then
    echo "✅ Backend API is responding"
else
    echo "❌ Backend API is not responding!"
    exit 1
fi
echo ""

# 3. Check database location and content
echo "3️⃣  Checking database..."
DB_PATH="/home/illufoxkasunagi/.local/share/DailyReminder/Daily Activity Reminder/activities.db"
if [ -f "$DB_PATH" ]; then
    echo "✅ Database found: $DB_PATH"
    
    # Count events with reminders
    REMINDER_COUNT=$(sqlite3 "$DB_PATH" "SELECT COUNT(*) FROM events WHERE is_reminder_enabled = 1;")
    echo "   Events with reminders enabled: $REMINDER_COUNT"
else
    echo "❌ Database not found!"
    exit 1
fi
echo ""

# 4. Check notify-send
echo "4️⃣  Testing system notifications..."
if command -v notify-send &> /dev/null; then
    echo "✅ notify-send is available"
    echo "   Sending test notification..."
    notify-send -u critical -i appointment-soon "Alarm Test" "If you see this, notifications work!"
    echo "   👀 Did you see a notification popup?"
else
    echo "❌ notify-send is not installed!"
    echo "   Install with: sudo dnf install libnotify"
fi
echo ""

# 5. Check current time vs reminder times
echo "5️⃣  Checking upcoming reminders..."
CURRENT_TIME=$(date -u +"%Y-%m-%dT%H:%M:%S")
echo "   Current UTC time: $CURRENT_TIME"
echo ""
echo "   Next 3 reminders:"
sqlite3 "$DB_PATH" "SELECT '   - ' || title || ' at ' || reminder_time FROM events WHERE is_reminder_enabled = 1 AND reminder_time > datetime('now') ORDER BY reminder_time LIMIT 3;"
echo ""
echo "   Past reminders (should have triggered already):"
PAST_COUNT=$(sqlite3 "$DB_PATH" "SELECT COUNT(*) FROM events WHERE is_reminder_enabled = 1 AND reminder_time <= datetime('now');")
if [ "$PAST_COUNT" -gt 0 ]; then
    sqlite3 "$DB_PATH" "SELECT '   ⚠️  ' || title || ' was at ' || reminder_time FROM events WHERE is_reminder_enabled = 1 AND reminder_time <= datetime('now') LIMIT 5;"
    echo ""
    echo "   ⚠️  Found $PAST_COUNT past reminders that should have triggered!"
    echo "      These won't trigger anymore (already in the past)"
else
    echo "   ✅ No past reminders found"
fi
echo ""

# 6. Create a test alarm for 1 minute from now
echo "6️⃣  Creating test alarm (will trigger in 1 minute)..."
REMINDER_TIME=$(date -u -d "+1 minute" +"%Y-%m-%dT%H:%M:%S")
EVENT_START=$(date -u -d "+10 minutes" +"%Y-%m-%dT%H:%M:%S")
EVENT_END=$(date -u -d "+30 minutes" +"%Y-%m-%dT%H:%M:%S")

RESPONSE=$(curl -s -X POST http://localhost:8080/api/event \
  -H "Content-Type: application/json" \
  -d "{
    \"category\": \"Test\",
    \"title\": \"🔔 DIAGNOSTIC TEST ALARM\",
    \"description\": \"This alarm should ring in 1 minute!\",
    \"startDate\": \"$EVENT_START\",
    \"endDate\": \"$EVENT_END\",
    \"color\": \"red\",
    \"isReminderEnabled\": true,
    \"reminderTime\": \"$REMINDER_TIME\"
  }")

if echo "$RESPONSE" | grep -q "error"; then
    echo "❌ Failed to create test event!"
    echo "   Response: $RESPONSE"
else
    echo "✅ Test event created!"
    echo "   Reminder will trigger at: $REMINDER_TIME (in ~1 minute)"
    EVENT_ID=$(echo "$RESPONSE" | jq -r '.id' 2>/dev/null)
    echo "   Event ID: $EVENT_ID"
fi
echo ""

# 7. Instructions
echo "============================================"
echo "📋 What to do now:"
echo "============================================"
echo ""
echo "1. Watch the Qt backend terminal (where you ran ./backend)"
echo "2. In ~1 minute, you should see:"
echo "   🔔 ALARM NOTIFICATION"
echo "   🔔 Title: 🔔 DIAGNOSTIC TEST ALARM"
echo ""
echo "3. You should also see a desktop notification popup"
echo ""
echo "4. If you DON'T see anything after 2 minutes:"
echo "   - Check if backend is still running"
echo "   - Look for any errors in the backend terminal"
echo "   - The alarm might not be checking properly"
echo ""
echo "⏰ Waiting for 90 seconds to see if alarm triggers..."
echo "   (Press Ctrl+C to skip)"
echo ""

# Wait and then check if it triggered
sleep 90

echo ""
echo "🔍 Checking if test alarm triggered..."
IS_DISABLED=$(sqlite3 "$DB_PATH" "SELECT is_reminder_enabled FROM events WHERE id = '$EVENT_ID';" 2>/dev/null)
if [ "$IS_DISABLED" = "0" ]; then
    echo "✅ SUCCESS! Alarm was triggered (reminder was disabled)"
elif [ "$IS_DISABLED" = "1" ]; then
    echo "❌ FAILED! Alarm did NOT trigger (reminder still enabled)"
    echo ""
    echo "Possible issues:"
    echo "   1. AlarmManager might not be running in the backend"
    echo "   2. Timer interval might be too long"
    echo "   3. SQL query might not be matching datetime format"
else
    echo "⚠️  Could not verify (event might have been deleted)"
fi
echo ""
echo "Diagnostic complete!"
