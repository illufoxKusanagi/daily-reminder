#include "alarmmanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QFile>

AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent), m_trayIcon(nullptr)
{
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &AlarmManager::onTimerTimeout);
    m_checkTimer->start(10000);

    loadActiveAlarms();

    qInfo() << "⏰ AlarmManager started - checking every 30 seconds";
}

void AlarmManager::setSystemTrayIcon(QSystemTrayIcon *trayIcon)
{
    m_trayIcon = trayIcon;
}

void AlarmManager::reloadAlarms()
{
    loadActiveAlarms();
}

void AlarmManager::checkAlarms()
{
    QDateTime now = QDateTime::currentDateTime();

    static int checkCount = 0;
    checkCount++;
    if (checkCount % 10 == 1)
    {
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
        QString eventId = query.value("id").toString();
        QString title = query.value("title").toString();
        QString category = query.value("category").toString();
        QString reminderTime = query.value("reminder_time").toString();
        QString startDate = query.value("start_date").toString();

        qInfo() << "🔔 ALARM TRIGGERED!" << "Event:" << title << "Category:" << category << "ID:" << eventId;

        showNotification(eventId, title, category, startDate);
        emit alarmTriggered(eventId, title);

        QSqlQuery updateQuery(Database::instance().db());
        updateQuery.prepare("UPDATE events SET is_reminder_enabled = 0 WHERE id = :id");
        updateQuery.bindValue(":id", eventId);
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
        loadActiveAlarms();
    }
}

void AlarmManager::onTimerTimeout()
{
    checkAlarms();
}

void AlarmManager::loadActiveAlarms()
{
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
        QString id = query.value("id").toString();
        QString title = query.value("title").toString();
        QString timeStr = query.value("reminder_time").toString();
        QDateTime triggerTime = QDateTime::fromString(timeStr, Qt::ISODate);

        if (triggerTime.isValid())
        {
            m_activeAlarms[id] = triggerTime;
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

    // Play alarm sound
    playAlarmSound();

#if defined(Q_OS_LINUX)
    // On Linux, always use notify-send (more reliable than QSystemTrayIcon)
    QProcess::startDetached("notify-send",
                            QStringList() << "-u" << "critical"
                                          << "-i" << "appointment-soon"
                                          << "-t" << "10000" // Show for 10 seconds
                                          << QString("Daily Reminder: %1").arg(title)
                                          << message);
    qInfo() << "✅ Linux notification sent via notify-send";

#elif defined(Q_OS_MACOS)
    QString script = QString("display notification \"%1\" with title \"Daily Reminder: %2\" sound name \"default\"")
                         .arg(message.replace("\"", "\\\""), title.replace("\"", "\\\""));
    QProcess::startDetached("osascript", QStringList() << "-e" << script);
    qInfo() << "✅ macOS notification sent via osascript";

#elif defined(Q_OS_WIN)
    // On Windows, use QSystemTrayIcon
    if (m_trayIcon && m_trayIcon->isVisible() && m_trayIcon->supportsMessages())
    {
        m_trayIcon->showMessage(
            QString("Daily Reminder: %1").arg(title),
            message,
            QSystemTrayIcon::Information,
            10000 // Show for 10 seconds
        );
        qInfo() << "✅ Windows notification sent via system tray";
    }
    else
    {
        qWarning() << "⚠️ System tray not available on Windows";
    }
#endif

// Also show in system tray as backup (if available and not already shown)
#if !defined(Q_OS_WIN)
    if (m_trayIcon && m_trayIcon->isVisible() && m_trayIcon->supportsMessages())
    {
        m_trayIcon->showMessage(
            QString("Daily Reminder: %1").arg(title),
            message,
            QSystemTrayIcon::Information,
            10000);
    }
#endif
}

void AlarmManager::playAlarmSound()
{
#if defined(Q_OS_LINUX)
    // Try multiple sound files in order of preference
    QStringList soundFiles = {
        "/usr/share/sounds/freedesktop/stereo/alarm-clock-elapsed.oga",
        "/usr/share/sounds/freedesktop/stereo/complete.oga",
        "/usr/share/sounds/freedesktop/stereo/bell.oga",
        "/usr/share/sounds/sound-icons/xylofon.wav"};

    for (const QString &soundFile : soundFiles)
    {
        if (QFile::exists(soundFile))
        {
            // Try paplay first (PulseAudio)
            if (QProcess::startDetached("paplay", QStringList() << soundFile))
            {
                qInfo() << "🔊 Playing alarm sound:" << soundFile;
                return;
            }
            // Fallback to aplay (ALSA)
            if (QProcess::startDetached("aplay", QStringList() << soundFile))
            {
                qInfo() << "🔊 Playing alarm sound:" << soundFile;
                return;
            }
        }
    }

    // Last resort: system beep
    QProcess::startDetached("beep", QStringList() << "-f" << "1000" << "-l" << "500" << "-r" << "3");
    qInfo() << "🔊 Using system beep";

#elif defined(Q_OS_MACOS)
    QProcess::startDetached("afplay", QStringList() << "/System/Library/Sounds/Glass.aiff");
    qInfo() << "🔊 Playing macOS system sound";

#elif defined(Q_OS_WIN)
    // Windows: Play system sound
    QProcess::startDetached("powershell", QStringList() << "-Command" << "[console]::beep(1000,500)");
    qInfo() << "🔊 Playing Windows beep";
#endif
}