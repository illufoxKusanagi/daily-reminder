#!/bin/bash

echo "======================================"
echo "Daily Task Reminder - Development Mode"
echo "======================================"

# Start Frontend Dev Server
echo ""
echo "Starting Next.js dev server..."
cd frontend
npm run dev &
FRONTEND_PID=$!

# Wait for frontend to start
echo "Waiting for frontend to start..."
sleep 5

# Build and run backend
echo ""
echo "Building and starting Qt backend..."
cd ../backend
mkdir -p build
cd build
cmake .. > /dev/null 2>&1
cmake --build . > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✅ Starting application..."
    ./DailyReminder
else
    echo "❌ Backend build failed!"
fi

# Cleanup
echo ""
echo "Stopping frontend dev server..."
kill $FRONTEND_PID
