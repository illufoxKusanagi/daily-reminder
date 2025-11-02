#!/bin/bash
# Build using Qt/CMake native installation

set -e

echo "🔨 Building Daily Reminder with Qt/CMake..."

PROJECT_ROOT=$(pwd)
BUILD_DIR="$PROJECT_ROOT/backend/build"
INSTALL_PREFIX="/usr/local"

# 1. Build frontend
echo "📦 Building frontend..."
cd frontend
npm install
npm run build
cd ..

# 2. Configure and build backend with CMake
echo "🔨 Configuring backend with CMake..."
cd backend
mkdir -p build && cd build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      ..

echo "🔨 Building backend..."
cmake --build . -j$(nproc)

echo ""
echo "========================================="
echo "✅ Build completed successfully!"
echo "========================================="
echo ""
echo "Binary location: $BUILD_DIR/backend"
echo "Frontend built at: $PROJECT_ROOT/frontend/out/"
echo ""
echo "To run locally:"
echo "  cd backend/build && ./backend"
echo ""
echo "To install system-wide (requires root):"
echo "  sudo cmake --install $BUILD_DIR"
echo "  sudo mkdir -p $INSTALL_PREFIX/share/daily-reminder"
echo "  sudo cp -r $PROJECT_ROOT/frontend/out/* $INSTALL_PREFIX/share/daily-reminder/"
echo ""
