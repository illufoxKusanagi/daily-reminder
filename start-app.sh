#!/bin/bash

# Daily Reminder - Start Both Backend and Frontend
# This script runs both the Qt backend (for alarms) and Next.js frontend (for UI)

echo "🚀 Starting Daily Reminder Application"
echo "========================================"
echo ""

# Check if Qt backend is built
if [ ! -f "backend/build/backend" ]; then
    echo "❌ Qt backend not built!"
    echo "   Please build it first:"
    echo "   cd backend/build && cmake --build ."
    exit 1
fi

# Check if frontend dependencies are installed
if [ ! -d "frontend/node_modules" ]; then
    echo "❌ Frontend dependencies not installed!"
    echo "   Please install them first:"
    echo "   cd frontend && npm install"
    exit 1
fi

echo "✅ All prerequisites met"
echo ""

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "🛑 Stopping Daily Reminder..."
    jobs -p | xargs -r kill 2>/dev/null
    exit 0
}

trap cleanup SIGINT SIGTERM

# Start Qt backend in background
echo "1️⃣  Starting Qt Backend (Alarm Engine)..."
echo "   📍 Location: http://localhost:8080"
echo "   🔔 AlarmManager will check for reminders every 30 seconds"
echo ""
cd backend/build
./backend &
BACKEND_PID=$!
cd ../..

# Wait for backend to start
sleep 2

# Check if backend started successfully
if ! curl -s http://localhost:8080/status > /dev/null 2>&1; then
    echo "❌ Qt backend failed to start!"
    echo "   Check backend/build/backend for errors"
    kill $BACKEND_PID 2>/dev/null
    exit 1
fi

echo "✅ Qt Backend is running (PID: $BACKEND_PID)"
echo ""

# Start Next.js frontend
echo "2️⃣  Starting Next.js Frontend (UI)..."
echo "   📍 Location: http://localhost:3000"
echo ""
cd frontend
npm run dev &
FRONTEND_PID=$!
cd ..

echo ""
echo "=========================================="
echo "🎉 Daily Reminder is now running!"
echo "=========================================="
echo ""
echo "📱 Open in browser: http://localhost:3000"
echo "🔔 Alarm notifications will appear when reminders trigger"
echo "📊 Backend API: http://localhost:8080"
echo ""
echo "⚠️  Keep this terminal open!"
echo "    Press Ctrl+C to stop both services"
echo ""
echo "📝 Logs:"
echo "   - Alarm notifications appear in this terminal"
echo "   - Look for: 🔔 ALARM NOTIFICATION"
echo ""

# Wait for both processes
wait
