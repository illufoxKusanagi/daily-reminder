# VSCode CMake Configuration Guide

## Overview

Your VSCode is now configured to work with Qt Creator's build system (Ninja) and CMake.

---

## ✅ What's Installed

- ✅ **Ninja** - Fast build system (Qt Creator's default)
- ✅ **CMake** - Build configuration tool
- ✅ VSCode settings configured for Qt + CMake

---

## 🎯 Building in VSCode

### Method 1: Using VSCode Tasks (Recommended)

Press `Ctrl+Shift+P` and type "Run Task", then select:

**Configure:**
- `CMake: Configure (Debug)` - Set up debug build
- `CMake: Configure (Release)` - Set up release build

**Build:**
- `CMake: Build` - Build the project (default: `Ctrl+Shift+B`)
- `Build: All (Debug)` - Full build (frontend + backend, debug)
- `Build: All (Release)` - Full build (frontend + backend, release)

**Run:**
- `Run: Application` - Run the built application
- `Run: Development Mode` - Start dev server + app

### Method 2: Using CMake Extension

1. Install the **CMake Tools** extension (recommended)
2. Click the CMake icon in the sidebar
3. Click "Configure" (select "Ninja" as generator)
4. Click "Build"

### Method 3: Using Terminal

```bash
# Configure
cmake -S backend -B backend/build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build backend/build

# Or use the shell scripts
./build.sh debug
```

---

## 🔧 VSCode Configuration Files

### `.vscode/settings.json`
- Sets Ninja as the CMake generator
- Configures build directory
- Sets up IntelliSense paths

### `.vscode/tasks.json`
- Defines build tasks
- Configure, build, clean, and run tasks
- Available via `Ctrl+Shift+P` → "Run Task"

### `.vscode/launch.json`
- Debug configuration for GDB
- Press `F5` to debug the Qt application

### `.vscode/c_cpp_properties.json`
- C++ IntelliSense configuration
- Qt include paths
- Compiler settings

---

## 🚀 Quick Start in VSCode

### First Time Setup

1. **Open VSCode in project root:**
   ```bash
   cd ~/Documents/Daily\ Reminder
   code .
   ```

2. **Install recommended extensions:**
   - CMake Tools (`ms-vscode.cmake-tools`)
   - C/C++ (`ms-vscode.cpptools`)
   - CMake (`twxs.cmake`)

3. **Configure the project:**
   - Press `Ctrl+Shift+P`
   - Type: "CMake: Configure"
   - Select "Ninja" as generator (if asked)

4. **Build the project:**
   - Press `Ctrl+Shift+B` (default build task)
   - Or: `Ctrl+Shift+P` → "CMake: Build"

5. **Run the application:**
   - Press `F5` to debug
   - Or: `Ctrl+Shift+P` → "Tasks: Run Task" → "Run: Application"

---

## 🐛 Debugging in VSCode

### Start Debugging

1. Set breakpoints in C++ code (click left of line numbers)
2. Press `F5` or click Run → Start Debugging
3. Application will start under GDB debugger

### Debug Configurations

**"Debug Qt Application"** - Builds then debugs
**"Debug Qt Application (No Build)"** - Debugs without building

---

## 🔄 Switching Between Qt Creator and VSCode

You can use **both** tools interchangeably!

### Qt Creator → VSCode

If you build in Qt Creator first:
```bash
# Qt Creator builds to: backend/build-*
# VSCode expects: backend/build

# Option 1: Use symlink
ln -s backend/build-Desktop-Debug backend/build

# Option 2: Configure VSCode to use Qt Creator's build dir
# Edit .vscode/settings.json:
# "cmake.buildDirectory": "${workspaceFolder}/backend/build-Desktop-Debug"
```

### VSCode → Qt Creator

If you build in VSCode first:
1. Open Qt Creator
2. File → Open File or Project
3. Select `backend/CMakeLists.txt`
4. Click "Configure Project"
5. Qt Creator will detect existing build directory

---

## 📊 Build Options Comparison

| Tool           | Generator  | Build Dir         | Command           |
| -------------- | ---------- | ----------------- | ----------------- |
| **VSCode**     | Ninja      | `backend/build`   | `Ctrl+Shift+B`    |
| **Qt Creator** | Ninja      | `backend/build-*` | Click hammer icon |
| **Terminal**   | Ninja/Make | `backend/build`   | `./build.sh`      |

---

## 🛠️ Common Tasks in VSCode

### Configure for Debug
```
Ctrl+Shift+P → CMake: Configure
Select: Debug
```

### Configure for Release
```
Ctrl+Shift+P → CMake: Configure
Select: Release
```

### Build
```
Ctrl+Shift+B
```

### Clean
```
Ctrl+Shift+P → Tasks: Run Task → CMake: Clean
```

### Rebuild
```
1. Ctrl+Shift+P → Tasks: Run Task → CMake: Clean
2. Ctrl+Shift+B
```

### Run
```
Ctrl+Shift+P → Tasks: Run Task → Run: Application
```

### Debug
```
F5
```

---

## 🔍 IntelliSense / Code Completion

VSCode will now provide:
- ✅ Qt class auto-completion
- ✅ Method suggestions
- ✅ Error detection
- ✅ Go to definition (`F12`)
- ✅ Find references (`Shift+F12`)

If IntelliSense doesn't work:
1. Build the project first (generates `compile_commands.json`)
2. Reload window: `Ctrl+Shift+P` → "Developer: Reload Window"

---

## 📝 Recommended Workflow

### Development Workflow

1. **Frontend changes:**
   - Edit files in `frontend/src/`
   - Terminal: `cd frontend && npm run dev`
   - Hot reload automatically

2. **Backend changes:**
   - Edit files in `backend/src/`
   - Press `Ctrl+Shift+B` to build
   - Press `F5` to debug

3. **Full build:**
   - `Ctrl+Shift+P` → "Tasks: Run Task" → "Build: All (Debug)"

### Release Workflow

1. `Ctrl+Shift+P` → "Tasks: Run Task" → "Build: All (Release)"
2. Test: `./run.sh`
3. Package: `./package.sh linux`

---

## 🆘 Troubleshooting

### "CMake generator not found"
```bash
# Install Ninja
sudo dnf install ninja-build

# Or use Make instead
cmake -G "Unix Makefiles" ..
```

### "Qt headers not found"
Check Qt installation:
```bash
# Find Qt include path
qmake6 -query QT_INSTALL_HEADERS

# Update .vscode/c_cpp_properties.json with the path
```

### "compile_commands.json not found"
```bash
# Build the project first
cmake -S backend -B backend/build -G Ninja
cmake --build backend/build

# Reload VSCode window
Ctrl+Shift+P → "Developer: Reload Window"
```

### IntelliSense not working
1. Install C/C++ extension
2. Install CMake Tools extension
3. Build the project once
4. Reload window

---

## 🎯 Quick Reference

| Action               | Shortcut                            |
| -------------------- | ----------------------------------- |
| **Build**            | `Ctrl+Shift+B`                      |
| **Debug**            | `F5`                                |
| **Run Task**         | `Ctrl+Shift+P` → "Tasks: Run Task"  |
| **Configure CMake**  | `Ctrl+Shift+P` → "CMake: Configure" |
| **Go to Definition** | `F12`                               |
| **Find References**  | `Shift+F12`                         |
| **Command Palette**  | `Ctrl+Shift+P`                      |

---

## ✨ Summary

You can now:
- ✅ Build Qt project in VSCode using Ninja
- ✅ Debug with GDB (`F5`)
- ✅ Use IntelliSense for Qt classes
- ✅ Switch between VSCode and Qt Creator
- ✅ Use keyboard shortcuts for common tasks

**Choose your preferred tool:**
- VSCode for full-stack development (frontend + backend)
- Qt Creator for Qt-specific features (UI designer, profiling)
- Terminal for automated builds (`./build.sh`)

All three methods work with the same codebase! 🎉
