# Architecture Overview

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         Qt Application                          │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                    MainWindow (QMainWindow)               │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │          QWebEngineView                             │  │  │
│  │  │  ┌───────────────────────────────────────────────┐  │  │  │
│  │  │  │         Next.js Frontend (React/TS)           │  │  │  │
│  │  │  │  ┌──────────────────────────────────────┐     │  │  │  │
│  │  │  │  │         App Router (page.tsx)        │     │  │  │  │
│  │  │  │  │  ┌────────────┐  ┌────────────┐      │     │  │  │  │
│  │  │  │  │  │ TaskForm   │  │ TaskList   │      │     │  │  │  │
│  │  │  │  │  └────────────┘  └────────────┘      │     │  │  │  │
│  │  │  │  │          ↕                           │     │  │  │  │
│  │  │  │  │    useQtBridge() Hook                │     │  │  │  │
│  │  │  │  │          ↕                           │     │  │  │  │
│  │  │  │  │   window.qtBridge (WebChannel)       │     │  │  │  │
│  │  │  │  └──────────────────────────────────────┘     │  │  │  │
│  │  │  └───────────────────────────────────────────────┘  │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  │                           ↕                               │  │
│  │                    QWebChannel                            │  │
│  │                           ↕                               │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │              QtBridge (QObject)                     │  │  │
│  │  │  • addTask()     • updateTask()                     │  │  │
│  │  │  • deleteTask()  • markComplete()                   │  │  │
│  │  │  • getTasks()                                       │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  │                           ↕                               │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │            TaskManager (QObject)                    │  │  │
│  │  │  • Task CRUD operations                             │  │  │
│  │  │  • Data persistence (JSON)                          │  │  │
│  │  │  • Signal: tasksChanged()                           │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                              ↕
                    ┌───────────────────┐
                    │  tasks.json       │
                    │  (AppDataLocation)│
                    └───────────────────┘
```

## Data Flow

### Adding a Task

```
User Input (TaskForm)
       ↓
React State Update
       ↓
useQtBridge.addTask()
       ↓
window.qtBridge.addTask({title, description, ...})  [JavaScript]
       ↓
QWebChannel (JavaScript ↔ C++ Bridge)
       ↓
QtBridge::addTask(QJsonObject)                      [C++]
       ↓
TaskManager::addTask(title, desc, date, priority)   [C++]
       ↓
1. Create Task object
2. Assign unique ID
3. Add to m_tasks list
4. saveTasks() → tasks.json
5. emit tasksChanged()
       ↓
QtBridge::getTasks() (connected to tasksChanged)
       ↓
emit sendTasks(QJsonArray)
       ↓
QWebChannel
       ↓
window.qtBridge.sendTasks.connect(callback)         [JavaScript]
       ↓
window.receiveTasksFromQt(tasks)
       ↓
React State Update (setTasks)
       ↓
UI Re-renders with new task
```

## Component Breakdown

### Frontend Components

1. **page.tsx** (Main Page)
   - Root component
   - Manages layout
   - Integrates TaskForm and TaskList

2. **TaskForm.tsx**
   - Input form for new tasks
   - Handles: title, description, due date, priority
   - Calls `onAddTask()` from useQtBridge

3. **TaskList.tsx**
   - Displays active and completed tasks
   - Separates tasks by completion status
   - Renders TaskItem components

4. **TaskItem.tsx**
   - Individual task card
   - Checkbox for completion
   - Delete button
   - Priority and date badges

5. **useQtBridge.ts** (Custom Hook)
   - Detects Qt environment
   - Provides task operations
   - Manages task state
   - Fallback for standalone testing

### Backend Classes

1. **main.cpp**
   - Application entry point
   - Creates QApplication
   - Instantiates MainWindow

2. **MainWindow**
   - Main application window
   - Embeds QWebEngineView
   - Sets up QWebChannel
   - Loads frontend (dev or prod)

3. **QtBridge**
   - Exposes C++ methods to JavaScript
   - Receives calls from frontend
   - Emits signals to send data back
   - Thin wrapper around TaskManager

4. **TaskManager**
   - Core business logic
   - Task CRUD operations
   - JSON persistence
   - Data validation

## File Locations

### Development
- **Frontend**: http://localhost:3000
- **Backend**: Loads from localhost:3000

### Production
- **Frontend Build**: `frontend/out/`
- **Backend Loads From**: `<app-dir>/frontend/out/index.html`
- **Task Storage**: 
  - Linux: `~/.local/share/DailyReminder/tasks.json`
  - macOS: `~/Library/Application Support/DailyReminder/tasks.json`
  - Windows: `%LOCALAPPDATA%\DailyReminder\tasks.json`

## Technology Stack

### Frontend
- **Framework**: Next.js 14 (App Router)
- **UI Library**: React 18
- **Language**: TypeScript
- **Styling**: Tailwind CSS
- **Build**: Static Export

### Backend
- **Framework**: Qt 6 (or Qt 5.15+)
- **Modules**: QtCore, QtWidgets, QtWebEngineWidgets, QtWebChannel
- **Language**: C++17
- **Build System**: CMake or qmake
- **Persistence**: JSON files

### Communication
- **Bridge**: Qt WebChannel
- **Protocol**: QWebChannel JavaScript API
- **Data Format**: JSON

## Advantages of This Architecture

1. ✅ **Separation of Concerns**
   - Frontend: UI/UX only
   - Backend: Business logic & persistence

2. ✅ **Native Performance**
   - Qt handles system integration
   - Native desktop features

3. ✅ **Modern UI**
   - React ecosystem
   - Easy to style and maintain

4. ✅ **Type Safety**
   - TypeScript on frontend
   - C++ on backend

5. ✅ **Cross-Platform**
   - Works on Linux, macOS, Windows
   - Same codebase

6. ✅ **Offline First**
   - No server required
   - Local data storage

## Extending the Application

### Adding a New Feature

1. **Define TypeScript Interface** (`frontend/src/types/`)
2. **Create React Component** (`frontend/src/components/`)
3. **Add Hook Method** (`frontend/src/hooks/useQtBridge.ts`)
4. **Add C++ Method** (`backend/src/qtbridge.{h,cpp}`)
5. **Implement Logic** (`backend/src/taskmanager.{h,cpp}`)
6. **Update UI** (`frontend/src/app/page.tsx`)

### Example: Adding Task Categories

1. Update `Task` interface with `category` field
2. Add category selector in `TaskForm.tsx`
3. Add `category` parameter to `addTask()` in hook
4. Update `QtBridge::addTask()` to accept category
5. Update `TaskManager::addTask()` to store category
6. Update JSON schema
7. Display category in `TaskItem.tsx`
