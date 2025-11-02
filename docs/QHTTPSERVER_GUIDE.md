# Complete Guide: Daily Reminder with Qt Backend (QHttpServer) + Next.js Frontend

## 📋 Project Architecture Overview

### **What We're Building:**
A desktop application where:
- **Frontend**: Next.js (React + TypeScript + shadcn/ui) - Pure UI, no business logic
- **Backend**: Qt 6 C++ with QHttpServer - REST API for all operations
- **Database**: SQLite (via QSqlDatabase) - Local data storage
- **Communication**: HTTP REST API between frontend and backend
- **Deployment**: Single executable that bundles everything

### **Why This Architecture?**

```
┌─────────────────────────────────────────────────────────────┐
│                   Qt Desktop Application                    │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  QWebEngineView (Chromium)                           │   │
│  │  ┌────────────────────────────────────────────────┐  │   │
│  │  │  Next.js Frontend (React)                      │  │   │
│  │  │  - Displays UI                                 │  │   │
│  │  │  - Handles user input                          │  │   │
│  │  │  - Makes HTTP requests                         │  │   │
│  │  └────────────────────────────────────────────────┘  │   │
│  └──────────────────────────────────────────────────────┘   │
│                          │                                  │
│                          │ HTTP (localhost:8080)            │
│                          ↓                                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  QHttpServer (REST API)                              │   │
│  │  GET    /api/activities       - List all             │   │
│  │  POST   /api/activities       - Create new           │   │
│  │  PUT    /api/activities/:id   - Update               │   │
│  │  DELETE /api/activities/:id   - Delete               │   │
│  │  GET    /api/alarms           - List alarms          │   │
│  │  POST   /api/alarms           - Create alarm         │   │
│  └──────────────────────────────────────────────────────┘   │
│                          │                                  │
│                          ↓                                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Business Logic (C++)                                │   │
│  │  - ActivityManager: CRUD operations                  │   │
│  │  - AlarmManager: Timer-based notifications           │   │
│  │  - Validation, sorting, filtering                    │   │
│  └──────────────────────────────────────────────────────┘   │
│                          │                                  │
│                          ↓                                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Database Layer (QSqlDatabase)                       │   │
│  │  - SQLite database: activities.db                    │   │
│  │  - Tables: activities, alarms                        │   │
│  │  - SQL queries, transactions                         │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

**Benefits:**
- ✅ **Clean separation**: Frontend = UI only, Backend = All logic
- ✅ **Testable**: Can test API with Postman/curl independently
- ✅ **Scalable**: Easy to add features (just add new endpoints)
- ✅ **Learning**: You learn proper REST API design
- ✅ **Future-proof**: Can replace frontend with mobile app later

---

## 📁 Project Structure

```
daily-reminder/
├── frontend/                      # Next.js Frontend
│   ├── app/
│   │   ├── page.tsx              # Main page
│   │   ├── layout.tsx            # Root layout
│   │   └── globals.css           # Global styles
│   ├── components/
│   │   ├── ui/                   # shadcn/ui components
│   │   ├── ActivityForm.tsx      # Form to create activities
│   │   ├── ActivityList.tsx      # Display activities
│   │   ├── ActivityCalendar.tsx  # Calendar view (future)
│   │   └── AlarmDialog.tsx       # Alarm setup
│   ├── lib/
│   │   ├── api.ts                # HTTP client (fetch wrapper)
│   │   └── types.ts              # TypeScript interfaces
│   ├── hooks/
│   │   ├── useActivities.ts      # Hook for activity operations
│   │   └── useAlarms.ts          # Hook for alarm operations
│   └── package.json
│
├── backend/                       # Qt Backend
│   ├── src/
│   │   ├── main.cpp              # Entry point
│   │   ├── mainwindow.{h,cpp}    # Main window + QWebEngineView
│   │   ├── httpserver.{h,cpp}    # QHttpServer setup
│   │   ├── activitymanager.{h,cpp}   # Activity CRUD + DB
│   │   ├── alarmmanager.{h,cpp}      # Alarm logic + timers
│   │   └── database.{h,cpp}      # Database initialization
│   ├── CMakeLists.txt
│   └── build/
│
├── package.json                   # Root workspace config
├── build.sh                       # Build script
├── run.sh                         # Run script
└── README.md
```

---

## 🔄 How It Works (Data Flow)

### **Example: Creating an Activity**

```
┌─────────────────────────────────────────────────────────────┐
│ Step 1: User clicks "Add Activity" in Next.js UI            │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 2: React Hook calls API                                │
│ const { addActivity } = useActivities();                    │
│ addActivity({title: "Meeting", time: "10:00"});             │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 3: HTTP Request                                        │
│ POST http://localhost:8080/api/activities                   │
│ Body: {"title": "Meeting", "time": "10:00"}                 │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 4: QHttpServer receives request                        │
│ HttpServer::handleCreateActivity(request)                   │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 5: Business Logic validates and processes              │
│ ActivityManager::createActivity(title, time)                │
│ - Validate data                                             │
│ - Generate ID                                               │
│ - Set created timestamp                                     │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 6: Save to Database                                    │
│ QSqlQuery: INSERT INTO activities (title, time) ...         │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 7: Return JSON Response                                │
│ HTTP 201 Created                                            │
│ {"id": 123, "title": "Meeting", "time": "10:00"}            │
└─────────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 8: Frontend updates UI                                 │
│ Activity appears in list                                    │
└─────────────────────────────────────────────────────────────┘
```

---

## 📚 Complete Code Implementation

### **1. Database Schema**

Create `backend/src/database.h`:
```cpp
#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

class Database
{
public:
    static Database& instance();
    bool initialize();
    QSqlDatabase& db() { return m_db; }
    
private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    bool createTables();
    
    QSqlDatabase m_db;
};

#endif
```

Create `backend/src/database.cpp`:
```cpp
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

Database::~Database()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::initialize()
{
    // Set database path
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataDir)) {
        dir.mkpath(dataDir);
    }
    
    QString dbPath = dataDir + "/activities.db";
    m_db.setDatabaseName(dbPath);
    
    qDebug() << "Database path:" << dbPath;
    
    if (!m_db.open()) {
        qDebug() << "ERROR: Failed to open database:" << m_db.lastError().text();
        return false;
    }
    
    qDebug() << "Database opened successfully";
    return createTables();
}

bool Database::createTables()
{
    QSqlQuery query(m_db);
    
    // Activities table
    QString createActivities = R"(
        CREATE TABLE IF NOT EXISTS activities (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            description TEXT,
            scheduled_time TEXT,
            duration INTEGER DEFAULT 60,
            priority INTEGER DEFAULT 0,
            completed INTEGER DEFAULT 0,
            created_at TEXT NOT NULL,
            updated_at TEXT
        )
    )";
    
    if (!query.exec(createActivities)) {
        qDebug() << "ERROR creating activities table:" << query.lastError().text();
        return false;
    }
    
    // Alarms table
    QString createAlarms = R"(
        CREATE TABLE IF NOT EXISTS alarms (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            activity_id INTEGER NOT NULL,
            alarm_time TEXT NOT NULL,
            is_active INTEGER DEFAULT 1,
            FOREIGN KEY (activity_id) REFERENCES activities(id) ON DELETE CASCADE
        )
    )";
    
    if (!query.exec(createAlarms)) {
        qDebug() << "ERROR creating alarms table:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Database tables created successfully";
    return true;
}
```

---

### **2. Activity Manager (Business Logic)**

Create `backend/src/activitymanager.h`:
```cpp
#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class ActivityManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ActivityManager(QObject *parent = nullptr);
    
    // CRUD Operations
    QJsonObject createActivity(const QJsonObject& data);
    QJsonArray getAllActivities();
    QJsonObject getActivityById(int id);
    QJsonObject updateActivity(int id, const QJsonObject& data);
    bool deleteActivity(int id);
    
    // Additional queries
    QJsonArray getActivitiesByDate(const QString& date);
    QJsonArray getUpcomingActivities();
    bool markAsCompleted(int id, bool completed);
    
signals:
    void activityCreated(int id);
    void activityUpdated(int id);
    void activityDeleted(int id);
    
private:
    QString currentDateTime() const;
    QJsonObject activityFromQuery(class QSqlQuery& query);
};

#endif
```

Create `backend/src/activitymanager.cpp`:
```cpp
#include "activitymanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

ActivityManager::ActivityManager(QObject *parent)
    : QObject(parent)
{
}

QJsonObject ActivityManager::createActivity(const QJsonObject& data)
{
    QSqlQuery query(Database::instance().db());
    
    query.prepare(R"(
        INSERT INTO activities (title, description, scheduled_time, duration, priority, created_at)
        VALUES (:title, :description, :scheduled_time, :duration, :priority, :created_at)
    )");
    
    query.bindValue(":title", data["title"].toString());
    query.bindValue(":description", data["description"].toString());
    query.bindValue(":scheduled_time", data["scheduledTime"].toString());
    query.bindValue(":duration", data["duration"].toInt(60));
    query.bindValue(":priority", data["priority"].toInt(0));
    query.bindValue(":created_at", currentDateTime());
    
    if (!query.exec()) {
        qDebug() << "ERROR creating activity:" << query.lastError().text();
        return QJsonObject{{"error", "Failed to create activity"}};
    }
    
    int id = query.lastInsertId().toInt();
    emit activityCreated(id);
    
    return getActivityById(id);
}

QJsonArray ActivityManager::getAllActivities()
{
    QSqlQuery query(Database::instance().db());
    
    if (!query.exec("SELECT * FROM activities ORDER BY scheduled_time DESC")) {
        qDebug() << "ERROR fetching activities:" << query.lastError().text();
        return QJsonArray();
    }
    
    QJsonArray activities;
    while (query.next()) {
        activities.append(activityFromQuery(query));
    }
    
    return activities;
}

QJsonObject ActivityManager::getActivityById(int id)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM activities WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec() || !query.next()) {
        qDebug() << "ERROR fetching activity:" << query.lastError().text();
        return QJsonObject{{"error", "Activity not found"}};
    }
    
    return activityFromQuery(query);
}

QJsonObject ActivityManager::updateActivity(int id, const QJsonObject& data)
{
    QSqlQuery query(Database::instance().db());
    
    query.prepare(R"(
        UPDATE activities SET
            title = :title,
            description = :description,
            scheduled_time = :scheduled_time,
            duration = :duration,
            priority = :priority,
            updated_at = :updated_at
        WHERE id = :id
    )");
    
    query.bindValue(":id", id);
    query.bindValue(":title", data["title"].toString());
    query.bindValue(":description", data["description"].toString());
    query.bindValue(":scheduled_time", data["scheduledTime"].toString());
    query.bindValue(":duration", data["duration"].toInt());
    query.bindValue(":priority", data["priority"].toInt());
    query.bindValue(":updated_at", currentDateTime());
    
    if (!query.exec()) {
        qDebug() << "ERROR updating activity:" << query.lastError().text();
        return QJsonObject{{"error", "Failed to update activity"}};
    }
    
    emit activityUpdated(id);
    return getActivityById(id);
}

bool ActivityManager::deleteActivity(int id)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("DELETE FROM activities WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "ERROR deleting activity:" << query.lastError().text();
        return false;
    }
    
    emit activityDeleted(id);
    return true;
}

QJsonArray ActivityManager::getActivitiesByDate(const QString& date)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM activities WHERE DATE(scheduled_time) = :date");
    query.bindValue(":date", date);
    
    if (!query.exec()) {
        qDebug() << "ERROR fetching activities by date:" << query.lastError().text();
        return QJsonArray();
    }
    
    QJsonArray activities;
    while (query.next()) {
        activities.append(activityFromQuery(query));
    }
    
    return activities;
}

QJsonArray ActivityManager::getUpcomingActivities()
{
    QSqlQuery query(Database::instance().db());
    query.prepare(R"(
        SELECT * FROM activities 
        WHERE scheduled_time >= :now AND completed = 0
        ORDER BY scheduled_time ASC
        LIMIT 10
    )");
    query.bindValue(":now", currentDateTime());
    
    if (!query.exec()) {
        qDebug() << "ERROR fetching upcoming activities:" << query.lastError().text();
        return QJsonArray();
    }
    
    QJsonArray activities;
    while (query.next()) {
        activities.append(activityFromQuery(query));
    }
    
    return activities;
}

bool ActivityManager::markAsCompleted(int id, bool completed)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("UPDATE activities SET completed = :completed WHERE id = :id");
    query.bindValue(":completed", completed ? 1 : 0);
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "ERROR marking activity:" << query.lastError().text();
        return false;
    }
    
    emit activityUpdated(id);
    return true;
}

QString ActivityManager::currentDateTime() const
{
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

QJsonObject ActivityManager::activityFromQuery(QSqlQuery& query)
{
    QJsonObject obj;
    obj["id"] = query.value("id").toInt();
    obj["title"] = query.value("title").toString();
    obj["description"] = query.value("description").toString();
    obj["scheduledTime"] = query.value("scheduled_time").toString();
    obj["duration"] = query.value("duration").toInt();
    obj["priority"] = query.value("priority").toInt();
    obj["completed"] = query.value("completed").toBool();
    obj["createdAt"] = query.value("created_at").toString();
    obj["updatedAt"] = query.value("updated_at").toString();
    return obj;
}
```

---

### **3. Alarm Manager**

Create `backend/src/alarmmanager.h`:
```cpp
#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>

class AlarmManager : public QObject
{
    Q_OBJECT
    
public:
    explicit AlarmManager(QObject *parent = nullptr);
    
    QJsonObject createAlarm(int activityId, const QString& alarmTime);
    QJsonArray getAlarmsForActivity(int activityId);
    bool deleteAlarm(int alarmId);
    void checkAlarms(); // Called periodically
    
signals:
    void alarmTriggered(int activityId, const QString& title);
    
private slots:
    void onTimerTimeout();
    
private:
    QTimer *m_checkTimer;
    QMap<int, QDateTime> m_activeAlarms; // alarmId -> trigger time
    
    void loadActiveAlarms();
    void showNotification(int activityId, const QString& title);
};

#endif
```

Create `backend/src/alarmmanager.cpp`:
```cpp
#include "alarmmanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent)
{
    // Check alarms every minute
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &AlarmManager::onTimerTimeout);
    m_checkTimer->start(60000); // 60 seconds
    
    loadActiveAlarms();
}

QJsonObject AlarmManager::createAlarm(int activityId, const QString& alarmTime)
{
    QSqlQuery query(Database::instance().db());
    
    query.prepare(R"(
        INSERT INTO alarms (activity_id, alarm_time, is_active)
        VALUES (:activity_id, :alarm_time, 1)
    )");
    
    query.bindValue(":activity_id", activityId);
    query.bindValue(":alarm_time", alarmTime);
    
    if (!query.exec()) {
        qDebug() << "ERROR creating alarm:" << query.lastError().text();
        return QJsonObject{{"error", "Failed to create alarm"}};
    }
    
    int id = query.lastInsertId().toInt();
    
    // Add to active alarms
    QDateTime triggerTime = QDateTime::fromString(alarmTime, Qt::ISODate);
    m_activeAlarms[id] = triggerTime;
    
    QJsonObject result;
    result["id"] = id;
    result["activityId"] = activityId;
    result["alarmTime"] = alarmTime;
    result["isActive"] = true;
    
    return result;
}

QJsonArray AlarmManager::getAlarmsForActivity(int activityId)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM alarms WHERE activity_id = :activity_id AND is_active = 1");
    query.bindValue(":activity_id", activityId);
    
    if (!query.exec()) {
        qDebug() << "ERROR fetching alarms:" << query.lastError().text();
        return QJsonArray();
    }
    
    QJsonArray alarms;
    while (query.next()) {
        QJsonObject obj;
        obj["id"] = query.value("id").toInt();
        obj["activityId"] = query.value("activity_id").toInt();
        obj["alarmTime"] = query.value("alarm_time").toString();
        obj["isActive"] = query.value("is_active").toBool();
        alarms.append(obj);
    }
    
    return alarms;
}

bool AlarmManager::deleteAlarm(int alarmId)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("UPDATE alarms SET is_active = 0 WHERE id = :id");
    query.bindValue(":id", alarmId);
    
    if (!query.exec()) {
        qDebug() << "ERROR deleting alarm:" << query.lastError().text();
        return false;
    }
    
    m_activeAlarms.remove(alarmId);
    return true;
}

void AlarmManager::checkAlarms()
{
    QDateTime now = QDateTime::currentDateTime();
    
    QList<int> triggeredAlarms;
    for (auto it = m_activeAlarms.begin(); it != m_activeAlarms.end(); ++it) {
        if (it.value() <= now) {
            triggeredAlarms.append(it.key());
        }
    }
    
    // Trigger alarms
    for (int alarmId : triggeredAlarms) {
        QSqlQuery query(Database::instance().db());
        query.prepare(R"(
            SELECT a.id, a.title, al.activity_id
            FROM alarms al
            JOIN activities a ON al.activity_id = a.id
            WHERE al.id = :alarm_id
        )");
        query.bindValue(":alarm_id", alarmId);
        
        if (query.exec() && query.next()) {
            int activityId = query.value("activity_id").toInt();
            QString title = query.value("title").toString();
            
            showNotification(activityId, title);
            emit alarmTriggered(activityId, title);
        }
        
        // Deactivate alarm
        deleteAlarm(alarmId);
    }
}

void AlarmManager::onTimerTimeout()
{
    checkAlarms();
}

void AlarmManager::loadActiveAlarms()
{
    QSqlQuery query(Database::instance().db());
    query.exec("SELECT id, alarm_time FROM alarms WHERE is_active = 1");
    
    while (query.next()) {
        int id = query.value("id").toInt();
        QString timeStr = query.value("alarm_time").toString();
        QDateTime triggerTime = QDateTime::fromString(timeStr, Qt::ISODate);
        
        if (triggerTime > QDateTime::currentDateTime()) {
            m_activeAlarms[id] = triggerTime;
        }
    }
    
    qDebug() << "Loaded" << m_activeAlarms.size() << "active alarms";
}

void AlarmManager::showNotification(int activityId, const QString& title)
{
    qDebug() << "ALARM TRIGGERED! Activity" << activityId << ":" << title;
    // TODO: Show system notification (QSystemTrayIcon, native notifications, etc.)
}
```

---

### **4. HTTP Server (REST API)**

Create `backend/src/httpserver.h`:
```cpp
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QHttpServer>

class ActivityManager;
class AlarmManager;

class HttpServer : public QObject
{
    Q_OBJECT
    
public:
    explicit HttpServer(ActivityManager *activityMgr, AlarmManager *alarmMgr, QObject *parent = nullptr);
    bool start(quint16 port = 8080);
    void stop();
    
private:
    void setupRoutes();
    
    // Helper methods
    QHttpServerResponse jsonResponse(const QJsonObject& obj, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::Ok);
    QHttpServerResponse jsonResponse(const QJsonArray& arr, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::Ok);
    QHttpServerResponse errorResponse(const QString& message, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest);
    
    QHttpServer *m_server;
    ActivityManager *m_activityManager;
    AlarmManager *m_alarmManager;
};

#endif
```

Create `backend/src/httpserver.cpp`:
```cpp
#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

HttpServer::HttpServer(ActivityManager *activityMgr, AlarmManager *alarmMgr, QObject *parent)
    : QObject(parent)
    , m_activityManager(activityMgr)
    , m_alarmManager(alarmMgr)
{
    m_server = new QHttpServer(this);
    setupRoutes();
}

bool HttpServer::start(quint16 port)
{
    auto tcpServer = m_server->listen(QHostAddress::LocalHost, port);
    
    if (tcpServer) {
        qDebug() << "HTTP Server running on http://localhost:" << port;
        return true;
    } else {
        qDebug() << "ERROR: Failed to start HTTP server on port" << port;
        return false;
    }
}

void HttpServer::stop()
{
    // QHttpServer doesn't have explicit stop, just delete or let it go out of scope
    qDebug() << "HTTP Server stopped";
}

void HttpServer::setupRoutes()
{
    // CORS headers for all responses
    m_server->afterRequest([](QHttpServerResponse &&resp) {
        resp.addHeader("Access-Control-Allow-Origin", "*");
        resp.addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp.addHeader("Access-Control-Allow-Headers", "Content-Type");
        return std::move(resp);
    });
    
    // ============ ACTIVITY ROUTES ============
    
    // GET /api/activities - List all activities
    m_server->route("/api/activities", QHttpServerRequest::Method::Get,
        [this]() {
            QJsonArray activities = m_activityManager->getAllActivities();
            return jsonResponse(activities);
        }
    );
    
    // POST /api/activities - Create new activity
    m_server->route("/api/activities", QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest &request) {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            if (!doc.isObject()) {
                return errorResponse("Invalid JSON");
            }
            
            QJsonObject activity = m_activityManager->createActivity(doc.object());
            if (activity.contains("error")) {
                return errorResponse(activity["error"].toString());
            }
            
            return jsonResponse(activity, QHttpServerResponse::StatusCode::Created);
        }
    );
    
    // GET /api/activities/:id - Get single activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Get,
        [this](int id) {
            QJsonObject activity = m_activityManager->getActivityById(id);
            if (activity.contains("error")) {
                return errorResponse(activity["error"].toString(), QHttpServerResponse::StatusCode::NotFound);
            }
            return jsonResponse(activity);
        }
    );
    
    // PUT /api/activities/:id - Update activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Put,
        [this](int id, const QHttpServerRequest &request) {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            if (!doc.isObject()) {
                return errorResponse("Invalid JSON");
            }
            
            QJsonObject activity = m_activityManager->updateActivity(id, doc.object());
            if (activity.contains("error")) {
                return errorResponse(activity["error"].toString());
            }
            
            return jsonResponse(activity);
        }
    );
    
    // DELETE /api/activities/:id - Delete activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Delete,
        [this](int id) {
            bool success = m_activityManager->deleteActivity(id);
            if (!success) {
                return errorResponse("Failed to delete activity");
            }
            
            return jsonResponse(QJsonObject{{"message", "Activity deleted successfully"}});
        }
    );
    
    // GET /api/activities/upcoming - Get upcoming activities
    m_server->route("/api/activities/upcoming", QHttpServerRequest::Method::Get,
        [this]() {
            QJsonArray activities = m_activityManager->getUpcomingActivities();
            return jsonResponse(activities);
        }
    );
    
    // PATCH /api/activities/:id/complete - Mark as completed
    m_server->route("/api/activities/<arg>/complete", QHttpServerRequest::Method::Patch,
        [this](int id, const QHttpServerRequest &request) {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            bool completed = doc.object()["completed"].toBool();
            
            bool success = m_activityManager->markAsCompleted(id, completed);
            if (!success) {
                return errorResponse("Failed to update activity");
            }
            
            return jsonResponse(QJsonObject{{"message", "Activity updated successfully"}});
        }
    );
    
    // ============ ALARM ROUTES ============
    
    // POST /api/alarms - Create alarm
    m_server->route("/api/alarms", QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest &request) {
            QJsonDocument doc = QJsonDocument::fromJson(request.body());
            QJsonObject data = doc.object();
            
            int activityId = data["activityId"].toInt();
            QString alarmTime = data["alarmTime"].toString();
            
            QJsonObject alarm = m_alarmManager->createAlarm(activityId, alarmTime);
            if (alarm.contains("error")) {
                return errorResponse(alarm["error"].toString());
            }
            
            return jsonResponse(alarm, QHttpServerResponse::StatusCode::Created);
        }
    );
    
    // GET /api/alarms/activity/:id - Get alarms for activity
    m_server->route("/api/alarms/activity/<arg>", QHttpServerRequest::Method::Get,
        [this](int activityId) {
            QJsonArray alarms = m_alarmManager->getAlarmsForActivity(activityId);
            return jsonResponse(alarms);
        }
    );
    
    // DELETE /api/alarms/:id - Delete alarm
    m_server->route("/api/alarms/<arg>", QHttpServerRequest::Method::Delete,
        [this](int id) {
            bool success = m_alarmManager->deleteAlarm(id);
            if (!success) {
                return errorResponse("Failed to delete alarm");
            }
            
            return jsonResponse(QJsonObject{{"message", "Alarm deleted successfully"}});
        }
    );
    
    // OPTIONS handler for CORS preflight
    m_server->route("/api/*", QHttpServerRequest::Method::Options,
        [this]() {
            return QHttpServerResponse(QHttpServerResponse::StatusCode::NoContent);
        }
    );
    
    qDebug() << "HTTP routes configured";
}

QHttpServerResponse HttpServer::jsonResponse(const QJsonObject& obj, QHttpServerResponse::StatusCode code)
{
    QJsonDocument doc(obj);
    return QHttpServerResponse("application/json", doc.toJson(), code);
}

QHttpServerResponse HttpServer::jsonResponse(const QJsonArray& arr, QHttpServerResponse::StatusCode code)
{
    QJsonDocument doc(arr);
    return QHttpServerResponse("application/json", doc.toJson(), code);
}

QHttpServerResponse HttpServer::errorResponse(const QString& message, QHttpServerResponse::StatusCode code)
{
    QJsonObject obj;
    obj["error"] = message;
    return jsonResponse(obj, code);
}
```

---

### **5. Main Window**

Create `backend/src/mainwindow.h`:
```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>

class HttpServer;
class ActivityManager;
class AlarmManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    void setupWebView();
    
    QWebEngineView *m_webView;
    HttpServer *m_httpServer;
    ActivityManager *m_activityManager;
    AlarmManager *m_alarmManager;
};

#endif
```

Create `backend/src/mainwindow.cpp`:
```cpp
#include "mainwindow.h"
#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include "database.h"
#include <QWebEngineView>
#include <QUrl>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Daily Activity Reminder");
    resize(1280, 800);
    
    // Initialize database
    if (!Database::instance().initialize()) {
        qCritical() << "Failed to initialize database!";
        return;
    }
    
    // Initialize managers
    m_activityManager = new ActivityManager(this);
    m_alarmManager = new AlarmManager(this);
    
    // Start HTTP server
    m_httpServer = new HttpServer(m_activityManager, m_alarmManager, this);
    if (!m_httpServer->start(8080)) {
        qCritical() << "Failed to start HTTP server!";
        return;
    }
    
    // Setup UI
    setupWebView();
}

MainWindow::~MainWindow()
{
    if (m_httpServer) {
        m_httpServer->stop();
    }
}

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
```

---

### **6. Main Entry Point**

Create `backend/src/main.cpp`:
```cpp
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("DailyReminder");
    QCoreApplication::setApplicationName("Daily Activity Reminder");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
```

---

### **7. CMakeLists.txt**

Update `backend/CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.16)
project(DailyReminder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Core Widgets WebEngineWidgets HttpServer Sql QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets WebEngineWidgets HttpServer Sql)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/httpserver.cpp
    src/activitymanager.cpp
    src/alarmmanager.cpp
    src/database.cpp
)

set(HEADERS
    src/mainwindow.h
    src/httpserver.h
    src/activitymanager.h
    src/alarmmanager.h
    src/database.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::HttpServer
        Qt6::Sql
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::WebEngineWidgets
        Qt5::HttpServer
        Qt5::Sql
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
```

---

## 🎨 Frontend Implementation

### **1. API Client**

Create `frontend/lib/api.ts`:
```typescript
const API_BASE_URL = 'http://localhost:8080/api';

export interface Activity {
  id?: number;
  title: string;
  description?: string;
  scheduledTime: string;
  duration?: number;
  priority?: number;
  completed?: boolean;
  createdAt?: string;
  updatedAt?: string;
}

export interface Alarm {
  id?: number;
  activityId: number;
  alarmTime: string;
  isActive?: boolean;
}

class ApiClient {
  private async request<T>(endpoint: string, options?: RequestInit): Promise<T> {
    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
      ...options,
      headers: {
        'Content-Type': 'application/json',
        ...options?.headers,
      },
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.error || 'API request failed');
    }

    return response.json();
  }

  // Activity endpoints
  async getActivities(): Promise<Activity[]> {
    return this.request('/activities');
  }

  async getActivity(id: number): Promise<Activity> {
    return this.request(`/activities/${id}`);
  }

  async createActivity(activity: Omit<Activity, 'id' | 'createdAt' | 'updatedAt'>): Promise<Activity> {
    return this.request('/activities', {
      method: 'POST',
      body: JSON.stringify(activity),
    });
  }

  async updateActivity(id: number, activity: Partial<Activity>): Promise<Activity> {
    return this.request(`/activities/${id}`, {
      method: 'PUT',
      body: JSON.stringify(activity),
    });
  }

  async deleteActivity(id: number): Promise<void> {
    await this.request(`/activities/${id}`, { method: 'DELETE' });
  }

  async markCompleted(id: number, completed: boolean): Promise<void> {
    await this.request(`/activities/${id}/complete`, {
      method: 'PATCH',
      body: JSON.stringify({ completed }),
    });
  }

  async getUpcomingActivities(): Promise<Activity[]> {
    return this.request('/activities/upcoming');
  }

  // Alarm endpoints
  async createAlarm(alarm: Omit<Alarm, 'id' | 'isActive'>): Promise<Alarm> {
    return this.request('/alarms', {
      method: 'POST',
      body: JSON.stringify(alarm),
    });
  }

  async getAlarmsForActivity(activityId: number): Promise<Alarm[]> {
    return this.request(`/alarms/activity/${activityId}`);
  }

  async deleteAlarm(id: number): Promise<void> {
    await this.request(`/alarms/${id}`, { method: 'DELETE' });
  }
}

export const api = new ApiClient();
```

---

### **2. React Hooks**

Create `frontend/hooks/useActivities.ts`:
```typescript
'use client';

import { useState, useEffect } from 'react';
import { api, Activity } from '@/lib/api';

export function useActivities() {
  const [activities, setActivities] = useState<Activity[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  const fetchActivities = async () => {
    try {
      setLoading(true);
      const data = await api.getActivities();
      setActivities(data);
      setError(null);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to fetch activities');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchActivities();
  }, []);

  const addActivity = async (activity: Omit<Activity, 'id' | 'createdAt' | 'updatedAt'>) => {
    try {
      const newActivity = await api.createActivity(activity);
      setActivities([...activities, newActivity]);
      return newActivity;
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to create activity');
      throw err;
    }
  };

  const updateActivity = async (id: number, updates: Partial<Activity>) => {
    try {
      const updated = await api.updateActivity(id, updates);
      setActivities(activities.map(a => a.id === id ? updated : a));
      return updated;
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update activity');
      throw err;
    }
  };

  const deleteActivity = async (id: number) => {
    try {
      await api.deleteActivity(id);
      setActivities(activities.filter(a => a.id !== id));
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete activity');
      throw err;
    }
  };

  const markCompleted = async (id: number, completed: boolean) => {
    try {
      await api.markCompleted(id, completed);
      setActivities(activities.map(a => 
        a.id === id ? { ...a, completed } : a
      ));
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update activity');
      throw err;
    }
  };

  return {
    activities,
    loading,
    error,
    addActivity,
    updateActivity,
    deleteActivity,
    markCompleted,
    refresh: fetchActivities,
  };
}
```

---

## 🚀 How to Build and Run

### **Step 1: Install Dependencies**

```bash
# Install Qt HttpServer module
sudo dnf install qt6-qthttpserver-devel

# Install frontend dependencies
cd frontend
npm install
cd ..
```

### **Step 2: Development Mode**

**Terminal 1 - Start Next.js:**
```bash
cd frontend
npm run dev
```

**Terminal 2 - Build and run Qt:**
```bash
cd backend
mkdir build && cd build
cmake ..
cmake --build .
./DailyReminder
```

### **Step 3: Test the API**

```bash
# Test with curl
curl http://localhost:8080/api/activities

# Create activity
curl -X POST http://localhost:8080/api/activities \
  -H "Content-Type: application/json" \
  -d '{"title":"Meeting","scheduledTime":"2025-10-30T10:00:00"}'
```

---

## 📊 Summary

**What You Have Now:**

1. ✅ **Qt Backend** with QHttpServer providing REST API
2. ✅ **SQLite Database** for persistent storage
3. ✅ **Activity CRUD** operations
4. ✅ **Alarm System** with timers
5. ✅ **Next.js Frontend** communicating via HTTP
6. ✅ **Professional architecture** separating concerns
7. ✅ **Scalable design** - easy to add calendar, notifications, etc.

**Next Steps:**
- Add calendar view component
- Add system notifications
- Add recurring activities
- Add activity categories/tags
- Add statistics dashboard

Ready to start building! 🎉
