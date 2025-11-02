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

    // Events table (personal daily activities)
    QString createEvents = R"(
        CREATE TABLE IF NOT EXISTS events (
            id TEXT PRIMARY KEY,
            category TEXT NOT NULL,
            start_date TEXT NOT NULL,
            end_date TEXT NOT NULL,
            title TEXT NOT NULL,
            color TEXT NOT NULL,
            description TEXT DEFAULT '',
            reminder_time TEXT,
            is_reminder_enabled INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createEvents))
    {
        qDebug() << "ERROR creating events table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database tables created successfully";
    return true;
}