#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Database &Database::instance()
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
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool Database::initialize()
{
    // Set database path
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataDir))
    {
        dir.mkpath(dataDir);
    }

    QString dbPath = dataDir + "/activities.db";
    m_db.setDatabaseName(dbPath);

    qDebug() << "Database path:" << dbPath;

    if (!m_db.open())
    {
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

    if (!query.exec(createActivities))
    {
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

    if (!query.exec(createAlarms))
    {
        qDebug() << "ERROR creating alarms table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database tables created successfully";
    return true;
}