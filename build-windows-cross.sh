#!/bin/bash
# Cross-compile Daily Reminder for Windows from Linux
# Requires: mingw-w64 toolchain and Qt for Windows

set -e

echo "🔨 Cross-compiling Daily Reminder for Windows (from Linux)..."

PROJECT_ROOT=$(pwd)
OUTPUT_DIR="$PROJECT_ROOT/output/windows"
QT_WIN_PATH="${QT_WIN_PATH:-$HOME/Qt/6.9.0/mingw_64}"

# Check if mingw-w64 is installed
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "❌ mingw-w64 not found!"
    echo ""
    echo "Install it with:"
    echo "  Fedora/RHEL: sudo dnf install mingw64-gcc-c++ mingw64-gcc mingw64-binutils"
    echo "  Ubuntu/Debian: sudo apt install mingw-w64 g++-mingw-w64-x86-64"
    echo "  Arch: sudo pacman -S mingw-w64-gcc"
    exit 1
fi

# Check if Qt for Windows is available
if [ ! -d "$QT_WIN_PATH" ]; then
    echo "❌ Qt for Windows not found at: $QT_WIN_PATH"
    echo ""
    echo "You need to install Qt for Windows (MinGW):"
    echo "  1. Download Qt Online Installer from https://www.qt.io/download"
    echo "  2. Select: Qt 6.x → MinGW 64-bit"
    echo "  3. Set QT_WIN_PATH environment variable or install to $HOME/Qt/"
    echo ""
    echo "Alternatively, set the path:"
    echo "  export QT_WIN_PATH=/path/to/Qt/6.x.x/mingw_64"
    echo "  ./build-windows-cross.sh"
    exit 1
fi

echo "✅ Found mingw-w64 toolchain"
echo "✅ Found Qt for Windows at: $QT_WIN_PATH"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# 1. Build frontend (same for all platforms)
echo ""
echo "📦 [1/4] Building frontend..."
cd frontend
npm install
npm run build
cd ..

# 2. Cross-compile backend
echo ""
echo "🔨 [2/4] Cross-compiling backend..."
cd backend
rm -rf build-windows
mkdir -p build-windows && cd build-windows

# Create toolchain file
cat > mingw-toolchain.cmake <<EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32 $QT_WIN_PATH)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_PREFIX_PATH "$QT_WIN_PATH")
EOF

cmake -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$QT_WIN_PATH" \
      ..

cmake --build . -j$(nproc)

cd ../..

# 3. Bundle everything
echo ""
echo "📦 [3/4] Bundling Windows package..."
mkdir -p "$OUTPUT_DIR/DailyReminder"

# Copy backend executable
cp backend/build-windows/backend.exe "$OUTPUT_DIR/DailyReminder/"

# Copy frontend
cp -r frontend/out "$OUTPUT_DIR/DailyReminder/frontend"

# 4. Copy Qt DLLs (manual since windeployqt doesn't work on Linux)
echo ""
echo "📦 [4/4] Copying Qt dependencies..."

QT_DLLS=(
    "Qt6Core.dll"
    "Qt6Gui.dll"
    "Qt6Widgets.dll"
    "Qt6Network.dll"
    "Qt6Sql.dll"
    "Qt6WebEngineCore.dll"
    "Qt6WebEngineWidgets.dll"
    "Qt6WebChannel.dll"
    "Qt6HttpServer.dll"
    "Qt6Qml.dll"
    "Qt6QmlModels.dll"
    "Qt6Quick.dll"
    "Qt6OpenGL.dll"
    "Qt6Positioning.dll"
    "Qt6PrintSupport.dll"
)

MINGW_DLLS=(
    "libgcc_s_seh-1.dll"
    "libstdc++-6.dll"
    "libwinpthread-1.dll"
)

# Copy Qt DLLs
for dll in "${QT_DLLS[@]}"; do
    if [ -f "$QT_WIN_PATH/bin/$dll" ]; then
        cp "$QT_WIN_PATH/bin/$dll" "$OUTPUT_DIR/DailyReminder/"
    else
        echo "⚠️  Warning: $dll not found"
    fi
done

# Copy MinGW runtime DLLs
MINGW_BIN="/usr/x86_64-w64-mingw32/bin"
if [ -d "$MINGW_BIN" ]; then
    for dll in "${MINGW_DLLS[@]}"; do
        if [ -f "$MINGW_BIN/$dll" ]; then
            cp "$MINGW_BIN/$dll" "$OUTPUT_DIR/DailyReminder/"
        fi
    done
fi

# Copy Qt plugins
mkdir -p "$OUTPUT_DIR/DailyReminder/platforms"
mkdir -p "$OUTPUT_DIR/DailyReminder/imageformats"
mkdir -p "$OUTPUT_DIR/DailyReminder/sqldrivers"

if [ -d "$QT_WIN_PATH/plugins/platforms" ]; then
    cp "$QT_WIN_PATH/plugins/platforms/qwindows.dll" "$OUTPUT_DIR/DailyReminder/platforms/" 2>/dev/null || true
fi

if [ -d "$QT_WIN_PATH/plugins/sqldrivers" ]; then
    cp "$QT_WIN_PATH/plugins/sqldrivers/qsqlite.dll" "$OUTPUT_DIR/DailyReminder/sqldrivers/" 2>/dev/null || true
fi

# Create qt.conf
cat > "$OUTPUT_DIR/DailyReminder/qt.conf" <<EOF
[Paths]
Plugins = .
EOF

# Create run script
cat > "$OUTPUT_DIR/DailyReminder/DailyReminder.bat" <<EOF
@echo off
start "" "%~dp0backend.exe"
EOF

echo ""
echo "========================================="
echo "✅ Windows build completed!"
echo "========================================="
echo ""
echo "Output: $OUTPUT_DIR/DailyReminder/"
echo "Executable: $OUTPUT_DIR/DailyReminder/backend.exe"
echo ""
echo "To test on Windows:"
echo "  1. Copy output/windows/DailyReminder/ to Windows PC"
echo "  2. Double-click DailyReminder.bat or backend.exe"
echo ""
echo "⚠️  Note: This is a basic deployment. Some Qt modules might be missing."
echo "For complete deployment, run windeployqt.exe on Windows after copying."
echo ""
