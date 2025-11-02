#!/bin/bash

# Daily Reminder - Package Script
# Creates distributable packages for Linux and Windows
# Usage: ./package.sh [linux|windows|all]

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PLATFORM="${1:-linux}"

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}  Daily Task Reminder - Package Script${NC}"
echo -e "${BLUE}  Platform: ${GREEN}${PLATFORM}${NC}"
echo -e "${BLUE}================================================${NC}"

# Check if build exists
if [ ! -f "backend/build/DailyReminder" ]; then
    echo -e "${RED}Error: Application not built!${NC}"
    echo -e "${YELLOW}Please run: ./build.sh release${NC}"
    exit 1
fi

cd backend/build

package_linux() {
    echo -e "\n${YELLOW}[Linux Packaging]${NC}"
    
    # Check if cpack is available
    if ! command -v cpack &> /dev/null; then
        echo -e "${RED}Error: cpack not found. Install cmake.${NC}"
        return 1
    fi
    
    # Create RPM package (Fedora-based)
    echo -e "${YELLOW}Creating RPM package...${NC}"
    if command -v rpmbuild &> /dev/null; then
        cpack -G RPM
        echo -e "${GREEN}✓${NC} RPM package created"
    else
        echo -e "${YELLOW}Warning: rpmbuild not found. Skipping RPM.${NC}"
    fi
    
    # Create DEB package (Debian-based)
    echo -e "${YELLOW}Creating DEB package...${NC}"
    if command -v dpkg-deb &> /dev/null; then
        cpack -G DEB
        echo -e "${GREEN}✓${NC} DEB package created"
    else
        echo -e "${YELLOW}Warning: dpkg-deb not found. Skipping DEB.${NC}"
    fi
    
    # Create AppImage (Universal Linux)
    echo -e "${YELLOW}Creating AppImage...${NC}"
    create_appimage
}

package_windows() {
    echo -e "\n${YELLOW}[Windows Packaging]${NC}"
    
    if ! command -v cpack &> /dev/null; then
        echo -e "${RED}Error: cpack not found.${NC}"
        return 1
    fi
    
    # Create NSIS installer
    echo -e "${YELLOW}Creating Windows installer...${NC}"
    cpack -G NSIS
    echo -e "${GREEN}✓${NC} Windows installer created"
    
    # Create portable ZIP
    echo -e "${YELLOW}Creating portable ZIP...${NC}"
    cpack -G ZIP
    echo -e "${GREEN}✓${NC} Portable ZIP created"
}

create_appimage() {
    # Create AppImage structure
    local APPDIR="AppDir"
    local APP_NAME="DailyReminder"
    
    echo -e "${YELLOW}Setting up AppImage directory...${NC}"
    
    # Create directory structure
    mkdir -p "$APPDIR/usr/bin"
    mkdir -p "$APPDIR/usr/lib"
    mkdir -p "$APPDIR/usr/share/applications"
    mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"
    mkdir -p "$APPDIR/frontend"
    
    # Copy executable
    cp DailyReminder "$APPDIR/usr/bin/"
    
    # Copy frontend
    cp -r frontend/* "$APPDIR/frontend/"
    
    # Create desktop file
    cat > "$APPDIR/usr/share/applications/$APP_NAME.desktop" << EOF
[Desktop Entry]
Type=Application
Name=Daily Task Reminder
Comment=Stay organized with your daily tasks
Exec=DailyReminder
Icon=daily-reminder
Categories=Utility;Office;
Terminal=false
EOF
    
    # Create AppRun script
    cat > "$APPDIR/AppRun" << 'EOF'
#!/bin/bash
APPDIR="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$APPDIR/usr/lib:$LD_LIBRARY_PATH"
exec "$APPDIR/usr/bin/DailyReminder" "$@"
EOF
    chmod +x "$APPDIR/AppRun"
    
    # Download appimagetool if not exists
    if [ ! -f "appimagetool-x86_64.AppImage" ]; then
        echo -e "${YELLOW}Downloading appimagetool...${NC}"
        wget -q "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
        chmod +x appimagetool-x86_64.AppImage
    fi
    
    # Create AppImage
    echo -e "${YELLOW}Building AppImage...${NC}"
    ARCH=x86_64 ./appimagetool-x86_64.AppImage "$APPDIR" DailyReminder-x86_64.AppImage
    
    echo -e "${GREEN}✓${NC} AppImage created: DailyReminder-x86_64.AppImage"
}

# Package based on platform
case "$PLATFORM" in
    linux)
        package_linux
        ;;
    windows)
        package_windows
        ;;
    all)
        package_linux
        package_windows
        ;;
    *)
        echo -e "${RED}Error: Invalid platform. Use 'linux', 'windows', or 'all'${NC}"
        exit 1
        ;;
esac

# Summary
echo -e "\n${BLUE}================================================${NC}"
echo -e "${GREEN}✓ Packaging Complete!${NC}"
echo -e "${BLUE}================================================${NC}"
echo -e "${YELLOW}Packages created in:${NC} ${GREEN}backend/build/${NC}"
ls -lh *.rpm *.deb *.AppImage *.exe *.zip 2>/dev/null || echo -e "${YELLOW}Check backend/build/ for generated packages${NC}"
echo -e "${BLUE}================================================${NC}"

cd ../..
