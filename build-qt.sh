#!/bin/bash
# Build using Qt/CMake native installation

set -e

echo "🔨 Building Daily Reminder with Qt/CMake..."

PROJECT_ROOT=$(pwd)
BUILD_DIR="$PROJECT_ROOT/backend/build"
OUTPUT_DIR="$PROJECT_ROOT/output"
INSTALL_PREFIX="/usr/local"

# Create output directory
mkdir -p "$OUTPUT_DIR"

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

# Copy binary to output directory
echo "📦 Copying binary to output directory..."
cp backend "$OUTPUT_DIR/daily-reminder"

# Copy frontend to output directory (next to binary)
echo "📦 Copying frontend files..."
mkdir -p "$OUTPUT_DIR/frontend"
cp -r "$PROJECT_ROOT/frontend/out/"* "$OUTPUT_DIR/frontend/"

echo ""
echo "========================================="
echo "✅ Build completed successfully!"
echo "========================================="
echo ""
echo "Binary location: $OUTPUT_DIR/daily-reminder"
echo "Frontend files: $OUTPUT_DIR/frontend/"
echo ""
echo "To run locally:"
echo "  ./output/daily-reminder"
echo ""
echo "To install system-wide (requires root):"
echo "  sudo cmake --install $BUILD_DIR"
echo "  sudo mkdir -p $INSTALL_PREFIX/share/daily-reminder"
echo "  sudo cp -r $PROJECT_ROOT/frontend/out/* $INSTALL_PREFIX/share/daily-reminder/"
echo ""
