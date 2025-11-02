# Daily Task Reminder - Setup Checklist

Use this checklist to ensure you have everything set up correctly.

## Prerequisites Checklist

### System Requirements
- [ ] Operating System: Linux, macOS, or Windows
- [ ] Internet connection (for downloading dependencies)
- [ ] ~500MB free disk space

### Software Requirements

#### For Frontend Development
- [ ] Node.js 18+ installed
  ```bash
  node --version  # Should show v18.x.x or higher
  ```
- [ ] npm installed
  ```bash
  npm --version
  ```

#### For Backend Development
- [ ] Qt 6.x or Qt 5.15+ installed
- [ ] Qt WebEngine module installed
- [ ] Qt WebChannel module installed
- [ ] CMake 3.16+ installed (or qmake)
  ```bash
  cmake --version
  ```
- [ ] C++17 compatible compiler
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode Command Line Tools
  - Windows: MSVC 2019+ or MinGW

## Installation Checklist

### Step 1: Install Node.js
- [ ] Download from https://nodejs.org/ (LTS version recommended)
- [ ] Or install via package manager:
  ```bash
  # Linux (Ubuntu/Debian)
  curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
  sudo apt-get install -y nodejs
  
  # macOS
  brew install node@18
  
  # Or use nvm (recommended)
  curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
  nvm install 18
  ```

### Step 2: Install Qt
- [ ] Linux:
  ```bash
  sudo apt-get update
  sudo apt-get install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev cmake build-essential
  ```
- [ ] macOS:
  ```bash
  brew install qt@6 cmake
  export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
  ```
- [ ] Windows:
  - Download Qt Online Installer: https://www.qt.io/download
  - Select Qt 6.x
  - Select Components: Qt WebEngine, Qt WebChannel
  - Install CMake: https://cmake.org/download/

### Step 3: Clone/Download Project
- [ ] Navigate to the project directory
  ```bash
  cd ~/Documents/Daily\ Reminder
  ```

### Step 4: Setup Frontend
- [ ] Navigate to frontend directory
  ```bash
  cd frontend
  ```
- [ ] Install dependencies
  ```bash
  npm install
  ```
- [ ] Verify installation
  ```bash
  npm run dev
  ```
- [ ] Check http://localhost:3000 in browser
- [ ] Stop dev server (Ctrl+C)

### Step 5: Setup Backend
- [ ] Navigate to backend directory
  ```bash
  cd ../backend
  ```
- [ ] Create build directory
  ```bash
  mkdir build && cd build
  ```
- [ ] Run CMake
  ```bash
  cmake ..
  ```
  - [ ] No errors about missing Qt
  - [ ] No errors about missing WebEngine
  - [ ] No errors about missing WebChannel
- [ ] Build the project
  ```bash
  cmake --build .
  ```
- [ ] Verify executable exists
  ```bash
  ls -la DailyReminder  # Linux/macOS
  dir DailyReminder.exe # Windows
  ```

## First Run Checklist

### Development Mode
- [ ] Start frontend dev server
  ```bash
  cd frontend
  npm run dev
  ```
- [ ] In a new terminal, run Qt app
  ```bash
  cd backend/build
  ./DailyReminder
  ```
- [ ] Application window opens
- [ ] UI loads correctly (no blank screen)
- [ ] Green "Qt Connected" badge visible
- [ ] Can add a test task
- [ ] Task appears in the list
- [ ] Can mark task as complete
- [ ] Can delete task

### Production Mode
- [ ] Build frontend for production
  ```bash
  cd frontend
  npm run build
  ```
- [ ] Verify `out/` directory created
  ```bash
  ls -la out/
  ```
- [ ] Copy frontend to backend
  ```bash
  mkdir -p ../backend/build/frontend
  cp -r out/* ../backend/build/frontend/
  ```
- [ ] Run Qt application
  ```bash
  cd ../backend/build
  ./DailyReminder
  ```
- [ ] Application loads offline (no dev server needed)
- [ ] All features work correctly

## Feature Verification Checklist

### Basic Features
- [ ] Add new task
- [ ] Task appears in "Active Tasks" section
- [ ] Edit task (mark complete)
- [ ] Task moves to "Completed Tasks" section
- [ ] Delete task
- [ ] Task removed from list

### Task Properties
- [ ] Set task title (required)
- [ ] Set task description (optional)
- [ ] Set due date
- [ ] Set priority (low/medium/high)
- [ ] Priority badge shows correct color
- [ ] Date displays correctly

### Data Persistence
- [ ] Add several tasks
- [ ] Close application
- [ ] Reopen application
- [ ] All tasks still present
- [ ] Check data file exists:
  ```bash
  # Linux
  ls ~/.local/share/DailyReminder/tasks.json
  
  # macOS
  ls ~/Library/Application\ Support/DailyReminder/tasks.json
  
  # Windows
  dir %LOCALAPPDATA%\DailyReminder\tasks.json
  ```

### UI/UX
- [ ] Dark mode works (if system uses dark theme)
- [ ] Responsive layout
- [ ] Smooth animations
- [ ] No console errors (check browser dev tools)
- [ ] Window can be resized

## Build Scripts Checklist

### Linux/macOS
- [ ] Scripts are executable
  ```bash
  chmod +x scripts/*.sh
  ```
- [ ] Build all script works
  ```bash
  ./scripts/build-all.sh
  ```
- [ ] Run dev script works
  ```bash
  ./scripts/run-dev.sh
  ```

### Windows
- [ ] Build all script works
  ```cmd
  scripts\build-all.bat
  ```

## Troubleshooting Checklist

If something doesn't work:

- [ ] Check error messages in terminal
- [ ] Check browser console (F12)
- [ ] Enable Qt debugging
  ```cpp
  // Add to main.cpp
  qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
  ```
- [ ] Review TROUBLESHOOTING.md
- [ ] Verify all prerequisites are installed
- [ ] Try clean build (delete build/ and rebuild)
- [ ] Try clean frontend (delete node_modules/ and reinstall)

## Documentation Checklist

- [ ] Read README.md for overview
- [ ] Read QUICKSTART.md for fast setup
- [ ] Read ARCHITECTURE.md to understand design
- [ ] Read TROUBLESHOOTING.md if issues occur
- [ ] Check example code in source files

## Next Steps

After completing this checklist:

- [ ] ✅ Application runs successfully
- [ ] ✅ All features work
- [ ] ✅ Data persists correctly
- [ ] Start customizing the UI
- [ ] Add new features
- [ ] Package for distribution

## Quick Reference

### Development Workflow
```bash
# Terminal 1: Frontend
cd frontend && npm run dev

# Terminal 2: Backend
cd backend/build && ./DailyReminder
```

### Production Build
```bash
# One command (using scripts)
./scripts/build-all.sh

# Or manually
cd frontend && npm run build
cd ../backend && mkdir build && cd build
cmake .. && cmake --build .
```

### Useful Commands
```bash
# Check versions
node --version
npm --version
cmake --version
qmake --version

# Clean build
rm -rf backend/build frontend/.next frontend/node_modules

# View logs
tail -f ~/.local/share/DailyReminder/tasks.json
```

## Support

If you complete all items and still have issues:
1. Review error messages carefully
2. Search for specific errors online
3. Check Qt documentation: https://doc.qt.io/
4. Check Next.js documentation: https://nextjs.org/docs
5. Review GitHub issues (if applicable)

---

**Congratulations!** 🎉 If you've checked all boxes, you're ready to start developing!
