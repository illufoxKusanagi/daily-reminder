# Step-by-Step Guide: Building a Qt + Next.js Application

A complete guide to create a daily task reminder using Qt (backend) and Next.js (frontend) with QWebEngine.

---

## Table of Contents
1. [Project Setup](#step-1-project-setup)
2. [Frontend Setup (Next.js)](#step-2-frontend-setup-nextjs)
3. [Backend Setup (Qt)](#step-3-backend-setup-qt)
4. [Qt-Next.js Bridge Code](#step-4-qt-nextjs-bridge-code)
5. [Building and Running](#step-5-building-and-running)
6. [Testing the Integration](#step-6-testing-the-integration)

---

## Step 1: Project Setup

### 1.1 Create Project Structure

```bash
mkdir daily-reminder
cd daily-reminder
mkdir frontend backend
```

Your folder structure should look like:
```
daily-reminder/
├── frontend/
└── backend/
```

### 1.2 Install Prerequisites

**Node.js (for Next.js):**
```bash
# Check if installed
node --version  # Should be 18+

# If not installed (Ubuntu/Debian)
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

# Or use nvm (recommended)
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
nvm install 18
nvm use 18
```

**Qt (for Backend):**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev libqt6webchannel6-dev cmake build-essential

# macOS
brew install qt@6 cmake

# Windows
# Download from: https://www.qt.io/download
# Select Qt 6.x with WebEngine and WebChannel modules
```

---

## Step 2: Frontend Setup (Next.js)

### 2.1 Initialize Next.js Project

```bash
cd frontend
npx create-next-app@latest . --typescript --tailwind --app --no-src-dir --import-alias "@/*"
```

Answer the prompts:
- ✔ Would you like to use TypeScript? **Yes**
- ✔ Would you like to use ESLint? **Yes**
- ✔ Would you like to use Tailwind CSS? **Yes**
- ✔ Would you like to use `src/` directory? **No**
- ✔ Would you like to use App Router? **Yes**
- ✔ Would you like to customize the default import alias? **No**

### 2.2 Configure for Static Export

Edit `next.config.js`:
```javascript
/** @type {import('next').NextConfig} */
const nextConfig = {
  output: 'export',
  distDir: 'out',
  images: {
    unoptimized: true,
  },
}

module.exports = nextConfig
```

### 2.3 Create TypeScript Types

Create `types/task.ts`:
```typescript
export interface Task {
  id: string;
  title: string;
  description: string;
  dueDate: string;
  completed: boolean;
  priority: 'low' | 'medium' | 'high';
  createdAt: string;
}

export interface QtBridge {
  addTask: (task: Omit<Task, 'id' | 'createdAt'>) => void;
  updateTask: (id: string, task: Partial<Task>) => void;
  deleteTask: (id: string) => void;
  getTasks: () => void;
  markComplete: (id: string, completed: boolean) => void;
}

declare global {
  interface Window {
    qtBridge?: QtBridge;
    receiveTasksFromQt?: (tasks: Task[]) => void;
  }
}
```

### 2.4 Create Qt Bridge Hook

Create `hooks/useQtBridge.ts`:
```typescript
'use client';

import { useEffect, useState } from 'react';
import { Task } from '@/types/task';

export const useQtBridge = () => {
  const [tasks, setTasks] = useState<Task[]>([]);
  const [isQtAvailable, setIsQtAvailable] = useState(false);

  useEffect(() => {
    // Check if running inside Qt WebEngine
    setIsQtAvailable(typeof window.qtBridge !== 'undefined');

    // Set up callback for receiving tasks from Qt
    window.receiveTasksFromQt = (receivedTasks: Task[]) => {
      console.log('Received tasks from Qt:', receivedTasks);
      setTasks(receivedTasks);
    };

    // Request initial tasks from Qt
    if (window.qtBridge) {
      window.qtBridge.getTasks();
    } else {
      console.log('Qt bridge not available - running in standalone mode');
    }

    return () => {
      delete window.receiveTasksFromQt;
    };
  }, []);

  const addTask = (task: Omit<Task, 'id' | 'createdAt'>) => {
    if (window.qtBridge) {
      console.log('Sending task to Qt:', task);
      window.qtBridge.addTask(task);
    } else {
      // Fallback for development without Qt
      const newTask: Task = {
        ...task,
        id: Date.now().toString(),
        createdAt: new Date().toISOString(),
      };
      setTasks([...tasks, newTask]);
    }
  };

  const updateTask = (id: string, updates: Partial<Task>) => {
    if (window.qtBridge) {
      window.qtBridge.updateTask(id, updates);
    } else {
      setTasks(tasks.map(task => task.id === id ? { ...task, ...updates } : task));
    }
  };

  const deleteTask = (id: string) => {
    if (window.qtBridge) {
      window.qtBridge.deleteTask(id);
    } else {
      setTasks(tasks.filter(task => task.id !== id));
    }
  };

  const markComplete = (id: string, completed: boolean) => {
    if (window.qtBridge) {
      window.qtBridge.markComplete(id, completed);
    } else {
      setTasks(tasks.map(task => task.id === id ? { ...task, completed } : task));
    }
  };

  return {
    tasks,
    addTask,
    updateTask,
    deleteTask,
    markComplete,
    isQtAvailable,
  };
};
```

### 2.5 Create UI Components

Create `components/TaskForm.tsx`:
```typescript
'use client';

import { useState } from 'react';
import { Task } from '@/types/task';

interface TaskFormProps {
  onAddTask: (task: Omit<Task, 'id' | 'createdAt'>) => void;
}

export default function TaskForm({ onAddTask }: TaskFormProps) {
  const [title, setTitle] = useState('');
  const [description, setDescription] = useState('');
  const [dueDate, setDueDate] = useState('');
  const [priority, setPriority] = useState<'low' | 'medium' | 'high'>('medium');

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!title.trim()) return;

    onAddTask({
      title,
      description,
      dueDate: dueDate || new Date().toISOString(),
      completed: false,
      priority,
    });

    setTitle('');
    setDescription('');
    setDueDate('');
    setPriority('medium');
  };

  return (
    <div className="bg-white dark:bg-gray-800 rounded-lg shadow-lg p-6">
      <h2 className="text-2xl font-semibold mb-4 text-gray-800 dark:text-white">
        Add New Task
      </h2>
      <form onSubmit={handleSubmit} className="space-y-4">
        <div>
          <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            Title *
          </label>
          <input
            type="text"
            value={title}
            onChange={(e) => setTitle(e.target.value)}
            className="w-full px-3 py-2 border border-gray-300 rounded-md dark:bg-gray-700 dark:border-gray-600 dark:text-white"
            required
          />
        </div>
        <div>
          <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            Description
          </label>
          <textarea
            value={description}
            onChange={(e) => setDescription(e.target.value)}
            rows={3}
            className="w-full px-3 py-2 border border-gray-300 rounded-md dark:bg-gray-700 dark:border-gray-600 dark:text-white"
          />
        </div>
        <div>
          <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            Due Date
          </label>
          <input
            type="date"
            value={dueDate}
            onChange={(e) => setDueDate(e.target.value)}
            className="w-full px-3 py-2 border border-gray-300 rounded-md dark:bg-gray-700 dark:border-gray-600 dark:text-white"
          />
        </div>
        <div>
          <label className="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-1">
            Priority
          </label>
          <select
            value={priority}
            onChange={(e) => setPriority(e.target.value as 'low' | 'medium' | 'high')}
            className="w-full px-3 py-2 border border-gray-300 rounded-md dark:bg-gray-700 dark:border-gray-600 dark:text-white"
          >
            <option value="low">Low</option>
            <option value="medium">Medium</option>
            <option value="high">High</option>
          </select>
        </div>
        <button
          type="submit"
          className="w-full bg-blue-600 hover:bg-blue-700 text-white font-semibold py-2 px-4 rounded-md"
        >
          Add Task
        </button>
      </form>
    </div>
  );
}
```

Create `app/page.tsx`:
```typescript
'use client';

import { useQtBridge } from '@/hooks/useQtBridge';
import TaskForm from '@/components/TaskForm';

export default function Home() {
  const { tasks, addTask, deleteTask, markComplete, isQtAvailable } = useQtBridge();

  return (
    <main className="min-h-screen p-8 bg-gradient-to-br from-blue-50 to-indigo-100">
      <div className="max-w-4xl mx-auto">
        <header className="mb-8">
          <h1 className="text-4xl font-bold text-gray-800 mb-2">
            Daily Task Reminder
          </h1>
          <p className="text-gray-600">
            Stay organized and productive
            {isQtAvailable && (
              <span className="ml-2 text-xs bg-green-500 text-white px-2 py-1 rounded">
                Qt Connected ✓
              </span>
            )}
          </p>
        </header>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          <div className="lg:col-span-1">
            <TaskForm onAddTask={addTask} />
          </div>
          
          <div className="lg:col-span-2">
            <div className="bg-white rounded-lg shadow-lg p-6">
              <h2 className="text-2xl font-semibold mb-4">Tasks ({tasks.length})</h2>
              {tasks.length === 0 ? (
                <p className="text-gray-500 text-center py-8">No tasks yet. Add one to get started!</p>
              ) : (
                <div className="space-y-3">
                  {tasks.map(task => (
                    <div key={task.id} className="border-l-4 border-blue-500 bg-gray-50 p-4 rounded">
                      <div className="flex items-start justify-between">
                        <div className="flex items-start space-x-3">
                          <input
                            type="checkbox"
                            checked={task.completed}
                            onChange={(e) => markComplete(task.id, e.target.checked)}
                            className="mt-1 h-5 w-5"
                          />
                          <div>
                            <h3 className={`font-semibold ${task.completed ? 'line-through' : ''}`}>
                              {task.title}
                            </h3>
                            {task.description && <p className="text-sm text-gray-600">{task.description}</p>}
                            <span className={`text-xs px-2 py-1 rounded ${
                              task.priority === 'high' ? 'bg-red-100 text-red-800' :
                              task.priority === 'medium' ? 'bg-yellow-100 text-yellow-800' :
                              'bg-green-100 text-green-800'
                            }`}>
                              {task.priority}
                            </span>
                          </div>
                        </div>
                        <button
                          onClick={() => deleteTask(task.id)}
                          className="text-red-500 hover:text-red-700"
                        >
                          Delete
                        </button>
                      </div>
                    </div>
                  ))}
                </div>
              )}
            </div>
          </div>
        </div>
      </div>
    </main>
  );
}
```

### 2.6 Test Frontend Standalone

```bash
cd frontend
npm install
npm run dev
```

Open http://localhost:3000 - you should see the UI (without Qt, it will use fallback mode).

---

## Step 3: Backend Setup (Qt)

### 3.1 Create Qt Project Files

Navigate to backend directory:
```bash
cd ../backend
mkdir src
```

### 3.2 Create CMakeLists.txt

Create `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.16)
project(DailyReminder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt
find_package(Qt6 COMPONENTS Core Widgets WebEngineWidgets WebChannel QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets WebEngineWidgets WebChannel)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/taskmanager.cpp
    src/qtbridge.cpp
)

set(HEADERS
    src/mainwindow.h
    src/taskmanager.h
    src/qtbridge.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets Qt6::WebChannel
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets Qt5::WebChannel
    )
endif()
```

---

## Step 4: Qt-Next.js Bridge Code

This is the **essential bridge** that connects Qt and Next.js!

### 4.1 Task Manager (Data Layer)

Create `src/taskmanager.h`:
```cpp
#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QList>

struct Task {
    QString id;
    QString title;
    QString description;
    QString dueDate;
    bool completed;
    QString priority;
    QString createdAt;
    
    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject& json);
};

class TaskManager : public QObject
{
    Q_OBJECT
    
public:
    explicit TaskManager(QObject *parent = nullptr);
    
    QString addTask(const QString& title, const QString& description, 
                   const QString& dueDate, const QString& priority);
    bool deleteTask(const QString& id);
    bool markComplete(const QString& id, bool completed);
    QJsonArray getTasksAsJson() const;
    
signals:
    void tasksChanged();
    
private:
    QList<Task> m_tasks;
    QString m_dataFilePath;
    
    void loadTasks();
    void saveTasks();
    QString generateId() const;
};

#endif
```

Create `src/taskmanager.cpp`:
```cpp
#include "taskmanager.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>

QJsonObject Task::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["title"] = title;
    obj["description"] = description;
    obj["dueDate"] = dueDate;
    obj["completed"] = completed;
    obj["priority"] = priority;
    obj["createdAt"] = createdAt;
    return obj;
}

Task Task::fromJson(const QJsonObject& json) {
    Task task;
    task.id = json["id"].toString();
    task.title = json["title"].toString();
    task.description = json["description"].toString();
    task.dueDate = json["dueDate"].toString();
    task.completed = json["completed"].toBool();
    task.priority = json["priority"].toString();
    task.createdAt = json["createdAt"].toString();
    return task;
}

TaskManager::TaskManager(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_dataFilePath = dataDir + "/tasks.json";
    loadTasks();
}

QString TaskManager::addTask(const QString& title, const QString& description,
                             const QString& dueDate, const QString& priority)
{
    Task task;
    task.id = generateId();
    task.title = title;
    task.description = description;
    task.dueDate = dueDate.isEmpty() ? QDateTime::currentDateTime().toString(Qt::ISODate) : dueDate;
    task.completed = false;
    task.priority = priority.isEmpty() ? "medium" : priority;
    task.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    m_tasks.append(task);
    saveTasks();
    emit tasksChanged();
    return task.id;
}

bool TaskManager::deleteTask(const QString& id)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            m_tasks.removeAt(i);
            saveTasks();
            emit tasksChanged();
            return true;
        }
    }
    return false;
}

bool TaskManager::markComplete(const QString& id, bool completed)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            m_tasks[i].completed = completed;
            saveTasks();
            emit tasksChanged();
            return true;
        }
    }
    return false;
}

QJsonArray TaskManager::getTasksAsJson() const
{
    QJsonArray arr;
    for (const Task& task : m_tasks) {
        arr.append(task.toJson());
    }
    return arr;
}

void TaskManager::loadTasks()
{
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No existing tasks file. Starting fresh.";
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        QJsonArray arr = doc.array();
        for (const QJsonValue& val : arr) {
            if (val.isObject()) {
                m_tasks.append(Task::fromJson(val.toObject()));
            }
        }
    }
    qDebug() << "Loaded" << m_tasks.size() << "tasks from" << m_dataFilePath;
}

void TaskManager::saveTasks()
{
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "ERROR: Could not save tasks!";
        return;
    }
    
    QJsonDocument doc(getTasksAsJson());
    file.write(doc.toJson());
    file.close();
}

QString TaskManager::generateId() const
{
    return QString::number(QDateTime::currentMSecsSinceEpoch());
}
```

### 4.2 Qt Bridge (JavaScript ↔ C++ Connection)

**This is the KEY component for Qt-Next.js communication!**

Create `src/qtbridge.h`:
```cpp
#ifndef QTBRIDGE_H
#define QTBRIDGE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class TaskManager;

class QtBridge : public QObject
{
    Q_OBJECT
    
public:
    explicit QtBridge(TaskManager *taskManager, QObject *parent = nullptr);
    
public slots:
    // These methods are called FROM JavaScript
    void addTask(const QJsonObject& taskData);
    void deleteTask(const QString& id);
    void getTasks();
    void markComplete(const QString& id, bool completed);
    
signals:
    // These signals send data TO JavaScript
    void sendTasks(const QJsonArray& tasks);
    
private:
    TaskManager *m_taskManager;
};

#endif
```

Create `src/qtbridge.cpp`:
```cpp
#include "qtbridge.h"
#include "taskmanager.h"
#include <QDebug>

QtBridge::QtBridge(TaskManager *taskManager, QObject *parent)
    : QObject(parent)
    , m_taskManager(taskManager)
{
    // When tasks change in TaskManager, automatically send update to JavaScript
    connect(m_taskManager, &TaskManager::tasksChanged, this, &QtBridge::getTasks);
}

void QtBridge::addTask(const QJsonObject& taskData)
{
    qDebug() << "QtBridge: addTask called from JavaScript with data:" << taskData;
    
    QString title = taskData["title"].toString();
    QString description = taskData["description"].toString();
    QString dueDate = taskData["dueDate"].toString();
    QString priority = taskData["priority"].toString();
    
    QString id = m_taskManager->addTask(title, description, dueDate, priority);
    qDebug() << "QtBridge: Task added with ID:" << id;
}

void QtBridge::deleteTask(const QString& id)
{
    qDebug() << "QtBridge: deleteTask called for ID:" << id;
    m_taskManager->deleteTask(id);
}

void QtBridge::getTasks()
{
    QJsonArray tasks = m_taskManager->getTasksAsJson();
    qDebug() << "QtBridge: Sending" << tasks.size() << "tasks to JavaScript";
    emit sendTasks(tasks);
}

void QtBridge::markComplete(const QString& id, bool completed)
{
    qDebug() << "QtBridge: markComplete called - ID:" << id << "Completed:" << completed;
    m_taskManager->markComplete(id, completed);
}
```

### 4.3 Main Window (WebEngine Setup)

Create `src/mainwindow.h`:
```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>

class TaskManager;
class QtBridge;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    void setupWebView();
    void setupWebChannel();
    void injectQtBridge();
    
    QWebEngineView *m_webView;
    QWebChannel *m_webChannel;
    TaskManager *m_taskManager;
    QtBridge *m_qtBridge;
};

#endif
```

Create `src/mainwindow.cpp`:
```cpp
#include "mainwindow.h"
#include "taskmanager.h"
#include "qtbridge.h"
#include <QWebEngineView>
#include <QWebChannel>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QUrl>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Daily Task Reminder");
    resize(1200, 800);
    
    // Initialize backend
    m_taskManager = new TaskManager(this);
    
    // Setup UI and bridge
    setupWebView();
    setupWebChannel();
    injectQtBridge();
}

MainWindow::~MainWindow() {}

void MainWindow::setupWebView()
{
    m_webView = new QWebEngineView(this);
    setCentralWidget(m_webView);
    
    // DEVELOPMENT MODE: Load from Next.js dev server
    // m_webView->load(QUrl("http://localhost:3000"));
    
    // PRODUCTION MODE: Load from exported static files
    QString htmlPath = QCoreApplication::applicationDirPath() + "/frontend/out/index.html";
    
    if (QFile::exists(htmlPath)) {
        qDebug() << "Loading frontend from:" << htmlPath;
        m_webView->load(QUrl::fromLocalFile(htmlPath));
    } else {
        qDebug() << "Production build not found. Trying development server...";
        m_webView->load(QUrl("http://localhost:3000"));
    }
}

void MainWindow::setupWebChannel()
{
    // Create WebChannel for JavaScript-C++ communication
    m_webChannel = new QWebChannel(this);
    
    // Create bridge object
    m_qtBridge = new QtBridge(m_taskManager, this);
    
    // Register the bridge object (accessible in JS as window.qtBridge)
    m_webChannel->registerObject(QStringLiteral("qtBridge"), m_qtBridge);
    
    // Set the channel on the page
    m_webView->page()->setWebChannel(m_webChannel);
    
    qDebug() << "WebChannel setup complete - qtBridge registered";
}

void MainWindow::injectQtBridge()
{
    // Inject qwebchannel.js initialization script
    QString script = R"(
        new QWebChannel(qt.webChannelTransport, function(channel) {
            console.log('QWebChannel initialized!');
            window.qtBridge = channel.objects.qtBridge;
            
            // Connect Qt signals to JavaScript callbacks
            window.qtBridge.sendTasks.connect(function(tasks) {
                console.log('Received tasks from Qt:', tasks);
                if (window.receiveTasksFromQt) {
                    window.receiveTasksFromQt(tasks);
                }
            });
            
            console.log('Qt Bridge ready! Available methods:', Object.keys(window.qtBridge));
            
            // Request initial tasks
            window.qtBridge.getTasks();
        });
    )";
    
    QWebEngineScript webChannelScript;
    webChannelScript.setName("qwebchannel_init");
    webChannelScript.setSourceCode(script);
    webChannelScript.setInjectionPoint(QWebEngineScript::DocumentReady);
    webChannelScript.setWorldId(QWebEngineScript::MainWorld);
    webChannelScript.setRunsOnSubFrames(false);
    
    m_webView->page()->scripts().insert(webChannelScript);
    
    qDebug() << "Qt Bridge injection script added";
}
```

### 4.4 Main Entry Point

Create `src/main.cpp`:
```cpp
#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    
    // Set application metadata
    QCoreApplication::setOrganizationName("DailyReminder");
    QCoreApplication::setApplicationName("Daily Task Reminder");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    qDebug() << "Starting Daily Task Reminder...";
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
```

---

## Step 5: Building and Running

### 5.1 Build Frontend (Production)

```bash
cd frontend
npm run build
```

This creates `frontend/out/` directory with static files.

### 5.2 Build Qt Backend

```bash
cd ../backend
mkdir build
cd build
cmake ..
cmake --build .
```

### 5.3 Copy Frontend to Backend

```bash
# From backend/build directory
mkdir -p frontend/out
cp -r ../../frontend/out/* frontend/out/
```

### 5.4 Run the Application

```bash
# From backend/build directory
./DailyReminder
```

---

## Step 6: Testing the Integration

### 6.1 Development Mode Testing

**Terminal 1 - Start Next.js:**
```bash
cd frontend
npm run dev
```

**Terminal 2 - Edit mainwindow.cpp:**
```cpp
// In setupWebView(), change to:
m_webView->load(QUrl("http://localhost:3000"));
```

**Build and run Qt:**
```bash
cd backend/build
cmake --build .
./DailyReminder
```

### 6.2 Verify Qt Bridge Connection

Open the Qt application and press **F12** (if remote debugging is enabled) or check the terminal output.

You should see:
```
QWebChannel initialized!
Qt Bridge ready! Available methods: ["addTask", "deleteTask", "getTasks", "markComplete"]
Received tasks from Qt: []
```

In the UI, you should see:
- Green badge: "Qt Connected ✓"

### 6.3 Test Task Operations

1. **Add a task:**
   - Fill in title: "Test Task"
   - Click "Add Task"
   - Check terminal: `QtBridge: addTask called from JavaScript`
   - Task should appear in the list

2. **Mark complete:**
   - Check the checkbox
   - Check terminal: `QtBridge: markComplete called`

3. **Delete task:**
   - Click "Delete"
   - Check terminal: `QtBridge: deleteTask called`

4. **Check persistence:**
   - Close application
   - Reopen application
   - Tasks should still be there
   - Check data file:
     ```bash
     cat ~/.local/share/DailyReminder/tasks.json
     ```

---

## Understanding the Bridge

### How Qt → JavaScript Works:

```cpp
// C++ (QtBridge)
emit sendTasks(tasks);  // Emit signal
```
↓
```javascript
// JavaScript (useQtBridge.ts)
window.qtBridge.sendTasks.connect(function(tasks) {
    window.receiveTasksFromQt(tasks);  // Callback
});
```

### How JavaScript → Qt Works:

```javascript
// JavaScript (useQtBridge.ts)
window.qtBridge.addTask(taskData);  // Call method
```
↓
```cpp
// C++ (QtBridge)
void QtBridge::addTask(const QJsonObject& taskData) {
    // Method executed
}
```

---

## Common Folder Structure

```
daily-reminder/
├── frontend/
│   ├── app/
│   │   ├── page.tsx
│   │   └── layout.tsx
│   ├── components/
│   │   └── TaskForm.tsx
│   ├── hooks/
│   │   └── useQtBridge.ts
│   ├── types/
│   │   └── task.ts
│   ├── package.json
│   └── next.config.js
│
└── backend/
    ├── src/
    │   ├── main.cpp
    │   ├── mainwindow.{h,cpp}
    │   ├── taskmanager.{h,cpp}
    │   └── qtbridge.{h,cpp}      ← THE BRIDGE!
    ├── build/
    │   ├── DailyReminder         ← Executable
    │   └── frontend/out/         ← Copied Next.js build
    └── CMakeLists.txt
```

---

## Troubleshooting

### Qt Bridge Not Available

**Symptom:** No "Qt Connected" badge, `window.qtBridge` is undefined

**Solutions:**
1. Check Qt debug output
2. Verify WebChannel is registered:
   ```cpp
   qDebug() << "Registering qtBridge";
   m_webChannel->registerObject("qtBridge", m_qtBridge);
   ```
3. Ensure injection script runs (check `injectQtBridge()`)

### Tasks Not Persisting

**Symptom:** Tasks disappear on restart

**Check:**
```bash
# Verify data file exists
ls -la ~/.local/share/DailyReminder/tasks.json

# View contents
cat ~/.local/share/DailyReminder/tasks.json
```

### Blank Screen in Qt

**Symptom:** Qt window is blank/white

**Solutions:**
1. Check console for errors (enable remote debugging)
2. Verify frontend path is correct
3. Try loading dev server instead: `http://localhost:3000`

---

## Next Steps

1. ✅ **Basic app is working**
2. Add more features (edit tasks, categories, search)
3. Improve UI styling
4. Add system tray integration
5. Add notifications
6. Package for distribution

---

## Summary

You now have a complete Qt + Next.js application with:
- ✅ Next.js frontend with TypeScript
- ✅ Qt backend with C++
- ✅ **QWebChannel bridge** connecting them
- ✅ Task CRUD operations
- ✅ Data persistence
- ✅ Beautiful UI

The **key to Qt-Next.js integration** is:
1. **QtBridge class** - exposes C++ methods to JavaScript
2. **QWebChannel** - the communication protocol
3. **window.qtBridge** - JavaScript interface to C++ backend
4. **Signals & Slots** - Qt's event system for callbacks

Happy coding! 🚀
