#!/bin/bash
# Build RPM package for Daily Reminder

set -e

echo "🔨 Building Daily Reminder RPM package..."

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
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)
cd ../..

# 3. Create RPM spec file
echo "📝 Creating RPM spec file..."
cat > daily-reminder.spec <<'EOF'
Name:           daily-reminder
Version:        1.0.0
Release:        1%{?dist}
Summary:        Desktop calendar application with reminders
License:        MIT
URL:            https://github.com/illufoxKusanagi/daily-reminder
BuildArch:      x86_64

Requires:       qt6-qtbase qt6-qtwebengine libnotify

%description
A desktop calendar application for managing daily activities with reminders,
built with Qt 6 (backend REST API) and Next.js (frontend UI).

%install
mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/daily-reminder
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/icons/hicolor/256x256/apps

# Install binary
install -m 755 backend/build/backend %{buildroot}/usr/bin/daily-reminder

# Install frontend files
cp -r frontend/out/* %{buildroot}/usr/share/daily-reminder/

# Create desktop entry
cat > %{buildroot}/usr/share/applications/daily-reminder.desktop <<DESKTOP
[Desktop Entry]
Name=Daily Reminder
Comment=Calendar application with reminders
Exec=/usr/bin/daily-reminder
Icon=appointment-soon
Terminal=false
Type=Application
Categories=Office;Calendar;
DESKTOP

%files
/usr/bin/daily-reminder
/usr/share/daily-reminder/*
/usr/share/applications/daily-reminder.desktop

%changelog
* Sat Nov 02 2025 Builder <builder@localhost> - 1.0.0-1
- Initial RPM release

EOF

# 4. Build RPM
echo "📦 Building RPM..."
mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Get the current directory (project root)
PROJECT_ROOT=$(pwd)

# Update spec file to use absolute paths
sed -i "s|backend/build/backend|${PROJECT_ROOT}/backend/build/backend|g" daily-reminder.spec
sed -i "s|frontend/out/\*|${PROJECT_ROOT}/frontend/out/*|g" daily-reminder.spec

cp daily-reminder.spec ~/rpmbuild/SPECS/
rpmbuild -bb ~/rpmbuild/SPECS/daily-reminder.spec

echo "✅ RPM package built successfully!"
echo "📦 Package location: ~/rpmbuild/RPMS/x86_64/daily-reminder-1.0.0-1.*.rpm"
echo ""
echo "To install:"
echo "  sudo rpm -ivh ~/rpmbuild/RPMS/x86_64/daily-reminder-1.0.0-1.*.rpm"
