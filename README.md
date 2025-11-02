# Daily Task Reminder

A desktop application for managing daily tasks, built with Qt (backend) and Next.js (frontend), integrated using QWebEngine.

## Architecture

- **Frontend**: Next.js (React) with TypeScript and Tailwind CSS
- **Backend**: Qt 6 (C++) with QWebEngineView and QWebChannel
- **Communication**: Qt WebChannel for JavaScript ↔ C++ bridge

## Project Structure

```
daily-reminder/
├── frontend/          # Next.js application
│   ├── src/
│   │   ├── app/      # Next.js 14 App Router
│   │   ├── components/
│   │   ├── hooks/
│   │   └── types/
│   └── package.json
│
├── backend/           # Qt application
│   ├── src/
│   │   ├── main.cpp
│   │   ├── mainwindow.{h,cpp}
│   │   ├── taskmanager.{h,cpp}
│   │   └── qtbridge.{h,cpp}
│   └── CMakeLists.txt
│
└── README.md
```

## Prerequisites

### Frontend
- Node.js 18+ and npm/yarn/pnpm
- Next.js dependencies

### Backend
- Qt 6.x or Qt 5.15+ (with WebEngine and WebChannel modules)
- CMake 3.16+ or qmake
- C++17 compiler (GCC, Clang, or MSVC)

## Step-by-Step Setup Guide

### Step 1: Install Dependencies

#### Install Qt
**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev
# Or for Qt5:
# sudo apt-get install qtbase5-dev qtwebengine5-dev libqt5webchannel5-dev
```

**macOS:**
```bash
brew install qt@6
# Or: brew install qt@5
```

**Windows:**
- Download Qt from https://www.qt.io/download
- Install Qt with WebEngine and WebChannel modules

#### Install Node.js
```bash
# Using nvm (recommended)
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
nvm install 18
nvm use 18

# Or download from: https://nodejs.org/
```

### Step 2: Build the Frontend

```bash
cd frontend

# Install dependencies
npm install
# Or: yarn install
# Or: pnpm install

# For development (runs dev server on localhost:3000)
npm run dev

# For production (creates static export)
npm run build
```

The production build will create an `out/` directory with static HTML/CSS/JS files.

### Step 3: Build the Qt Backend

#### Using CMake (Recommended)

```bash
cd backend
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# The executable will be in: backend/build/DailyReminder
```

#### Using qmake

```bash
cd backend

# Generate Makefile
qmake daily-reminder.pro

# Build
make

# The executable will be in: backend/DailyReminder
```

### Step 4: Run the Application

#### Development Mode
1. Start the Next.js dev server:
   ```bash
   cd frontend
   npm run dev
   ```

2. Run the Qt application (it will connect to localhost:3000):
   ```bash
   cd backend/build
   ./DailyReminder
   ```

#### Production Mode
1. Build and export the Next.js frontend:
   ```bash
   cd frontend
   npm run build
   ```

2. Copy the frontend build to the backend directory:
   ```bash
   mkdir -p backend/build/frontend
   cp -r frontend/out backend/build/frontend/
   ```

3. Run the Qt application:
   ```bash
   cd backend/build
   ./DailyReminder
   ```

## How It Works

### Qt WebChannel Bridge

The `QtBridge` class exposes C++ methods to JavaScript:

```cpp
// C++ side (qtbridge.h)
class QtBridge : public QObject {
    Q_OBJECT
public slots:
    void addTask(const QJsonObject& taskData);
    void getTasks();
signals:
    void sendTasks(const QJsonArray& tasks);
};
```

```typescript
// JavaScript side (useQtBridge.ts)
window.qtBridge.addTask(taskData);
window.receiveTasksFromQt = (tasks) => {
    setTasks(tasks);
};
```

### Data Flow

1. **User Action** → Frontend React component
2. **React Hook** → Calls `window.qtBridge.method()`
3. **Qt WebChannel** → Invokes C++ method
4. **TaskManager** → Processes and stores data
5. **Qt Signal** → Sends data back to frontend
6. **JavaScript Callback** → Updates React state

### Data Persistence

Tasks are stored in a JSON file:
- **Linux**: `~/.local/share/DailyReminder/tasks.json`
- **macOS**: `~/Library/Application Support/DailyReminder/tasks.json`
- **Windows**: `C:\Users\<username>\AppData\Local\DailyReminder\tasks.json`

## Features

- ✅ Add, edit, and delete tasks
- ✅ Mark tasks as complete
- ✅ Set task priority (low, medium, high)
- ✅ Set due dates
- ✅ Persistent storage
- ✅ Dark mode support
- ✅ Responsive UI

## Development Tips

### Debugging WebChannel

Enable Qt WebEngine debugging:
```cpp
// In main.cpp, before creating QApplication
qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
```

Then open Chrome and navigate to `chrome://inspect` to debug.

### Frontend Development

The frontend works standalone without Qt:
```bash
cd frontend
npm run dev
```

Access at http://localhost:3000 - it will use fallback mock data.

### Qt Development

Edit `mainwindow.cpp` to switch between dev and production modes:
```cpp
// Development: Load from localhost
m_webView->load(QUrl("http://localhost:3000"));

// Production: Load from static files
m_webView->load(QUrl::fromLocalFile(htmlPath));
```

## Troubleshooting

### Qt WebEngine not found
Install Qt WebEngine module:
```bash
# Linux
sudo apt-get install qt6-webengine-dev

# macOS
brew reinstall qt@6
```

### Next.js build errors
Clear cache and rebuild:
```bash
cd frontend
rm -rf .next node_modules
npm install
npm run build
```

### WebChannel not connecting
1. Check browser console for errors
2. Verify `qwebchannel.js` is loaded
3. Check Qt debug output
4. Ensure CORS is not blocking (use file:// or localhost)

## Building for Distribution

### Linux AppImage
```bash
# Use linuxdeployqt
linuxdeployqt DailyReminder -appimage
```

### macOS App Bundle
```bash
# Use macdeployqt
macdeployqt DailyReminder.app -dmg
```

### Windows Installer
```bash
# Use windeployqt
windeployqt DailyReminder.exe
# Then create installer with NSIS or Inno Setup
```

## License

MIT License - feel free to use and modify!

## Contributing

Pull requests are welcome! Please ensure:
- Frontend code follows TypeScript best practices
- Backend code follows Qt coding conventions
- All features are tested

## Support

For issues or questions, please open a GitHub issue.
