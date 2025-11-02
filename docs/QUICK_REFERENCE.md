# Daily Reminder - Quick Reference

## 🚀 Quick Start (From Root Directory)

### First Time Setup
```bash
# Linux/macOS
npm install
./build.sh

# Windows
npm install
build.bat
```

### Development
```bash
# Linux/macOS
./dev.sh

# Windows
# Terminal 1
cd frontend && npm run dev

# Terminal 2
cd backend\build && Debug\DailyReminder.exe
```

### Build
```bash
# Linux/macOS - Release
./build.sh release

# Linux/macOS - Debug
./build.sh debug

# Windows - Release
build.bat release

# Windows - Debug
build.bat debug
```

### Run
```bash
# Linux/macOS
./run.sh

# Windows
run.bat
```

### Package
```bash
# Linux
./package.sh linux       # RPM + DEB + AppImage
./package.sh windows     # EXE + ZIP (if cross-compiling)
./package.sh all         # All platforms

# Windows
package.bat              # EXE installer + ZIP
```

---

## 📁 Project Structure

```
daily-reminder/
├── package.json              ← Root package (workspace manager)
├── node_modules/             ← Shared dependencies (NO duplication!)
│
├── frontend/
│   ├── package.json          ← Frontend config
│   ├── (NO node_modules)     ← Uses root node_modules
│   └── out/                  ← Build output
│
├── backend/
│   ├── src/                  ← C++ source files
│   ├── build/
│   │   ├── DailyReminder     ← Executable (Linux/macOS)
│   │   ├── Release/          ← Release build (Windows)
│   │   ├── Debug/            ← Debug build (Windows)
│   │   ├── frontend/         ← Next.js build copied here
│   │   ├── *.rpm             ← RPM package
│   │   ├── *.deb             ← DEB package
│   │   ├── *.AppImage        ← AppImage package
│   │   └── *.exe             ← Windows installer
│   └── CMakeLists.txt
│
├── build.sh / build.bat      ← Build scripts
├── run.sh / run.bat          ← Run scripts
├── dev.sh                    ← Development mode (Linux/macOS)
└── package.sh / package.bat  ← Package scripts
```

---

## 🔧 Scripts Explained

### build.sh / build.bat
**Purpose:** Build the entire application

**Options:**
- `./build.sh debug` - Debug build (with symbols, no optimization)
- `./build.sh release` - Release build (optimized, production-ready)
- `./build.sh` - Default (release)

**What it does:**
1. Installs npm dependencies (root level)
2. Builds Next.js frontend → `frontend/out/`
3. Builds Qt backend → `backend/build/`
4. Copies frontend to `backend/build/frontend/`
5. Sets permissions

### run.sh / run.bat
**Purpose:** Run the built application

**Requirements:**
- Must run `build.sh` first
- Executable must exist in `backend/build/`

### dev.sh
**Purpose:** Development mode (Linux/macOS only)

**What it does:**
1. Starts Next.js dev server on port 3000
2. Builds Qt backend (if not built)
3. Runs Qt app (loads from localhost:3000)
4. Cleans up dev server on exit

**For Windows Development:**
```cmd
REM Terminal 1
cd frontend
npm run dev

REM Terminal 2
cd backend\build\Debug
DailyReminder.exe
```

### package.sh / package.bat
**Purpose:** Create distributable packages

**Linux Options:**
- `./package.sh linux` - RPM + DEB + AppImage
- `./package.sh windows` - Windows installer (cross-compile)
- `./package.sh all` - All platforms

**Windows:**
- `package.bat` - NSIS installer + ZIP

**Output:**
- Linux: `backend/build/*.rpm`, `*.deb`, `*.AppImage`
- Windows: `backend/build/*.exe`, `*.zip`

---

## 📦 Package Types Explained

### Linux

**RPM Package** (`.rpm`)
- For: Fedora, RHEL, CentOS, openSUSE
- Install: `sudo rpm -i DailyReminder-1.0.0.rpm`
- Uninstall: `sudo rpm -e DailyReminder`

**DEB Package** (`.deb`)
- For: Debian, Ubuntu, Linux Mint
- Install: `sudo dpkg -i DailyReminder-1.0.0.deb`
- Uninstall: `sudo dpkg -r DailyReminder`

**AppImage** (`.AppImage`)
- For: All Linux distributions (universal)
- Usage: 
  ```bash
  chmod +x DailyReminder-x86_64.AppImage
  ./DailyReminder-x86_64.AppImage
  ```
- No installation needed!
- Portable - run from anywhere

### Windows

**NSIS Installer** (`.exe`)
- Standard Windows installer
- Installs to Program Files
- Creates Start Menu shortcut
- Uninstall via Control Panel

**Portable ZIP** (`.zip`)
- Extract anywhere
- Run `DailyReminder.exe`
- No installation needed
- Fully portable

---

## 🛠️ Common Commands

### Clean Everything
```bash
# Remove all build artifacts and dependencies
rm -rf node_modules frontend/.next frontend/out backend/build

# Or use npm script
npm run clean
```

### Rebuild from Scratch
```bash
# Clean first
npm run clean

# Reinstall and build
npm install
./build.sh release
```

### Check Build Type
```bash
# Linux/macOS
file backend/build/DailyReminder
# If optimized: "stripped" appears
# If debug: "not stripped" or "with debug_info"

# Windows
# Check file size:
# Debug: ~50-100MB
# Release: ~10-30MB
```

---

## 🐛 Troubleshooting

### "node_modules not found"
```bash
npm install
```

### "Frontend has node_modules directory"
```bash
rm -rf frontend/node_modules
```

### "Build fails - Qt not found"
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-webengine-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel

# Windows
# Download from https://www.qt.io/download
```

### "Package creation fails"
```bash
# Install packaging tools

# Ubuntu/Debian (for DEB)
sudo apt install dpkg-dev

# Fedora (for RPM)
sudo dnf install rpm-build

# For AppImage
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
mv appimagetool-x86_64.AppImage backend/build/
```

### "Windows: Missing DLL errors"
```cmd
REM Run windeployqt to bundle Qt libraries
cd backend\build\Release
windeployqt DailyReminder.exe
```

---

## ⚡ Performance Tips

### Debug vs Release

**Debug Build:**
- ✅ Easy debugging with GDB/LLDB
- ✅ Full error messages
- ❌ Slower performance (2-5x)
- ❌ Larger file size

**Release Build:**
- ✅ Optimized performance
- ✅ Smaller file size (50-70% smaller)
- ✅ Production-ready
- ❌ Harder to debug

**Recommendation:**
- Use **debug** during development
- Use **release** for distribution

---

## 📊 File Sizes (Approximate)

| Component       | Debug      | Release  |
| --------------- | ---------- | -------- |
| Qt Executable   | 80-120 MB  | 15-30 MB |
| Next.js Build   | 5-10 MB    | 5-10 MB  |
| Total (Linux)   | 90-130 MB  | 20-40 MB |
| Total (Windows) | 100-150 MB | 25-45 MB |
| AppImage        | 100-150 MB | 30-50 MB |
| DEB Package     | 25-35 MB   | 10-20 MB |
| RPM Package     | 25-35 MB   | 10-20 MB |

---

## 🎯 Distribution Checklist

Before distributing to users:

- [ ] Build in **release** mode
- [ ] Test on clean system
- [ ] Verify all dependencies bundled
- [ ] Test installation process
- [ ] Test uninstallation process
- [ ] Create README for users
- [ ] Include license information
- [ ] Test on target OS versions

---

## 📝 Notes

- **Root node_modules:** All npm dependencies stored in root
- **No duplication:** Frontend uses root node_modules (via npm workspaces)
- **Build outputs:** Always in `backend/build/`
- **Data storage:** `~/.local/share/DailyReminder/` (Linux) or `%LOCALAPPDATA%\DailyReminder\` (Windows)

---

## 🔗 Quick Links

- Full Guide: `STEP_BY_STEP_GUIDE.md`
- Root Setup: `ROOT_SETUP_GUIDE.md`
- Architecture: `ARCHITECTURE.md`
- Troubleshooting: `TROUBLESHOOTING.md`
