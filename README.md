# Daily Reminder

A simple and beautiful desktop calendar application to help you manage your daily activities and never miss important events.

## 📥 Download & Install

**Get the latest version:** [Download Daily Reminder v0.3.5](https://github.com/illufoxKusanagi/daily-reminder/releases/tag/0.3.5)

### Windows
1. Download `DailyReminder-Windows-x64.zip`
2. Extract the ZIP file to a folder
3. Run `DailyReminder.bat` or `backend.exe`
4. The app will open automatically

### Linux (RPM - Fedora/RHEL/CentOS)
1. Download `daily-reminder-*.rpm`
2. Install with:
   ```bash
   sudo rpm -ivh daily-reminder-*.rpm
   ```
3. Launch from your application menu or run `daily-reminder` in terminal

### Linux (DEB - Ubuntu/Debian)
1. Download `daily-reminder_*.deb`
2. Install with:
   ```bash
   sudo dpkg -i daily-reminder_*.deb
   ```
3. Launch from your application menu or run `daily-reminder` in terminal

> **Note:** Linux requires `notify-send` for notifications (usually pre-installed)

## ✨ What is Daily Reminder?

Daily Reminder is a desktop application that helps you keep track of your schedule with:

- **📅 Visual Calendar** - See all your events in a clean monthly view
- **🎨 Color Categories** - Organize events with customizable colored categories (Work, Personal, Health, etc.)
- **⏰ Smart Reminders** - Get desktop notifications before your events start
- **🌓 Dark/Light Mode** - Choose the theme that suits your preference
- **💾 Local Storage** - All your data stays on your computer, completely private

## 🎯 How to Use

### Creating Your First Event

1. **Click any date** on the calendar to create a new event
2. Fill in the details:
   - **Title**: What's the event?
   - **Category**: Choose or create a category (Work, Personal, etc.)
   - **Date & Time**: When does it start and end?
   - **Description**: Add any notes (optional)
   - **Reminder**: Set when you want to be notified
3. Click **Save** - that's it!

### Managing Events

- **View**: Click any event on the calendar to see full details
- **Edit**: Click the edit button to change event information
- **Delete**: Remove events you no longer need
- **Categories**: Create custom categories with your favorite colors

### Setting Up Reminders

1. When creating/editing an event, toggle **Enable Reminder**
2. Choose how long before the event you want to be notified
3. You'll get a desktop notification at that time!

## 🎨 Features

- ✅ **Easy to Use** - Intuitive interface, no learning curve
- ✅ **Fast & Lightweight** - Runs smoothly on your computer
- ✅ **Privacy First** - All data stored locally, no internet required
- ✅ **Cross-Platform** - Works on Windows and Linux
- ✅ **System Tray** - Runs quietly in the background
- ✅ **No Ads** - Completely free and open-source

## 🖥️ System Requirements

- **Windows**: Windows 10 or later
- **Linux**: Any modern distribution (Fedora, Ubuntu, Debian, etc.)
- **Memory**: 100MB RAM
- **Storage**: 50MB disk space

## ❓ Troubleshooting

**App won't start?**
- Make sure you have the required dependencies installed
- On Linux, check if `libnotify` is installed

**Notifications not working?**
- Linux: Install `libnotify-bin` package
- Windows: Check your Windows notification settings

**Can't see the calendar?**
- Try refreshing by restarting the app
- Check if the app is running (look for it in system tray)

## 💡 Tips & Tricks

- Use **different colors** for different types of activities
- Set **recurring reminders** for daily tasks
- Use the **system tray icon** to quickly open the app
- **Dark mode** is easier on the eyes for night use

## 🆘 Need Help?

Found a bug or have a suggestion? [Open an issue on GitHub](https://github.com/illufoxKusanagi/daily-reminder/issues)

---

## 🛠️ For Developers
- ✅ **Categories**: Work, Personal, Health, Shopping, Exercise, Study, Social, Home, Other
- ✅ **Reminders**: Set reminder times with enable/disable toggle
- ✅ **Alarm/Reminder System**: Qt-based timer that checks every 30 seconds and triggers system notifications
- ✅ **Date/Time Pickers**: Intuitive datetime selection
- ✅ **Persistent Storage**: SQLite database for data persistence
- ✅ **REST API**: Clean HTTP API for all operations
- ✅ **Desktop App**: Qt-based native desktop application

## 🗂️ Project Structure

```
daily-reminder/
├── frontend/                    # Next.js Frontend (localhost:3000 in dev)
│   ├── src/
│   │   ├── app/                # Next.js 14 App Router
│   │   │   ├── page.tsx       # Main calendar page
│   │   │   └── layout.tsx     # Root layout
│   │   ├── components/
│   │   │   └── ui/            # shadcn/ui components
│   │   └── modules/
│   │       └── calendar/      # Calendar module
│   │           ├── components/
│   │           │   ├── calendar.tsx           # Main calendar component
│   │           │   ├── color-picker.tsx       # Event color selector
│   │           │   └── event-tooltip.tsx      # Event hover tooltips
│   │           ├── dialogs/
│   │           │   └── add-edit-event-dialog.tsx  # Event form dialog
│   │           ├── contexts/
│   │           │   └── calendar-context.tsx   # Calendar state management
│   │           ├── hooks/
│   │           │   └── use-calendar.ts        # Calendar operations hook
│   │           ├── interfaces.ts              # TypeScript types (IEvent)
│   │           ├── schemas.ts                 # Zod validation schemas
│   │           └── requests.ts                # API client functions
│   ├── package.json
│   └── next.config.js
│
├── backend/                     # Qt Backend (localhost:8080)
│   ├── main.cpp                # Application entry point
│   ├── mainwindow.h/cpp        # Main window with QWebEngineView
│   ├── httpserver.h/cpp        # QHttpServer REST API endpoints
│   ├── database.h/cpp          # SQLite database operations
│   ├── CMakeLists.txt          # Qt/CMake build configuration
│   └── build/                  # Build output directory
│
├── scripts/
│   ├── seed-activities.sh      # Seed sample events to database
│   └── test-api.sh             # Test API endpoints with curl
│
├── docs/                       # Documentation
│   ├── API.md                  # REST API reference
│   └── ARCHITECTURE.md         # System architecture details
│
└── README.md                   # This file
```

## 📡 REST API Endpoints

All API endpoints are served by the Qt backend on `http://localhost:8080`.

### **GET /api/event** - Get All Events

**Response:**
```json
[
  {
    "id": 1,
    "category": "Work",
    "title": "Team Meeting",
    "description": "Weekly team sync",
    "startDate": "2025-11-02T10:00:00",
    "endDate": "2025-11-02T11:00:00",
    "color": "blue",
    "reminderTime": "2025-11-02T09:45:00",
    "isReminderEnabled": true
  }
]
```

### **POST /api/event** - Create New Event

**Request:**
```json
{
  "category": "Personal",
  "title": "Dentist Appointment",
  "description": "Regular checkup",
  "startDate": "2025-11-05T14:00:00",
  "endDate": "2025-11-05T15:00:00",
  "color": "green",
  "reminderTime": "2025-11-05T13:30:00",
  "isReminderEnabled": true
}
```

**Response:**
```json
{
  "id": 2,
  "category": "Personal",
  "title": "Dentist Appointment",
  "description": "Regular checkup",
  "startDate": "2025-11-05T14:00:00",
  "endDate": "2025-11-05T15:00:00",
  "color": "green",
  "reminderTime": "2025-11-05T13:30:00",
  "isReminderEnabled": true
}
```

### **PUT /api/event/:id** - Update Event

**Request:**
```json
{
  "title": "Updated Title",
  "description": "Updated description",
  "color": "red"
}
```

**Response:**
```json
{
  "id": 2,
  "category": "Personal",
  "title": "Updated Title",
  "description": "Updated description",
  "startDate": "2025-11-05T14:00:00",
  "endDate": "2025-11-05T15:00:00",
  "color": "red",
  "reminderTime": "2025-11-05T13:30:00",
  "isReminderEnabled": true
}
```

### **DELETE /api/event/:id** - Delete Event

**Response:**
```json
{
  "message": "Event deleted successfully"
}
```

## 🔧 Prerequisites

### Frontend Requirements
- Node.js 18+
- npm/yarn/pnpm

### Backend Requirements
- Qt 6.5+ (with HttpServer, WebEngine, and Sql modules)
- CMake 3.16+
- C++17 compiler (GCC, Clang, or MSVC)
- SQLite (included with Qt)
- **Linux**: `notify-send` (libnotify) and `paplay` or `aplay` for sound
- **macOS**: Built-in notification system
- **Windows**: QSystemTrayIcon for notifications

## 🚀 Quick Start

### 1. Install Qt 6

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev qt6-httpserver-dev libnotify-bin
```

**macOS:**
```bash
brew install qt@6
```
⚠️ Note: Desktop notifications may not work on macOS.

**Windows:**
- Download Qt from https://www.qt.io/download
- Install Qt with WebEngine and HttpServer modules
- Ensure PowerShell is available (default in Windows 10/11)

### 2. Install Node.js
```bash
# Using nvm (recommended)
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.0/install.sh | bash
nvm install 18
nvm use 18

# Or download from: https://nodejs.org/
```

### 3. Clone and Setup Frontend

```bash
cd frontend

# Install dependencies
npm install

# Run development server
npm run dev
```

The frontend will be available at `http://localhost:3000`

### 4. Build and Run Qt Backend

```bash
cd backend
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./DailyReminder
```

The backend API will start on `http://localhost:8080` and automatically load the frontend.

## 📝 How to Use the API

### From Frontend (JavaScript/TypeScript)

The frontend uses a centralized API client in `frontend/src/modules/calendar/requests.ts`:

```typescript
// Get all events
import { getEvents, createEvent, updateEvent, deleteEvent } from '@/modules/calendar/requests';

// Fetch events
const events = await getEvents();

// Create new event
const newEvent = await createEvent({
  category: "Work",
  title: "Team Meeting",
  description: "Weekly sync",
  startDate: "2025-11-02T10:00:00",
  endDate: "2025-11-02T11:00:00",
  color: "blue",
  reminderTime: "2025-11-02T09:45:00",
  isReminderEnabled: true
});

// Update event
const updated = await updateEvent("1", {
  title: "Updated Meeting Title"
});

// Delete event
await deleteEvent("1");
```

### From Command Line (curl)

```bash
# Get all events
curl http://localhost:8080/api/event

# Create new event
curl -X POST http://localhost:8080/api/event \
  -H "Content-Type: application/json" \
  -d '{
    "category": "Personal",
    "title": "Doctor Appointment",
    "description": "Annual checkup",
    "startDate": "2025-11-03T14:00:00",
    "endDate": "2025-11-03T15:00:00",
    "color": "green",
    "reminderTime": "2025-11-03T13:30:00",
    "isReminderEnabled": true
  }'

# Update event (replace :id with actual event ID)
curl -X PUT http://localhost:8080/api/event/1 \
  -H "Content-Type: application/json" \
  -d '{
    "title": "Updated Title",
    "color": "red"
  }'

# Delete event
curl -X DELETE http://localhost:8080/api/event/1
```

## 🗄️ Database Schema

The SQLite database (`events.db`) has the following structure:

```sql
CREATE TABLE events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    category TEXT NOT NULL,
    title TEXT NOT NULL,
    description TEXT,
    start_date TEXT NOT NULL,
    end_date TEXT NOT NULL,
    color TEXT NOT NULL,
    reminder_time TEXT,
    is_reminder_enabled INTEGER DEFAULT 0
);
```

**Field Descriptions:**
- `id`: Auto-incrementing primary key
- `category`: Event category (Work, Personal, Health, etc.)
- `title`: Event title (required)
- `description`: Optional event description
- `start_date`: ISO 8601 datetime string (e.g., "2025-11-02T10:00:00")
- `end_date`: ISO 8601 datetime string
- `color`: Event color (blue, green, red, yellow, purple, orange)
- `reminder_time`: ISO 8601 datetime string for reminder
- `is_reminder_enabled`: Boolean (0 or 1)

## 💻 Development Workflow

### Working on Frontend Only

```bash
cd frontend
npm run dev
```

The frontend runs independently on `http://localhost:3000` and connects to the Qt backend API at `http://localhost:8080`.

### Working on Backend Only

```bash
cd backend/build
./DailyReminder
```

Test the API directly with curl or Postman without the frontend.

### Full Stack Development

1. **Terminal 1** - Start Qt backend:
   ```bash
   cd backend/build
   ./DailyReminder
   ```

2. **Terminal 2** - Start Next.js frontend:
   ```bash
   cd frontend
   npm run dev
   ```

3. Open `http://localhost:3000` in your browser to see the app
4. The Qt app will also load the frontend in its embedded browser

## 🔍 Data Flow Example

### Creating an Event (Step by Step)

1. **User Action**: Click "Add Event" button in calendar
2. **Frontend**: Opens dialog with form
3. **User Input**: Fills in title, category, dates, etc.
4. **Frontend Validation**: Zod schema validates input
5. **API Call**: `POST http://localhost:8080/api/event` with JSON body
6. **Qt Backend**: Receives request in `HttpServer::handlePostEvent()`
7. **Database**: Inserts new row into `events` table
8. **Response**: Returns JSON with new event (including generated ID)
9. **Frontend Update**: Adds event to calendar state
10. **UI Refresh**: Calendar displays new event

### Updating an Event

1. **User Action**: Click existing event, modify in dialog
2. **API Call**: `PUT http://localhost:8080/api/event/1` with changed fields
3. **Qt Backend**: Updates row in database
4. **Response**: Returns updated event JSON
5. **Frontend**: Updates event in calendar state and UI

### Deleting an Event

1. **User Action**: Click delete button on event
2. **API Call**: `DELETE http://localhost:8080/api/event/1`
3. **Qt Backend**: Removes row from database
4. **Response**: Returns success message
5. **Frontend**: Removes event from calendar state and UI

## 🎨 Frontend Components

### Calendar Context (`calendar-context.tsx`)
Manages global calendar state (events, selected date, view mode)

### Event Dialog (`add-edit-event-dialog.tsx`)
Form for creating/editing events with validation

### API Client (`requests.ts`)
Centralized HTTP client for all API calls to `http://localhost:8080`

```typescript
const API_BASE_URL = "http://localhost:8080";

export const getEvents = async (): Promise<IEvent[]> => {
  const response = await fetch(`${API_BASE_URL}/api/event`);
  if (!response.ok) throw new Error("Failed to fetch events");
  return response.json();
};
```

## 🏗️ Backend Architecture

### Main Components

**`main.cpp`**
- Entry point of the application
- Initializes Qt application

**`mainwindow.cpp`**
- Creates main window with QWebEngineView
- Loads frontend URL (dev: localhost:3000, prod: static files)

**`httpserver.cpp`**
- Sets up QHttpServer on port 8080
- Defines REST API routes:
  - `GET /api/event` → `handleGetEvents()`
  - `POST /api/event` → `handlePostEvent()`
  - `PUT /api/event/:id` → `handlePutEvent()`
  - `DELETE /api/event/:id` → `handleDeleteEvent()`
- Handles CORS headers for cross-origin requests

**`database.cpp`**
- Initializes SQLite database
- Creates `events` table if not exists
- Provides CRUD operations:
  - `getAllEvents()` → SELECT query
  - `insertEvent()` → INSERT query
  - `updateEvent()` → UPDATE query
  - `deleteEvent()` → DELETE query

## 🧪 Testing the API

## 🧪 Testing the API

Use the provided test script:
```bash
./scripts/test-api.sh
```

Or manually test endpoints:
```bash
# Test GET
curl http://localhost:8080/api/event

# Test POST
curl -X POST http://localhost:8080/api/event \
  -H "Content-Type: application/json" \
  -d '{"category":"Work","title":"Test","startDate":"2025-11-02T10:00:00","endDate":"2025-11-02T11:00:00","color":"blue","isReminderEnabled":false}'

# Test PUT
curl -X PUT http://localhost:8080/api/event/1 \
  -H "Content-Type: application/json" \
  -d '{"title":"Updated Title"}'

# Test DELETE
curl -X DELETE http://localhost:8080/api/event/1
```

## 📚 Additional Documentation

- **[API Reference](docs/API.md)** - Complete API documentation with examples
- **[Architecture Guide](docs/ARCHITECTURE.md)** - Detailed system architecture
- **[Quick Reference](docs/QUICK_REFERENCE.md)** - Quick commands and shortcuts

## 🛠️ Troubleshooting

### Frontend can't connect to backend
- Ensure Qt backend is running on port 8080
- Check if `API_BASE_URL` in `requests.ts` is correct
- Look for CORS errors in browser console

### Database errors
- Database file location: `~/.local/share/DailyReminder/events.db` (Linux)
- Check write permissions
- Delete database file to reset

### Qt build errors
```bash
# Install missing Qt modules
sudo apt-get install qt6-httpserver-dev qt6-webengine-dev

# Clean and rebuild
cd backend/build
rm -rf *
cmake ..
cmake --build .
```

## 📦 Building for Production

### Create Standalone Executable

1. Build frontend:
   ```bash
   cd frontend
   npm run build
   ```

2. Build Qt backend in Release mode:
   ```bash
   cd backend/build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . --config Release
   ```

3. Deploy the application:

   **Linux - AppImage:**
   ```bash
   # Install linuxdeploy
   wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
   wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
   chmod +x linuxdeploy*.AppImage
   
   # Create AppImage
   ./linuxdeploy-x86_64.AppImage --appdir AppDir --executable backend/build/backend --plugin qt --output appimage
   ```

   **Linux - Create .deb package:**
   ```bash
   # Install checkinstall
   sudo apt-get install checkinstall
   
   # Build and create .deb
   cd backend/build
   sudo checkinstall --pkgname=daily-reminder --pkgversion=1.0.0 --default
   ```

   **Windows - Portable .exe:**
   ```bash
   # After building in Release mode
   cd backend/build/Release
   
   # Deploy Qt dependencies
   windeployqt.exe backend.exe
   
   # Create installer using Inno Setup or NSIS (optional)
   ```

   **macOS - .app bundle:**
   ```bash
   # After building in Release mode
   cd backend/build
   macdeployqt backend.app -dmg
   ```

## 📦 Building Distributable Packages

### Linux AppImage (Recommended)

```bash
# 1. Build frontend
cd frontend
npm run build

# 2. Build backend
cd ../backend
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)

# 3. Create AppImage
cd ../..
mkdir -p AppDir/usr/bin
cp backend/build/backend AppDir/usr/bin/
cp -r frontend/out AppDir/usr/share/frontend

# Use linuxdeploy to bundle everything
./linuxdeploy-x86_64.AppImage --appdir AppDir \
  --executable AppDir/usr/bin/backend \
  --plugin qt \
  --output appimage
```

### Windows .exe

```bash
# 1. Build in Release mode using Qt Creator or CMake
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# 2. Deploy Qt libraries
cd Release
windeployqt.exe backend.exe

# 3. (Optional) Create installer with Inno Setup
# Download from: https://jrsoftware.org/isdl.php
```

### Linux .rpm (Fedora/RedHat)

```bash
# Quick build with automated script
npm run build:rpm

# Output: output/daily-reminder-1.0.0-1.fc42.x86_64.rpm

# Install
sudo rpm -ivh output/daily-reminder-*.rpm
```

### Windows .exe

**Option 1: Build on Windows (Recommended)**
```cmd
npm run build:windows
# Output: output/DailyReminder/backend.exe
```

**Option 2: Cross-compile from Linux**
```bash
# Requires: mingw-w64 toolchain and Qt for Windows
npm run build:windows-cross
# Output: output/windows/DailyReminder/backend.exe
```

Requirements for cross-compilation:
- Install mingw-w64: `sudo dnf install mingw64-gcc-c++`
- Install Qt for Windows (MinGW): Download from qt.io
- Set Qt path: `export QT_WIN_PATH=$HOME/Qt/6.9.0/mingw_64`

### Qt Build (Development)

```bash
# Build using Qt/CMake
./build-qt.sh

# Output: output/daily-reminder
# Run: ./output/daily-reminder
```

> **Note**: All build scripts output to the `output/` directory

## 🤝 Contributing

Contributions are welcome! Please:
- Follow TypeScript/React best practices for frontend
- Follow Qt coding conventions for backend
- Test API endpoints before submitting
- Update documentation as needed

## 📄 License

MIT License - See LICENSE file for details

## 🙏 Acknowledgments

- **Frontend**: Next.js, React, TypeScript, shadcn/ui, FullCalendar
- **Backend**: Qt Framework, SQLite
- **Icons**: Lucide Icons

---

**Questions?** Open an issue on GitHub or check the [docs](docs/) folder for more detailed guides.
