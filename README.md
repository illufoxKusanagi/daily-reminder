# Daily Reminder

A desktop calendar application for managing daily activities with reminders, built with Qt 6 (backend REST API) and Next.js (frontend UI).

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                Qt Desktop Application                           │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │         QWebEngineView (Embedded Chromium Browser)        │  │
│  │  ┌─────────────────────────────────────────────────────┐  │  │
│  │  │   Next.js Frontend (React + TypeScript)             │  │  │
│  │  │   • Calendar UI (FullCalendar)                      │  │  │
│  │  │   • Event dialogs (shadcn/ui)                       │  │  │
│  │  │   • Category management                             │  │  │
│  │  │   • Date/time pickers                               │  │  │
│  │  └─────────────────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────────────────┘  │
│                            │                                     │
│              HTTP REST API │ (localhost:8080)                    │
│                            ↓                                     │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              QHttpServer (REST API)                       │  │
│  │   GET    /api/event          - List all events           │  │
│  │   POST   /api/event          - Create new event          │  │
│  │   PUT    /api/event/:id      - Update event              │  │
│  │   DELETE /api/event/:id      - Delete event              │  │
│  └───────────────────────────────────────────────────────────┘  │
│                            │                                     │
│                            ↓                                     │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                SQLite Database                            │  │
│  │   events table:                                           │  │
│  │   • id, category, title, description                      │  │
│  │   • start_date, end_date, color                           │  │
│  │   • reminder_time, is_reminder_enabled                    │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

## 🌟 Features

- ✅ **Calendar View**: Monthly calendar with color-coded events
- ✅ **Event Management**: Create, edit, delete events with categories
- ✅ **Categories**: Work, Personal, Health, Shopping, Exercise, Study, Social, Home, Other
- ✅ **Reminders**: Set reminder times with enable/disable toggle
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

## 🚀 Quick Start

### 1. Install Qt 6
### 1. Install Qt 6

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev qt6-httpserver-dev
```

**macOS:**
```bash
brew install qt@6
```

**Windows:**
- Download Qt from https://www.qt.io/download
- Install Qt with WebEngine and HttpServer modules

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

2. Build Qt backend:
   ```bash
   cd backend/build
   cmake --build . --config Release
   ```

3. Deploy Qt app:
   ```bash
   # Linux
   linuxdeployqt DailyReminder -appimage
   
   # macOS
   macdeployqt DailyReminder.app -dmg
   
   # Windows
   windeployqt DailyReminder.exe
   ```

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
