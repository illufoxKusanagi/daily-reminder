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

QJsonObject AlarmManager::createAlarm(int activityId, const QString &alarmTime)
{
    QSqlQuery query(Database::instance().db());

    query.prepare(R"(
        INSERT INTO alarms (activity_id, alarm_time, is_active)
        VALUES (:activity_id, :alarm_time, 1)
    )");

    query.bindValue(":activity_id", activityId);
    query.bindValue(":alarm_time", alarmTime);

    if (!query.exec())
    {
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

    if (!query.exec())
    {
        qDebug() << "ERROR fetching alarms:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray alarms;
    while (query.next())
    {
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

    if (!query.exec())
    {
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
    for (auto it = m_activeAlarms.begin(); it != m_activeAlarms.end(); ++it)
    {
        if (it.value() <= now)
        {
            triggeredAlarms.append(it.key());
        }
    }

    // Trigger alarms
    for (int alarmId : triggeredAlarms)
    {
        QSqlQuery query(Database::instance().db());
        query.prepare(R"(
            SELECT a.id, a.title, al.activity_id
            FROM alarms al
            JOIN activities a ON al.activity_id = a.id
            WHERE al.id = :alarm_id
        )");
        query.bindValue(":alarm_id", alarmId);

        if (query.exec() && query.next())
        {
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

    while (query.next())
    {
        int id = query.value("id").toInt();
        QString timeStr = query.value("alarm_time").toString();
        QDateTime triggerTime = QDateTime::fromString(timeStr, Qt::ISODate);

        if (triggerTime > QDateTime::currentDateTime())
        {
            m_activeAlarms[id] = triggerTime;
        }
    }

    qDebug() << "Loaded" << m_activeAlarms.size() << "active alarms";
}

void AlarmManager::showNotification(int activityId, const QString &title)
{
    qDebug() << "ALARM TRIGGERED! Activity" << activityId << ":" << title;
    // TODO: Show system notification (QSystemTrayIcon, native notifications, etc.)
}