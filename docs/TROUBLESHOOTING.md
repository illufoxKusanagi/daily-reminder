# Troubleshooting Guide

## Common Issues and Solutions

### Frontend Issues

#### 1. `npm install` fails

**Problem**: Dependencies fail to install
```
npm ERR! code ERESOLVE
```

**Solution**:
```bash
# Clear npm cache
npm cache clean --force

# Delete node_modules and package-lock.json
rm -rf node_modules package-lock.json

# Reinstall
npm install

# Or use --legacy-peer-deps
npm install --legacy-peer-deps
```

#### 2. Next.js build fails

**Problem**: Build errors during `npm run build`

**Solution**:
```bash
# Check Node version (requires 18+)
node --version

# Update if needed
nvm install 18
nvm use 18

# Clear Next.js cache
rm -rf .next

# Rebuild
npm run build
```

#### 3. Tailwind CSS not working

**Problem**: Styles not applied

**Solution**:
- Ensure `tailwind.config.js` has correct content paths
- Check `globals.css` has `@tailwind` directives
- Restart dev server

### Backend Issues

#### 1. Qt not found

**Problem**:
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solution**:
```bash
# Linux - Install Qt6
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev

# macOS
brew install qt@6
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6"

# Or manually specify Qt path
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64 ..
```

#### 2. WebEngine module not found

**Problem**:
```
error: QtWebEngineView: No such file or directory
```

**Solution**:
```bash
# Linux
sudo apt-get install qt6-webengine-dev libqt6webenginecore6

# macOS
brew reinstall qt@6

# Windows
# Reinstall Qt with WebEngine module selected
```

#### 3. WebChannel not connecting

**Problem**: Qt bridge not available in JavaScript

**Solution**:

1. Check if `qwebchannel.js` is loaded:
```javascript
// In browser console (F12)
console.log(typeof QWebChannel);  // Should not be 'undefined'
```

2. Enable Qt WebEngine debugging:
```cpp
// Add to main.cpp before QApplication
qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
```

3. Open Chrome and go to `chrome://inspect`

4. Verify channel registration:
```cpp
// In mainwindow.cpp
qDebug() << "Registering qtBridge with WebChannel";
m_webChannel->registerObject(QStringLiteral("qtBridge"), m_qtBridge);
```

#### 4. Build fails with "undefined reference"

**Problem**:
```
undefined reference to vtable for QtBridge
```

**Solution**:
```bash
# Clean build
cd build
rm -rf *
cmake ..
cmake --build .
```

**Or** ensure MOC is running:
```cmake
# In CMakeLists.txt
set(CMAKE_AUTOMOC ON)
```

### Integration Issues

#### 1. Frontend loads but shows blank screen

**Problem**: White screen in Qt WebEngine

**Solution**:

1. Check browser console (enable remote debugging)
2. Verify correct path:
```cpp
// In mainwindow.cpp
QString htmlPath = QCoreApplication::applicationDirPath() + "/frontend/out/index.html";
qDebug() << "Loading from:" << htmlPath;
qDebug() << "File exists:" << QFile::exists(htmlPath);
```

3. Check for CORS issues (shouldn't happen with file://)
4. Try loading development server instead:
```cpp
m_webView->load(QUrl("http://localhost:3000"));
```

#### 2. Tasks not persisting

**Problem**: Tasks disappear after restart

**Solution**:

1. Check data directory:
```cpp
// Add to TaskManager constructor
qDebug() << "Data file path:" << m_dataFilePath;
```

2. Verify write permissions:
```bash
# Linux/macOS
ls -la ~/.local/share/DailyReminder/
chmod 644 ~/.local/share/DailyReminder/tasks.json
```

3. Check JSON format:
```bash
cat ~/.local/share/DailyReminder/tasks.json | python -m json.tool
```

#### 3. Qt bridge methods not working

**Problem**: Calling `window.qtBridge.addTask()` does nothing

**Solution**:

1. Verify bridge is available:
```javascript
// In browser console
console.log(window.qtBridge);
console.log(typeof window.qtBridge.addTask);
```

2. Check Qt side connection:
```cpp
// In qtbridge.cpp - add debug output
void QtBridge::addTask(const QJsonObject& taskData) {
    qDebug() << "addTask called with:" << taskData;
    // ... rest of code
}
```

3. Ensure WebChannel is initialized:
```javascript
// Check in console
if (typeof qt !== 'undefined') {
    console.log('Qt WebChannel transport available');
}
```

### Platform-Specific Issues

#### Linux

**Problem**: Missing system dependencies
```
error while loading shared libraries: libQt6WebEngine.so.6
```

**Solution**:
```bash
sudo apt-get install libqt6webenginecore6 libqt6webenginewidgets6
sudo ldconfig
```

#### macOS

**Problem**: "App is damaged and can't be opened"

**Solution**:
```bash
xattr -cr /path/to/DailyReminder.app
```

**Problem**: Qt libraries not found

**Solution**:
```bash
# Add to ~/.zshrc or ~/.bash_profile
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/qt@6/lib"
export CPPFLAGS="-I/opt/homebrew/opt/qt@6/include"
```

#### Windows

**Problem**: Missing DLL errors

**Solution**:
```cmd
# Use windeployqt to copy required DLLs
cd backend\build\Release
windeployqt DailyReminder.exe
```

**Problem**: Qt not found in PATH

**Solution**:
```cmd
# Add to environment variables
set PATH=C:\Qt\6.x\msvc2019_64\bin;%PATH%
```

### Build Script Issues

#### Scripts won't execute (Linux/macOS)

**Solution**:
```bash
chmod +x scripts/*.sh
```

#### Permission denied on tasks.json

**Solution**:
```bash
# Linux/macOS
chmod 644 ~/.local/share/DailyReminder/tasks.json

# Or delete and let app recreate
rm ~/.local/share/DailyReminder/tasks.json
```

## Debugging Tips

### Enable All Debug Output

**Frontend**:
```typescript
// In useQtBridge.ts
console.log('Qt available:', isQtAvailable);
console.log('Current tasks:', tasks);
```

**Backend**:
```cpp
// In main.cpp
qSetMessagePattern("[%{type}] %{function}: %{message}");

// Enable WebEngine debugging
qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--enable-logging --v=1");
```

### Test WebChannel Separately

Create a simple test HTML file:
```html
<!DOCTYPE html>
<html>
<body>
    <script src="qrc:///qtwebchannel/qwebchannel.js"></script>
    <script>
        new QWebChannel(qt.webChannelTransport, function(channel) {
            console.log('Channel objects:', channel.objects);
            window.qtBridge = channel.objects.qtBridge;
            console.log('Bridge methods:', Object.keys(qtBridge));
        });
    </script>
</body>
</html>
```

### Check Versions

```bash
# Node.js
node --version  # Should be 18+

# npm
npm --version

# Qt
qmake --version  # or: cmake --version

# C++
g++ --version  # or: clang++ --version
```

## Getting Help

1. **Check Documentation**:
   - README.md
   - ARCHITECTURE.md
   - QUICKSTART.md

2. **Search Issues**:
   - Qt WebEngine issues: https://bugreports.qt.io/
   - Next.js issues: https://github.com/vercel/next.js/issues

3. **Enable Debug Mode**:
   - Qt: `qDebug()` statements
   - JavaScript: `console.log()`
   - Remote debugging: Chrome DevTools

4. **Minimal Reproduction**:
   - Test with `webchannel-test.html`
   - Test frontend standalone: `npm run dev`
   - Test backend with simple HTML

## Clean Slate

If all else fails, start fresh:

```bash
# Frontend
cd frontend
rm -rf node_modules .next package-lock.json
npm install
npm run build

# Backend
cd ../backend
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .

# Data
rm -rf ~/.local/share/DailyReminder/tasks.json
```

## Still Need Help?

1. Check Qt version compatibility
2. Verify all dependencies are installed
3. Review error messages carefully
4. Search for specific error messages online
5. Create a minimal example that reproduces the issue
