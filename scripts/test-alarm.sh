#!/bin/bash

# Test script for alarm functionality

API_BASE="http://localhost:8080"

echo "🧪 Testing Alarm Functionality"
echo "================================"
echo ""

# Create an event with a reminder 2 minutes from now
REMINDER_TIME=$(date -u -d "+2 minutes" +"%Y-%m-%dT%H:%M:%S")
START_TIME=$(date -u -d "+3 minutes" +"%Y-%m-%dT%H:%M:%S")
END_TIME=$(date -u -d "+4 minutes" +"%Y-%m-%dT%H:%M:%S")

echo "📅 Creating test event with reminder..."
echo "   Reminder time: $REMINDER_TIME (2 minutes from now)"
echo "   Event start: $START_TIME (3 minutes from now)"
echo ""

RESPONSE=$(curl -s -X POST $API_BASE/api/event \
  -H "Content-Type: application/json" \
  -d '{
    "category": "Test",
    "title": "Alarm Test Event",
    "description": "Testing alarm notifications",
    "startDate": "'"$START_TIME"'",
    "endDate": "'"$END_TIME"'",
    "color": "blue",
    "reminderTime": "'"$REMINDER_TIME"'",
    "isReminderEnabled": true
  }')

echo "📝 Response:"
echo "$RESPONSE" | jq '.'
echo ""

EVENT_ID=$(echo "$RESPONSE" | jq -r '.id')

if [ "$EVENT_ID" != "null" ] && [ -n "$EVENT_ID" ]; then
    echo "✅ Event created with ID: $EVENT_ID"
    echo ""
    echo "⏰ The alarm should trigger in approximately 2 minutes."
    echo "   Watch the Qt backend console for the alarm notification!"
    echo ""
    echo "🔍 To check all events with reminders:"
    echo "   curl $API_BASE/api/event | jq '.[] | select(.isReminderEnabled == true)'"
else
    echo "❌ Failed to create event"
fi

echo ""
echo "💡 Tip: Check the backend logs for:"
echo "   - '⏰ Reloaded alarms after creating event with reminder'"
echo "   - '📋 Loaded X active alarm(s)'"
echo "   - '🔔 ALARM TRIGGERED!' (when the alarm fires)"
