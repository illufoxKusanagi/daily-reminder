# Project Setup Guide - Root-Level Configuration

## Overview

This project uses a **monorepo structure** with root-level `node_modules` to avoid duplication.

```
daily-reminder/
├── package.json          ← Root package.json (manages workspace)
├── node_modules/         ← Shared dependencies (installed here)
├── build.sh              ← Build script (debug/release)
├── run.sh                ← Run the application
├── dev.sh                ← Development mode
├── package.sh            ← Create distributable packages
├── frontend/
│   ├── package.json      ← Frontend-specific config
│   └── (no node_modules) ← Uses root node_modules
└── backend/
    └── build/
        └── DailyReminder ← Executable
```

---

## Initial Setup

### 1. Install Dependencies (Root Level)

```bash
# From project root
npm install
```

This installs all dependencies in the **root `node_modules/`** directory.

### 2. Make Scripts Executable

```bash
chmod +x build.sh run.sh dev.sh package.sh
```

---

## Usage

### Development Mode

Start Next.js dev server + Qt application:

```bash
./dev.sh
```

This will:
- Start Next.js on `http://localhost:3000`
- Build Qt backend (if needed)
- Launch Qt application

### Build for Production

**Debug Build:**
```bash
./build.sh debug
```

**Release Build:**
```bash
./build.sh release
```

Or default (release):
```bash
./build.sh
```

### Run the Application

```bash
./run.sh
```

### Create Distribution Packages

**Linux Packages (RPM + DEB + AppImage):**
```bash
./package.sh linux
```

**Windows Package (EXE installer + ZIP):**
```bash
./package.sh windows
```

**All Platforms:**
```bash
./package.sh all
```

---

## Refactoring Existing Projects

If you have a project with **duplicate node_modules** (both in root and frontend):

### Step 1: Backup

```bash
cp -r your-project your-project-backup
```

### Step 2: Clean Frontend node_modules

```bash
cd your-project/frontend
rm -rf node_modules package-lock.json
cd ..
```

### Step 3: Create Root package.json

Create `package.json` in root:

```json
{
  "name": "your-project",
  "version": "1.0.0",
  "workspaces": [
    "frontend"
  ],
  "scripts": {
    "dev": "cd frontend && npm run dev",
    "build:frontend": "cd frontend && npm run build"
  }
}
```

### Step 4: Modify Frontend package.json

Change the name to just `"frontend"`:

```json
{
  "name": "frontend",
  "version": "0.1.0",
  ...
}
```

### Step 5: Install Dependencies

```bash
# From root
npm install
```

Now `node_modules/` will be in the root, and frontend will use it automatically!

### Step 6: Verify

```bash
# Check that frontend has no node_modules
ls frontend/node_modules  # Should not exist or be empty

# Check root has node_modules
ls node_modules  # Should contain all packages
```

---

## Build Types Explained

### Debug Build

```bash
./build.sh debug
```

- Includes debug symbols
- No optimization
- Better for development/debugging
- Larger executable size
- Slower performance

**When to use:**
- During development
- When debugging issues
- When using GDB/LLDB

### Release Build

```bash
./build.sh release
```

- Optimized code
- Smaller executable
- Faster performance
- No debug symbols
- Strip unnecessary data

**When to use:**
- Production deployment
- Distribution to users
- Performance testing

---

## Package Distribution

### Linux Packages

**RPM (Fedora, RHEL, CentOS, openSUSE):**
```bash
sudo rpm -i backend/build/DailyReminder-1.0.0.rpm
```

**DEB (Debian, Ubuntu, Linux Mint):**
```bash
sudo dpkg -i backend/build/DailyReminder-1.0.0.deb
```

**AppImage (Universal - All Linux):**
```bash
chmod +x backend/build/DailyReminder-x86_64.AppImage
./backend/build/DailyReminder-x86_64.AppImage
```

### Windows Packages

**Installer (.exe):**
- Double-click to install
- Installs to Program Files
- Creates Start Menu shortcuts

**Portable (.zip):**
- Extract anywhere
- Run DailyReminder.exe
- No installation needed

---

## Scripts Reference

### build.sh

```bash
./build.sh [debug|release]
```

Steps:
1. Install npm dependencies
2. Build Next.js frontend
3. Build Qt backend (debug or release)
4. Copy frontend to backend
5. Set permissions

### run.sh

```bash
./run.sh
```

Launches the built application.

### dev.sh

```bash
./dev.sh
```

Development mode:
- Starts Next.js dev server (port 3000)
- Runs Qt app connected to dev server
- Auto-cleanup on exit

### package.sh

```bash
./package.sh [linux|windows|all]
```

Creates distribution packages:
- **linux**: RPM, DEB, AppImage
- **windows**: NSIS installer, ZIP
- **all**: All of the above

---

## Troubleshooting

### "npm: command not found"

Install Node.js:
```bash
# Ubuntu/Debian
sudo apt install nodejs npm

# Fedora
sudo dnf install nodejs npm
```

### "Module not found" errors

Reinstall dependencies:
```bash
rm -rf node_modules package-lock.json
npm install
```

### Frontend still has node_modules

Remove it:
```bash
rm -rf frontend/node_modules
```

npm will use root `node_modules/` automatically.

### Build fails with Qt errors

Install Qt dependencies:
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtwebchannel-devel
```

### Package creation fails

Install packaging tools:
```bash
# For RPM
sudo dnf install rpm-build

# For DEB
sudo apt install dpkg-dev

# For AppImage
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
```

---

## Directory Structure After Setup

```
daily-reminder/
├── package.json
├── package-lock.json
├── node_modules/              ← All dependencies here
│   ├── react/
│   ├── next/
│   └── ...
├── frontend/
│   ├── package.json
│   ├── app/
│   ├── components/
│   └── (no node_modules)      ← Uses root node_modules
├── backend/
│   ├── src/
│   ├── build/
│   │   ├── DailyReminder
│   │   ├── frontend/          ← Next.js build copied here
│   │   ├── *.rpm              ← Generated packages
│   │   ├── *.deb
│   │   └── *.AppImage
│   └── CMakeLists.txt
├── build.sh
├── run.sh
├── dev.sh
└── package.sh
```

---

## Quick Commands Summary

```bash
# Install dependencies
npm install

# Development
./dev.sh

# Build (release)
./build.sh

# Build (debug)
./build.sh debug

# Run
./run.sh

# Package for Linux
./package.sh linux

# Package for Windows
./package.sh windows

# Clean everything
npm run clean
```

---

## Benefits of This Setup

✅ **No Duplicate Dependencies** - Single `node_modules/` in root
✅ **Easy Build Management** - One command for debug/release
✅ **Multi-Platform** - Linux (RPM, DEB, AppImage) + Windows (EXE, ZIP)
✅ **Simple Workflow** - Clear scripts for each task
✅ **Fast Installation** - Workspace setup handles dependencies
✅ **Professional Packaging** - Installers for end users

---

## Next Steps

1. Run `npm install` to set up workspace
2. Use `./dev.sh` for development
3. Use `./build.sh release` for production build
4. Use `./package.sh linux` to create distributable packages
5. Share packages with users!
