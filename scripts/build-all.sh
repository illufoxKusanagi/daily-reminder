#!/bin/bash

echo "======================================"
echo "Building Daily Task Reminder"
echo "======================================"

# Build Frontend
echo ""
echo "1. Building Frontend (Next.js)..."
cd frontend
npm install
npm run build

if [ $? -ne 0 ]; then
    echo "❌ Frontend build failed!"
    exit 1
fi
echo "✅ Frontend build successful!"

# Build Backend
echo ""
echo "2. Building Backend (Qt)..."
cd ../backend
mkdir -p build
cd build
cmake ..
cmake --build .

if [ $? -ne 0 ]; then
    echo "❌ Backend build failed!"
    exit 1
fi
echo "✅ Backend build successful!"

# Copy frontend to backend
echo ""
echo "3. Copying frontend to backend..."
mkdir -p frontend
cp -r ../../frontend/out/* frontend/

echo ""
echo "======================================"
echo "✅ Build Complete!"
echo "======================================"
echo "Executable: backend/build/DailyReminder"
echo ""
echo "To run: cd backend/build && ./DailyReminder"
