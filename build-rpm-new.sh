#!/bin/bash
# Build RPM package for Daily Reminder

set -e

echo "🔨 Building Daily Reminder RPM package..."

# Get project root and setup output directory
PROJECT_ROOT=$(pwd)
OUTPUT_DIR="$PROJECT_ROOT/app_output"
mkdir -p "$OUTPUT_DIR"

# 1. Build frontend
echo "📦 Building frontend..."
cd frontend
npm install
npm run build
cd ..

# 2. Build backend in Release mode
echo "🔨 Building backend..."
cd backend
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
cd ../..

# 3. Create installation directory structure
echo "📁 Creating package structure..."
INSTALL_DIR="$PROJECT_ROOT/rpm-package"
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR"/{usr/bin,usr/share/daily-reminder,usr/share/applications}

# Copy files
cp backend/build/backend "$INSTALL_DIR/usr/bin/daily-reminder"
cp -r frontend/out/* "$INSTALL_DIR/usr/share/daily-reminder/"

# Create desktop entry
cat > "$INSTALL_DIR/usr/share/applications/daily-reminder.desktop" <<'DESKTOP'
[Desktop Entry]
Name=Daily Reminder
Comment=Calendar application with reminders
Exec=/usr/bin/daily-reminder
Icon=appointment-soon
Terminal=false
Type=Application
Categories=Office;Calendar;
DESKTOP

# 4. Create tarball for RPM
echo "📦 Creating source tarball..."
cd "$PROJECT_ROOT"
tar czf daily-reminder-1.0.0.tar.gz -C rpm-package .

# Setup rpmbuild directories
mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
cp daily-reminder-1.0.0.tar.gz ~/rpmbuild/SOURCES/

# 5. Create spec file
cat > ~/rpmbuild/SPECS/daily-reminder.spec <<'SPEC'
%global debug_package %{nil}

Name:           daily-reminder
Version:        1.0.0
Release:        1%{?dist}
Summary:        Desktop calendar application with reminders
License:        MIT
URL:            https://github.com/illufoxKusanagi/daily-reminder
Source0:        %{name}-%{version}.tar.gz
BuildArch:      x86_64
Requires:       qt6-qtbase qt6-qtwebengine libnotify

%description
A desktop calendar application for managing daily activities with reminders,
built with Qt 6 (backend REST API) and Next.js (frontend UI).

%prep
%setup -q -c

%install
mkdir -p %{buildroot}
cp -a * %{buildroot}/

%files
/usr/bin/daily-reminder
/usr/share/daily-reminder/*
/usr/share/applications/daily-reminder.desktop

%changelog
* Sat Nov 02 2025 Builder <builder@localhost> - 1.0.0-1
- Initial RPM release
SPEC

# 6. Build RPM
echo "📦 Building RPM..."
rpmbuild -bb ~/rpmbuild/SPECS/daily-reminder.spec

# Copy RPM to output directory
cp ~/rpmbuild/RPMS/x86_64/daily-reminder-*.rpm "$OUTPUT_DIR/"

# Cleanup
rm -rf "$INSTALL_DIR"
rm -f daily-reminder-1.0.0.tar.gz

echo ""
echo "========================================="
echo "✅ RPM package built successfully!"
echo "========================================="
echo ""
ls -lh "$OUTPUT_DIR"/*.rpm
echo ""
echo "To install:"
echo "  sudo rpm -ivh output/daily-reminder-*.rpm"
echo ""
echo "To uninstall:"
echo "  sudo rpm -e daily-reminder"
echo ""
