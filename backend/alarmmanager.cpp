#include "alarmmanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QApplication>

#if defined(Q_OS_WIN)
#include <QSystemTrayIcon>
#elif defined(Q_OS_LINUX)
#include <QProcess>
#elif defined(Q_OS_MACOS)
#include <QProcess>
#endif

AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent)
{
    // Check alarms every 30 seconds for better responsiveness
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &AlarmManager::onTimerTimeout);
    m_checkTimer->start(30000); // 30 seconds

    loadActiveAlarms();

    qInfo() << "⏰ AlarmManager started - checking every 30 seconds";
}

void AlarmManager::reloadAlarms()
{
    loadActiveAlarms();
}

void AlarmManager::checkAlarms()
{
    QDateTime now = QDateTime::currentDateTime();

    // Debug: Show that we're checking
    static int checkCount = 0;
    checkCount++;
    if (checkCount % 10 == 1)
    { // Log every 10th check to avoid spam
        qInfo() << "⏰ Checking alarms... (check #" << checkCount << ")";
    }

    QSqlQuery query(Database::instance().db());
    query.prepare(R"(
        SELECT id, title, category, reminder_time, start_date
        FROM events
        WHERE is_reminder_enabled = 1
        AND reminder_time IS NOT NULL
        AND datetime(reminder_time) <= datetime('now', 'localtime')
    )");

    if (!query.exec())
    {
        qWarning() << "❌ Failed to check alarms:" << query.lastError().text();
        return;
    }

    int triggeredCount = 0;
    while (query.next())
    {
        QString eventId = query.value("id").toString(); // Fixed: ID is TEXT, not INTEGER
        QString title = query.value("title").toString();
        QString category = query.value("category").toString();
        QString reminderTime = query.value("reminder_time").toString();
        QString startDate = query.value("start_date").toString();

        qInfo() << "🔔 ALARM TRIGGERED!" << "Event:" << title << "Category:" << category << "ID:" << eventId;

        showNotification(eventId, title, category, startDate);
        emit alarmTriggered(eventId, title); // Now using QString for eventId

        // Disable the reminder so it doesn't trigger again
        QSqlQuery updateQuery(Database::instance().db());
        updateQuery.prepare("UPDATE events SET is_reminder_enabled = 0 WHERE id = :id");
        updateQuery.bindValue(":id", eventId); // Use QString, not int
        if (!updateQuery.exec())
        {
            qWarning() << "❌ Failed to disable reminder for event" << eventId << ":" << updateQuery.lastError().text();
        }
        else
        {
            qInfo() << "✅ Disabled reminder for event" << eventId;
        }

        triggeredCount++;
    }

    if (triggeredCount > 0)
    {
        qInfo() << "✅ Triggered" << triggeredCount << "alarm(s)";
        // Reload active alarms after triggering to update the count
        loadActiveAlarms();
    }
}

void AlarmManager::onTimerTimeout()
{
    checkAlarms();
}

void AlarmManager::loadActiveAlarms()
{
    // Query events table for upcoming reminders using SQLite datetime functions
    QSqlQuery query(Database::instance().db());
    query.prepare(R"(
        SELECT id, title, reminder_time
        FROM events
        WHERE is_reminder_enabled = 1
        AND reminder_time IS NOT NULL
        AND datetime(reminder_time) > datetime('now')
    )");

    if (!query.exec())
    {
        qWarning() << "❌ Failed to load active alarms:" << query.lastError().text();
        return;
    }

    m_activeAlarms.clear();
    int count = 0;

    while (query.next())
    {
        QString id = query.value("id").toString(); // Fixed: ID is TEXT
        QString title = query.value("title").toString();
        QString timeStr = query.value("reminder_time").toString();
        QDateTime triggerTime = QDateTime::fromString(timeStr, Qt::ISODate);

        if (triggerTime.isValid())
        {
            m_activeAlarms[id] = triggerTime; // Map now uses QString keys
            count++;
        }
    }

    qInfo() << "📋 Loaded" << count << "active alarm(s)";
}

void AlarmManager::showNotification(const QString &eventId, const QString &title, const QString &category, const QString &startTime)
{
    QString message = QString("Event: %1\nCategory: %2\nTime: %3").arg(title, category, startTime);

    qInfo() << "🔔 ================================";
    qInfo() << "🔔 ALARM NOTIFICATION";
    qInfo() << "🔔 Event ID:" << eventId;
    qInfo() << "🔔 Title:" << title;
    qInfo() << "🔔 Category:" << category;
    qInfo() << "🔔 Start Time:" << startTime;
    qInfo() << "🔔 ================================";

#if defined(Q_OS_LINUX)
    // Use notify-send on Linux
    QProcess::startDetached("notify-send",
                            QStringList() << "-u" << "critical"
                                          << "-i" << "appointment-soon"
                                          << QString("Daily Reminder: %1").arg(title)
                                          << message);

#elif defined(Q_OS_MACOS)
    // Use osascript for macOS notifications
    QString script = QString("display notification \"%1\" with title \"Daily Reminder: %2\" sound name \"default\"")
                         .arg(message.replace("\"", "\\\""), title.replace("\"", "\\\""));
    QProcess::startDetached("osascript", QStringList() << "-e" << script);

#elif defined(Q_OS_WIN)
    // Windows: Could use QSystemTrayIcon or Windows Toast notifications
    // For now, just log (you can add Windows-specific notification later)
    qInfo() << "💡 To enable Windows notifications, integrate QSystemTrayIcon or Windows Toast API";

#else
    qInfo() << "⚠️ Platform-specific notifications not configured for this OS";
#endif

    // Also try to show application notification if running with GUI
    if (QApplication::instance())
    {
        // Could show QMessageBox or custom dialog here if needed
        qInfo() << "📱 GUI is available - could show in-app notification";
    }
}