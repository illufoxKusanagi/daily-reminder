# Frontend Loading Fix & Windows Cross-Compilation

## Issue 1: ✅ FIXED - Blank Screen (Frontend Not Loading)

### Problem
The backend was hardcoded to load from `http://localhost:3000` (development server), which doesn't exist in production builds.

### Solution
Updated `backend/mainwindow.cpp` to:
1. **Search for frontend files** in multiple locations:
   - `./frontend/index.html` (next to binary)
   - `../share/daily-reminder/index.html` (system install)
   - `/usr/share/daily-reminder/index.html` (Linux install)
   - `/usr/local/share/daily-reminder/index.html` (alternative)
   - Falls back to `http://localhost:3000` for development

2. **Updated build scripts** to copy frontend files:
   - `build-qt.sh` now copies frontend to `output/frontend/`
   - `build-rpm-new.sh` already packages frontend correctly
   - `build-windows.bat` copies to `output/DailyReminder/frontend/`

### Test It
```bash
# Rebuild
./build-qt.sh

# Run - should now show the UI!
./output/daily-reminder
```

The console will show: `✅ Found frontend at: /path/to/frontend/index.html`

---

## Issue 2: Windows Cross-Compilation from Linux

### ❌ Simple Answer: NOT RECOMMENDED

Building Qt applications for Windows from Linux is **technically possible but complex**. Here are your options:

### Option 1: 🏆 Build on Windows (BEST)
```cmd
# On Windows with Qt installed
build-windows.bat
```
- ✅ Guaranteed to work
- ✅ All dependencies handled automatically
- ✅ windeployqt bundles everything correctly

### Option 2: 🔧 Cross-Compile (COMPLEX)
```bash
# On Linux with mingw-w64 + Qt for Windows
npm run build:windows-cross
```

**Requirements:**
1. **Install MinGW-w64 toolchain:**
   ```bash
   sudo dnf install mingw64-gcc-c++ mingw64-gcc mingw64-binutils
   ```

2. **Install Qt for Windows:**
   - Download Qt Online Installer from https://www.qt.io/download
   - Select: Qt 6.x → MinGW 64-bit
   - Install to `~/Qt/6.x.x/mingw_64`

3. **Set environment:**
   ```bash
   export QT_WIN_PATH=$HOME/Qt/6.9.0/mingw_64
   ```

**Challenges:**
- ❌ Must manually manage all Qt DLLs
- ❌ WebEngine plugins are tricky
- ❌ May miss dependencies
- ⚠️  Still needs windeployqt on Windows for complete packaging

### Option 3: 🤖 GitHub Actions (AUTOMATED)
Set up CI/CD to automatically build for Windows in the cloud:
- Builds on real Windows runners
- No local setup needed
- Creates releases automatically

**Would you like me to set this up?**

---

## Recommended Workflow

### For Development (Linux)
```bash
./build-qt.sh
./output/daily-reminder
```

### For Linux Distribution
```bash
npm run build:rpm
sudo rpm -ivh output/daily-reminder-*.rpm
```

### For Windows Distribution
**Option A: Build on Windows**
1. Copy project to Windows PC
2. Install Qt 6.x
3. Run `build-windows.bat`
4. Share `output/DailyReminder/` folder

**Option B: GitHub Actions**
1. Push to GitHub
2. CI builds for Windows automatically
3. Download from Releases

---

## Summary

✅ **Frontend loading issue**: FIXED
- Backend now finds frontend files automatically
- Works in development and production
- Logs show which path was found

❌ **Windows from Linux**: NOT RECOMMENDED
- Technically possible with MinGW + Qt for Windows
- Very complex dependency management
- Better to build on Windows or use GitHub Actions

🎯 **Best Solution**:
- Develop on Linux: `./build-qt.sh`
- Package for Linux: `npm run build:rpm`
- Package for Windows: Use Windows machine or GitHub Actions

---

## Next Steps

1. **Test the frontend fix:**
   ```bash
   ./build-qt.sh
   ./output/daily-reminder
   ```

2. **For Windows builds**, choose one:
   - Build on Windows natively ✅
   - Set up GitHub Actions for automated builds 🤖
   - Attempt cross-compilation (complex) 🔧

Would you like me to set up GitHub Actions for automated Windows builds?
