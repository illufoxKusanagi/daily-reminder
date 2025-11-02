# System Architecture Documentation

Comprehensive guide to the Daily Reminder application architecture.

## Table of Contents

1. [Overview](#overview)
2. [Architecture Diagram](#architecture-diagram)
3. [Technology Stack](#technology-stack)
4. [Component Breakdown](#component-breakdown)
5. [Data Flow](#data-flow)
6. [Communication Protocol](#communication-protocol)
7. [Frontend Architecture](#frontend-architecture)
8. [Backend Architecture](#backend-architecture)
9. [Database Design](#database-design)
10. [Request/Response Flow](#requestresponse-flow)

---

## Overview

Daily Reminder is a desktop calendar application built with a **Qt C++ backend** and **Next.js React frontend**, communicating via **HTTP REST API**.

### Key Architectural Decisions

**Why Qt + Next.js?**
- ✅ **Separation of Concerns**: Backend handles all business logic, frontend is pure UI
- ✅ **Technology Independence**: Can swap frontend (mobile app) or backend (web server) independently
- ✅ **Developer Experience**: Modern React dev tools + Qt's robust desktop capabilities
- ✅ **Testability**: API can be tested independently with curl/Postman

**Why HTTP REST API instead of Qt WebChannel?**
- ✅ **Standard Protocol**: RESTful APIs are universal and well-understood
- ✅ **Tool Support**: Can test with curl, Postman, or any HTTP client
- ✅ **Scalability**: Easy to add more endpoints or migrate to web server
- ✅ **Debugging**: Network inspector tools work out of the box

---

## Architecture Diagram

```
┌────────────────────────────────────────────────────────────────────┐
│                     Qt Desktop Application                         │
│                                                                    │
│  ┌───────────────────────────────────────────────────────────────┐ │
│  │               QMainWindow (mainwindow.cpp)                    │ │
│  │  ┌─────────────────────────────────────────────────────────┐  │ │
│  │  │         QWebEngineView (Chromium Browser)               │  │ │
│  │  │                                                         │  │ │
│  │  │  ┌────────────────────────────────────────────────────┐ │  │ │
│  │  │  │    Next.js Frontend (localhost:3000 in dev)        │ │  │ │
│  │  │  │                                                    │ │  │ │
│  │  │  │  App Router (app/)                                 │ │  │ │
│  │  │  │    └─ page.tsx (Calendar page)                     │ │  │ │
│  │  │  │                                                    │ │  │ │
│  │  │  │  Calendar Module (modules/calendar/)               │ │  │ │
│  │  │  │    ├─ components/calendar.tsx                      │ │  │ │
│  │  │  │    ├─ dialogs/add-edit-event-dialog.tsx            │ │  │ │
│  │  │  │    ├─ contexts/calendar-context.tsx                │ │  │ │
│  │  │  │    ├─ hooks/use-calendar.ts                        │ │  │ │
│  │  │  │    ├─ requests.ts (API Client)                     │ │  │ │
│  │  │  │    ├─ interfaces.ts (TypeScript types)             │ │  │ │
│  │  │  │    └─ schemas.ts (Zod validation)                  │ │  │ │
│  │  │  │                                                    │ │  │ │
│  │  │  │  UI Components (components/ui/)                    │ │  │ │
│  │  │  │    └─ shadcn/ui (Dialog, Button, Input, etc.)      │ │  │ │
│  │  │  └─────────────────────────────────────────────────── ┘ │  │ │
│  │  │                         │                               │  │ │
│  │  │                         │ HTTP Fetch API                │  │ │
│  │  │                         ↓                               │  │ │
│  │  └─────────────────────────────────────────────────────────┘  │ │
│  └───────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              │                                     │
│  ┌───────────────────────────────────────────────────────────────┐ │
│  │              QHttpServer (httpserver.cpp)                     │ │
│  │                   Port: 8080                                  │ │
│  │                                                               │ │
│  │  REST API Endpoints:                                          │ │
│  │    GET    /api/event          → handleGetEvents()             │ │
│  │    POST   /api/event          → handlePostEvent()             │ │
│  │    PUT    /api/event/:id      → handlePutEvent()              │ │
│  │    DELETE /api/event/:id      → handleDeleteEvent()           │ │
│  │                                                               │ │
│  │  CORS Middleware:                                             │ │
│  │    Access-Control-Allow-Origin: *                             │ │
│  │    Access-Control-Allow-Methods: GET, POST, PUT, DELETE       │ │
│  │    Access-Control-Allow-Headers: Content-Type                 │ │
│  └───────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              │ SQL Queries                         │
│                              ↓                                     │
│  ┌───────────────────────────────────────────────────────────────┐ │
│  │              Database Layer (database.cpp)                    │ │
│  │                                                               │ │
│  │  QSqlDatabase (SQLite):                                       │ │
│  │    • getAllEvents()       → SELECT * FROM events              │ │
│  │    • insertEvent()        → INSERT INTO events ...            │ │
│  │    • updateEvent()        → UPDATE events SET ...             │ │
│  │    • deleteEvent()        → DELETE FROM events WHERE ...      │ │
│  │    • initDatabase()       → CREATE TABLE IF NOT EXISTS        │ │
│  └───────────────────────────────────────────────────────────────┘ │
│                              │                                     │
│                              │ File I/O                            │
│                              ↓                                     │
│  ┌───────────────────────────────────────────────────────────────┐ │
│  │                   SQLite Database File                        │ │
│  │   ~/.local/share/DailyReminder/events.db (Linux)              │ │
│  │                                                               │ │
│  │   Table: events                                               │ │
│  │   ┌──────┬─────────┬───────┬─────────────┬────────────┬─────┐ │ │
│  │   │  id  │category │ title │ description │ start_date │ ... │ │ │
│  │   ├──────┼─────────┼───────┼─────────────┼────────────┼─────┤ │ │
│  │   │  1   │ Work    │Meeting│ Team sync   │2025-11-... │ ... │ │ │
│  │   │  2   │Personal │Dentist│ Checkup     │2025-11-... │ ... │ │ │
│  │   └──────┴─────────┴───────┴─────────────┴────────────┴─────┘ │ │
│  └───────────────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────┘
```

---

## Technology Stack

### Frontend

| Component       | Technology      | Purpose                         |
| --------------- | --------------- | ------------------------------- |
| **Framework**   | Next.js 14      | React framework with App Router |
| **Language**    | TypeScript      | Type-safe JavaScript            |
| **UI Library**  | React 18        | Component-based UI              |
| **Styling**     | Tailwind CSS    | Utility-first CSS               |
| **Components**  | shadcn/ui       | Pre-built accessible components |
| **Calendar**    | FullCalendar    | Interactive calendar component  |
| **Forms**       | React Hook Form | Form state management           |
| **Validation**  | Zod             | Schema validation               |
| **Date/Time**   | date-fns        | Date manipulation utilities     |
| **HTTP Client** | Fetch API       | Built-in browser API            |

### Backend

| Component        | Technology            | Purpose                           |
| ---------------- | --------------------- | --------------------------------- |
| **Framework**    | Qt 6                  | C++ desktop application framework |
| **Language**     | C++17                 | System programming language       |
| **HTTP Server**  | QHttpServer           | Built-in HTTP server              |
| **Web View**     | QWebEngineView        | Chromium-based browser            |
| **Database**     | QSqlDatabase (SQLite) | Local SQL database                |
| **JSON**         | QJsonDocument         | JSON parsing/serialization        |
| **Build System** | CMake                 | Cross-platform build              |

---

## Component Breakdown

### Frontend Components

#### 1. **API Client** (`requests.ts`)
```typescript
const API_BASE_URL = "http://localhost:8080";

export const getEvents = async (): Promise<IEvent[]> => {
  const response = await fetch(`${API_BASE_URL}/api/event`);
  if (!response.ok) throw new Error("Failed to fetch events");
  return response.json();
};
```

**Responsibilities:**
- Makes HTTP requests to backend
- Handles response parsing
- Throws errors for failed requests
- Provides clean async/await interface

#### 2. **Calendar Context** (`calendar-context.tsx`)
```typescript
const CalendarContext = createContext<CalendarContextType | undefined>(undefined);

export function CalendarProvider({ children }: { children: ReactNode }) {
  const [events, setEvents] = useState<IEvent[]>([]);
  
  const addEvent = async (event: IEvent) => {
    const newEvent = await createEvent(event);
    setEvents([...events, newEvent]);
  };
  
  // ... updateEvent, deleteEvent
}
```

**Responsibilities:**
- Manages global calendar state
- Provides CRUD operations to components
- Syncs state with backend via API calls
- Handles optimistic updates

#### 3. **Event Dialog** (`add-edit-event-dialog.tsx`)
```typescript
const form = useForm<TEventFormData>({
  resolver: zodResolver(eventSchema),
  defaultValues: { /* ... */ }
});

const onSubmit = async (values: TEventFormData) => {
  const response = await fetch(`${API_BASE_URL}/api/event`, {
    method: 'POST',
    body: JSON.stringify(values)
  });
  // ...
};
```

**Responsibilities:**
- Renders form UI
- Validates input with Zod
- Submits data to backend
- Shows success/error toasts

#### 4. **Type Definitions** (`interfaces.ts`)
```typescript
export interface IEvent {
  id: number;
  category: string;
  title: string;
  description?: string;
  startDate: string;
  endDate: string;
  color: string;
  reminderTime?: string;
  isReminderEnabled?: boolean;
}
```

**Responsibilities:**
- Defines TypeScript interfaces
- Ensures type safety across frontend
- Matches backend JSON structure

### Backend Components

#### 1. **Main Application** (`main.cpp`)
```cpp
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("DailyReminder");
    app.setApplicationName("DailyReminder");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
```

**Responsibilities:**
- Initializes Qt application
- Creates main window
- Starts event loop

#### 2. **Main Window** (`mainwindow.cpp`)
```cpp
MainWindow::MainWindow() {
    m_webView = new QWebEngineView(this);
    m_httpServer = new HttpServer(this);
    
    m_httpServer->start(8080);
    m_webView->load(QUrl("http://localhost:3000")); // Dev mode
    
    setCentralWidget(m_webView);
}
```

**Responsibilities:**
- Creates web view for frontend
- Starts HTTP server
- Loads frontend URL
- Manages window lifecycle

#### 3. **HTTP Server** (`httpserver.cpp`)
```cpp
void HttpServer::setupRoutes() {
    m_server->route("/api/event", QHttpServerRequest::Method::Get,
        [this](const QHttpServerRequest &request) {
            return handleGetEvents(request);
        });
    
    m_server->route("/api/event", QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest &request) {
            return handlePostEvent(request);
        });
    // ... PUT, DELETE
}
```

**Responsibilities:**
- Defines API endpoints
- Routes requests to handlers
- Adds CORS headers
- Returns JSON responses

#### 4. **Database** (`database.cpp`)
```cpp
QJsonArray Database::getAllEvents() {
    QSqlQuery query("SELECT * FROM events ORDER BY start_date");
    QJsonArray events;
    
    while (query.next()) {
        QJsonObject event;
        event["id"] = query.value("id").toInt();
        event["category"] = query.value("category").toString();
        // ... other fields
        events.append(event);
    }
    
    return events;
}
```

**Responsibilities:**
- Executes SQL queries
- Converts between SQL and JSON
- Handles database errors
- Manages database file

---

## Data Flow

### Example: Creating an Event

```
┌─────────────────────────────────────────────────────────────────────┐
│ 1. USER ACTION                                                      │
│    User clicks "Add Event" button in calendar                       │
│    Opens dialog, fills form, clicks "Save"                          │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 2. FRONTEND VALIDATION (Zod Schema)                                 │
│    eventSchema.parse(formData)                                      │
│    • Validates required fields (title, category, dates, color)      │
│    • Validates date formats (ISO 8601)                              │
│    • Validates enum values (category, color)                        │
│    ❌ If invalid → Show error messages                              │
│    ✅ If valid → Continue                                           │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 3. API REQUEST (Frontend → Backend)                                 │
│    const response = await fetch("http://localhost:8080/api/event", {│
│      method: "POST",                                                │
│      headers: { "Content-Type": "application/json" },               │
│      body: JSON.stringify({                                         │
│        category: "Work",                                            │
│        title: "Team Meeting",                                       │
│        startDate: "2025-11-02T10:00:00",                            │
│        endDate: "2025-11-02T11:00:00",                              │
│        color: "blue",                                               │
│        isReminderEnabled: false                                     │
│      })                                                             │
│    });                                                              │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 4. BACKEND ROUTING (QHttpServer)                                    │
│    Server receives POST /api/event                                  │
│    Routes to handlePostEvent(request)                               │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 5. REQUEST PARSING (httpserver.cpp)                                 │
│    QJsonDocument doc = QJsonDocument::fromJson(request.body());     │
│    QJsonObject json = doc.object();                                 │
│                                                                     │
│    Validate required fields:                                        │
│    • Check json.contains("category")                                │
│    • Check json.contains("title")                                   │
│    • Check json.contains("startDate")                               │
│    • Check json.contains("endDate")                                 │
│    • Check json.contains("color")                                   │
│                                                                     │
│    ❌ If missing → Return 400 Bad Request                           │
│    ✅ If complete → Continue                                        │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 6. DATABASE INSERT (database.cpp)                                   │
│    QSqlQuery query;                                                 │
│    query.prepare(                                                   │
│      "INSERT INTO events (category, title, description, "           │
│      "start_date, end_date, color, reminder_time, "                 │
│      "is_reminder_enabled) VALUES (?, ?, ?, ?, ?, ?, ?, ?)"         │
│    );                                                               │
│    query.addBindValue(json["category"].toString());                 │
│    query.addBindValue(json["title"].toString());                    │
│    // ... bind other values                                         │
│    query.exec();                                                    │
│                                                                     │
│    int newId = query.lastInsertId().toInt();                        │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 7. RESPONSE BUILDING                                                │
│    QJsonObject response = json; // Original data                    │
│    response["id"] = newId;      // Add generated ID                 │
│                                                                     │
│    QHttpServerResponse httpResponse(                                │
│      QJsonDocument(response).toJson(),                              │
│      QHttpServerResponse::StatusCode::Created // 201                │
│    );                                                               │
│    httpResponse.addHeader("Content-Type", "application/json");      │
│    httpResponse.addHeader("Access-Control-Allow-Origin", "*");      │
│                                                                     │
│    return httpResponse;                                             │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 8. FRONTEND UPDATE                                                  │
│    const newEvent = await response.json();                          │
│    // newEvent = { id: 1, category: "Work", title: "...", ... }     │
│                                                                     │
│    addEvent(newEvent);  // Update context state                     │
│    // → setEvents([...events, newEvent])                            │
│                                                                     │
│    toast.success("Event created successfully");                     │
│    onClose();  // Close dialog                                      │
└─────────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────────┐
│ 9. UI REFRESH                                                       │
│    React re-renders calendar with new event                         │
│    Event appears on calendar with correct date, color, title        │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Communication Protocol

### HTTP REST API over localhost

**Protocol:** HTTP/1.1  
**Port:** 8080  
**Base URL:** `http://localhost:8080`  
**Content-Type:** `application/json`

### Request/Response Cycle

```
Frontend (Browser)                          Backend (Qt)
      │                                          │
      │  POST /api/event                         │
      │  Content-Type: application/json          │
      │  {"title": "Meeting", ...}               │
      ├─────────────────────────────────────────>│
      │                                          │
      │                                  ┌───────┴───────┐
      │                                  │ Parse JSON    │
      │                                  │ Validate data │
      │                                  │ Insert to DB  │
      │                                  │ Build response│
      │                                  └───────┬───────┘
      │                                          │
      │  201 Created                             │
      │  Content-Type: application/json          │
      │  {"id": 1, "title": "Meeting", ...}      │
      │<─────────────────────────────────────────┤
      │                                          │
┌─────┴─────┐                                    │
│ Update UI │                                    │
│ Add event │                                    │
│ to state  │                                    │
└───────────┘                                    │
```

---

## Frontend Architecture

### Directory Structure

```
frontend/src/
├── app/
│   ├── layout.tsx              # Root layout with providers
│   ├── page.tsx                # Main calendar page
│   └── globals.css             # Global styles
│
├── components/
│   └── ui/                     # shadcn/ui components
│       ├── button.tsx
│       ├── dialog.tsx
│       ├── input.tsx
│       └── ...
│
└── modules/
    └── calendar/
        ├── components/
        │   ├── calendar.tsx            # Main calendar component
        │   ├── color-picker.tsx        # Color selector
        │   └── event-tooltip.tsx       # Hover tooltip
        │
        ├── dialogs/
        │   └── add-edit-event-dialog.tsx   # Event form
        │
        ├── contexts/
        │   └── calendar-context.tsx    # State management
        │
        ├── hooks/
        │   └── use-calendar.ts         # Calendar operations
        │
        ├── interfaces.ts               # TypeScript types
        ├── schemas.ts                  # Zod validation
        └── requests.ts                 # API client
```

### State Management Flow

```
┌──────────────────────────────────────────────────────────────┐
│              CalendarProvider (Context)                      │
│  ┌────────────────────────────────────────────────────────┐  │
│  │ State:                                                 │  │
│  │   - events: IEvent[]                                   │  │
│  │   - selectedDate: Date                                 │  │
│  │   - viewMode: 'month' | 'week' | 'day'                 │  │
│  └────────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────────┐  │
│  │ Actions:                                               │  │
│  │   - addEvent(event)      → POST /api/event             │  │
│  │   - updateEvent(id, data) → PUT /api/event/:id         │  │
│  │   - deleteEvent(id)       → DELETE /api/event/:id      │  │
│  │   - refreshEvents()       → GET /api/event             │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
                            │
                            ├─→ Calendar Component (read events, selectedDate)
                            ├─→ EventDialog (call addEvent/updateEvent)
                            └─→ EventList (call deleteEvent)
```

---

## Backend Architecture

### Class Diagram

```
┌────────────────────────────────────────┐
│           MainWindow                   │
│────────────────────────────────────────│
│ - m_webView: QWebEngineView*           │
│ - m_httpServer: HttpServer*            │
│────────────────────────────────────────│
│ + MainWindow()                         │
│ + ~MainWindow()                        │
└────────────────────────────────────────┘
                  │
                  │ creates
                  ↓
┌────────────────────────────────────────┐
│           HttpServer                   │
│────────────────────────────────────────│
│ - m_server: QHttpServer*               │
│ - m_database: Database*                │
│────────────────────────────────────────│
│ + start(port: int)                     │
│ - setupRoutes()                        │
│ - handleGetEvents()                    │
│ - handlePostEvent()                    │
│ - handlePutEvent()                     │
│ - handleDeleteEvent()                  │
│ - addCorsHeaders()                     │
└────────────────────────────────────────┘
                  │
                  │ uses
                  ↓
┌────────────────────────────────────────┐
│            Database                    │
│────────────────────────────────────────│
│ - m_db: QSqlDatabase                   │
│────────────────────────────────────────│
│ + initDatabase()                       │
│ + getAllEvents(): QJsonArray           │
│ + insertEvent(data): int               │
│ + updateEvent(id, data): bool          │
│ + deleteEvent(id): bool                │
│ + eventExists(id): bool                │
└────────────────────────────────────────┘
```

---

## Database Design

### Schema

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

### Field Constraints

| Field                 | Type    | Constraints                | Description                          |
| --------------------- | ------- | -------------------------- | ------------------------------------ |
| `id`                  | INTEGER | PRIMARY KEY, AUTOINCREMENT | Unique identifier                    |
| `category`            | TEXT    | NOT NULL                   | Must be one of predefined categories |
| `title`               | TEXT    | NOT NULL                   | Event title, cannot be empty         |
| `description`         | TEXT    | NULL                       | Optional description                 |
| `start_date`          | TEXT    | NOT NULL                   | ISO 8601 datetime                    |
| `end_date`            | TEXT    | NOT NULL                   | ISO 8601 datetime                    |
| `color`               | TEXT    | NOT NULL                   | Must be one of predefined colors     |
| `reminder_time`       | TEXT    | NULL                       | ISO 8601 datetime                    |
| `is_reminder_enabled` | INTEGER | DEFAULT 0                  | Boolean: 0 or 1                      |

### Indexes

Currently no additional indexes. For performance with many events, consider:
```sql
CREATE INDEX idx_start_date ON events(start_date);
CREATE INDEX idx_category ON events(category);
```

---

## Request/Response Flow

### Complete HTTP Transaction

```
┌──────────────────────────────────────────────────────────────┐
│ Frontend (TypeScript)                                        │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 1. User clicks "Add Event"
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Event Dialog (React Component)                               │
│   - Collects form data                                       │
│   - Validates with Zod schema                                │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 2. onSubmit(formData)
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ API Client (requests.ts)                                     │
│   createEvent(data) {                                        │
│     return fetch("http://localhost:8080/api/event", {        │
│       method: "POST",                                        │
│       body: JSON.stringify(data)                             │
│     })                                                       │
│   }                                                          │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 3. HTTP POST Request
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ QHttpServer (Qt Backend)                                     │
│   - Receives POST /api/event                                 │
│   - Routes to handlePostEvent()                              │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 4. Parse request body
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ HttpServer::handlePostEvent()                                │
│   - Validate JSON structure                                  │
│   - Check required fields                                    │
│   - Call database.insertEvent()                              │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 5. SQL INSERT
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Database::insertEvent()                                      │
│   INSERT INTO events (...) VALUES (...)                      │
│   RETURNING id                                               │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 6. Return new ID
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ HttpServer::handlePostEvent()                                │
│   - Build JSON response with ID                              │
│   - Set status 201 Created                                   │
│   - Add CORS headers                                         │
│   - Return QHttpServerResponse                               │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 7. HTTP 201 Response
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ API Client (requests.ts)                                     │
│   - Parse JSON response                                      │
│   - Return event object                                      │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 8. Update state
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Calendar Context                                             │
│   - Add new event to state                                   │
│   - Trigger re-render                                        │
└──────────────────────────────────────────────────────────────┘
                            │
                            │ 9. UI update
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ Calendar Component                                           │
│   - Display new event on calendar                            │
│   - Show success toast                                       │
│   - Close dialog                                             │
└──────────────────────────────────────────────────────────────┘
```

---

## Summary

**Key Takeaways:**

1. **Clean Separation**: Frontend handles UI only, backend handles all business logic
2. **Standard Protocol**: HTTP REST API makes the system testable and extensible
3. **Type Safety**: TypeScript + Zod on frontend, C++ types on backend
4. **Simple Storage**: SQLite for lightweight local database
5. **Modern Stack**: React + Next.js for great developer experience, Qt for robust desktop capabilities

This architecture makes it easy to:
- Test API endpoints independently
- Swap frontend (e.g., mobile app)
- Add new features (just add new endpoints)
- Debug issues (network inspector, SQL logs)
- Scale up (migrate to web server if needed)
