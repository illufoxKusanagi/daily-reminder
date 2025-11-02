#!/bin/bash
# Build Windows executable using Wine (simpler but requires Wine)

set -e

echo "🍷 Building Daily Reminder for Windows using Wine..."

PROJECT_ROOT=$(pwd)
OUTPUT_DIR="$PROJECT_ROOT/output/windows"

# Check if Wine is installed
if ! command -v wine &> /dev/null; then
    echo "❌ Wine not found!"
    echo ""
    echo "Install Wine:"
    echo "  Fedora/RHEL: sudo dnf install wine"
    echo "  Ubuntu/Debian: sudo apt install wine64"
    echo "  Arch: sudo pacman -S wine"
    echo ""
    echo "❌ ALTERNATIVE: Use the cross-compile script instead:"
    echo "  ./build-windows-cross.sh"
    exit 1
fi

echo "⚠️  WARNING: This approach is experimental and may not work reliably."
echo "⚠️  Recommended: Build on actual Windows or use build-windows-cross.sh"
echo ""
read -p "Continue anyway? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

echo "📦 Building frontend..."
cd frontend
npm install
npm run build
cd ..

echo ""
echo "This would require Qt for Windows installed in Wine prefix..."
echo "❌ This method is too complex. Please use one of these instead:"
echo ""
echo "OPTION 1 (Best): Build on Windows natively"
echo "  build-windows.bat"
echo ""
echo "OPTION 2: Cross-compile from Linux (requires Qt for Windows + MinGW)"
echo "  ./build-windows-cross.sh"
echo ""
echo "OPTION 3: Use GitHub Actions to build Windows release"
echo "  (We can set this up if you want automated builds)"
echo ""
