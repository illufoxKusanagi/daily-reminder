# Daily Task Reminder - Quick Start

## Quick Development Setup

### 1. Frontend Setup (5 minutes)
```bash
cd frontend
npm install
npm run dev
```
Frontend will be available at: http://localhost:3000

### 2. Backend Setup (10 minutes)

#### Install Qt (if not installed)
```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev

# macOS
brew install qt@6

# Windows - Download from qt.io
```

#### Build Backend
```bash
cd backend
mkdir build && cd build
cmake ..
cmake --build .
./DailyReminder
```

## Project Structure Explained

### Frontend (`/frontend`)
```
src/
├── app/
│   ├── layout.tsx         # Root layout with metadata
│   ├── page.tsx           # Main page component
│   └── globals.css        # Global styles
├── components/
│   ├── TaskForm.tsx       # Add new task form
│   ├── TaskList.tsx       # Display tasks (active/completed)
│   └── TaskItem.tsx       # Individual task card
├── hooks/
│   └── useQtBridge.ts     # Qt ↔ JS communication hook
└── types/
    └── task.ts            # TypeScript interfaces
```

### Backend (`/backend`)
```
src/
├── main.cpp               # Application entry point
├── mainwindow.{h,cpp}     # Main window with QWebEngineView
├── taskmanager.{h,cpp}    # Task CRUD operations & persistence
└── qtbridge.{h,cpp}       # Qt-JavaScript bridge via WebChannel
```

## Communication Flow

```
┌─────────────────┐         ┌──────────────────┐
│   Next.js UI    │         │   Qt Backend     │
│                 │         │                  │
│  TaskForm.tsx   │──(1)───▶│  QtBridge        │
│                 │         │      ↓           │
│  useQtBridge()  │◀──(2)───│  TaskManager     │
│                 │         │      ↓           │
│  TaskList.tsx   │         │  JSON Storage    │
└─────────────────┘         └──────────────────┘

(1) JS calls: window.qtBridge.addTask({...})
(2) Qt emits: sendTasks([...]) → window.receiveTasksFromQt([...])
```

## Key Files to Customize

### Frontend
- `src/app/page.tsx` - Main UI layout
- `src/components/TaskForm.tsx` - Task input form
- `src/hooks/useQtBridge.ts` - Qt integration logic

### Backend
- `src/mainwindow.cpp` - Window size, URL loading
- `src/taskmanager.cpp` - Task business logic
- `src/qtbridge.cpp` - Exposed methods to JavaScript

## Common Commands

```bash
# Frontend
npm run dev        # Development mode
npm run build      # Production build
npm run start      # Serve production build

# Backend (CMake)
cmake --build .              # Build
cmake --build . --clean-first # Rebuild

# Backend (qmake)
qmake && make     # Build
make clean && make # Rebuild
```

## Next Steps

1. ✅ **You are here** - Project created
2. 🔄 Install dependencies
3. 🚀 Run in development mode
4. 🎨 Customize UI and features
5. 📦 Build for production
6. 🎁 Package for distribution

## Need Help?

- Check `README.md` for detailed documentation
- Qt Docs: https://doc.qt.io/
- Next.js Docs: https://nextjs.org/docs
- WebChannel: https://doc.qt.io/qt-6/qtwebchannel-index.html
