#!/bin/bash

# Direct SQL test for alarm matching

DB_PATH="/home/illufoxkasunagi/.local/share/DailyReminder/Daily Activity Reminder/activities.db"

echo "🔍 Testing Alarm SQL Query"
echo "=========================="
echo ""

echo "1️⃣  Current time (SQLite's perspective):"
sqlite3 "$DB_PATH" "SELECT datetime('now') as sqlite_now;"
echo ""

echo "2️⃣  Current time (system UTC):"
date -u +"%Y-%m-%dT%H:%M:%S"
echo ""

echo "3️⃣  All events with reminders enabled:"
sqlite3 "$DB_PATH" "SELECT id, title, reminder_time, is_reminder_enabled FROM events WHERE is_reminder_enabled = 1;" | head -5
echo ""

echo "4️⃣  Testing the exact SQL query used by AlarmManager:"
echo "     SELECT id, title, category, reminder_time"
echo "     WHERE is_reminder_enabled = 1"
echo "     AND datetime(reminder_time) <= datetime('now')"
echo "     AND datetime(reminder_time) > datetime('now', '-60 seconds')"
echo ""
sqlite3 "$DB_PATH" "
SELECT id, title, category, reminder_time, 
       datetime(reminder_time) as parsed_time,
       datetime('now') as current_time,
       CASE 
         WHEN datetime(reminder_time) <= datetime('now') THEN 'SHOULD TRIGGER'
         ELSE 'Future'
       END as status
FROM events
WHERE is_reminder_enabled = 1
AND reminder_time IS NOT NULL
AND datetime(reminder_time) <= datetime('now')
AND datetime(reminder_time) > datetime('now', '-60 seconds');"
echo ""

echo "5️⃣  Reminders that are in the past (should have triggered):"
sqlite3 "$DB_PATH" "
SELECT id, title, reminder_time,
       datetime(reminder_time) as parsed_time,
       datetime('now') as current_time
FROM events
WHERE is_reminder_enabled = 1
AND reminder_time IS NOT NULL
AND datetime(reminder_time) <= datetime('now')
LIMIT 5;"
echo ""

echo "6️⃣  Creating a test event with reminder in 1 minute..."
REMINDER_TIME=$(date -u -d "+1 minute" +"%Y-%m-%dT%H:%M:%S")
EVENT_START=$(date -u -d "+10 minutes" +"%Y-%m-%dT%H:%M:%S")
EVENT_END=$(date -u -d "+30 minutes" +"%Y-%m-%dT%H:%M:%S")

echo "   Reminder time: $REMINDER_TIME"
echo ""

curl -s -X POST http://localhost:8080/api/event \
  -H "Content-Type: application/json" \
  -d "{
    \"category\": \"Test\",
    \"title\": \"SQL DEBUG TEST\",
    \"description\": \"Testing SQL query matching\",
    \"startDate\": \"$EVENT_START\",
    \"endDate\": \"$EVENT_END\",
    \"color\": \"red\",
    \"isReminderEnabled\": true,
    \"reminderTime\": \"$REMINDER_TIME\"
  }" | jq '.'

echo ""
echo "7️⃣  Verifying the test event was created:"
sqlite3 "$DB_PATH" "SELECT id, title, reminder_time, is_reminder_enabled FROM events WHERE title = 'SQL DEBUG TEST';"
echo ""

echo "8️⃣  Waiting 70 seconds for alarm to trigger..."
echo "    (Watch the Qt backend terminal!)"
echo "    Press Ctrl+C to cancel"
echo ""

for i in {70..1}; do
    echo -ne "   Countdown: $i seconds remaining...\r"
    sleep 1
done

echo ""
echo ""
echo "9️⃣  Checking if alarm triggered (reminder should be disabled now):"
sqlite3 "$DB_PATH" "SELECT id, title, is_reminder_enabled FROM events WHERE title = 'SQL DEBUG TEST';"
echo ""

IS_DISABLED=$(sqlite3 "$DB_PATH" "SELECT is_reminder_enabled FROM events WHERE title = 'SQL DEBUG TEST' LIMIT 1;")
if [ "$IS_DISABLED" = "0" ]; then
    echo "✅ SUCCESS! Alarm triggered (reminder was disabled)"
else
    echo "❌ FAILED! Alarm did NOT trigger (reminder still enabled = $IS_DISABLED)"
    echo ""
    echo "Debugging info:"
    echo "  - Check if backend is running: ps aux | grep './backend'"
    echo "  - Check backend logs for errors"
    echo "  - Verify timer is actually running (should see check messages)"
fi
