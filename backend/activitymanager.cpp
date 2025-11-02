#include "activitymanager.h"
#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUuid>
#include <QDebug>

ActivityManager::ActivityManager(QObject *parent)
    : QObject(parent)
{
}

QJsonObject ActivityManager::createActivity(const QJsonObject &data)
{
    QSqlQuery query(Database::instance().db());

    // Generate UUID for event id
    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    query.prepare(R"(
        INSERT INTO events (id, category, start_date, end_date, title, color, description, reminder_time, is_reminder_enabled)
        VALUES (:id, :category, :start_date, :end_date, :title, :color, :description, :reminder_time, :is_reminder_enabled)
    )");

    query.bindValue(":id", id);
    query.bindValue(":category", data["category"].toString());
    query.bindValue(":start_date", data["startDate"].toString());
    query.bindValue(":end_date", data["endDate"].toString());
    query.bindValue(":title", data["title"].toString());
    query.bindValue(":color", data["color"].toString());
    query.bindValue(":description", data["description"].toString(""));

    // Handle optional reminder time (can be NULL)
    if (data.contains("reminderTime") && !data["reminderTime"].toString().isEmpty())
    {
        query.bindValue(":reminder_time", data["reminderTime"].toString());
    }
    else
    {
        query.bindValue(":reminder_time", QVariant(QVariant::String));
    }

    query.bindValue(":is_reminder_enabled", data["isReminderEnabled"].toBool(false) ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "ERROR creating event:" << query.lastError().text();
        qDebug() << "Data received:" << data;
        return QJsonObject{{"error", "Failed to create event"}};
    }

    emit activityCreated(id);

    return getActivityById(id);
}

QJsonArray ActivityManager::getAllActivities()
{
    QSqlQuery query(Database::instance().db());

    if (!query.exec("SELECT * FROM events ORDER BY start_date ASC"))
    {
        qDebug() << "ERROR fetching events:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray events;
    while (query.next())
    {
        events.append(activityFromQuery(query));
    }

    return events;
}

QJsonObject ActivityManager::getActivityById(const QString &id)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM events WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "ERROR fetching event:" << query.lastError().text();
        return QJsonObject{{"error", "Event not found"}};
    }

    return activityFromQuery(query);
}

QJsonObject ActivityManager::updateActivity(const QString &id, const QJsonObject &data)
{
    QSqlQuery query(Database::instance().db());

    query.prepare(R"(
        UPDATE events SET
            category = :category,
            start_date = :start_date,
            end_date = :end_date,
            title = :title,
            color = :color,
            description = :description,
            reminder_time = :reminder_time,
            is_reminder_enabled = :is_reminder_enabled
        WHERE id = :id
    )");

    query.bindValue(":id", id);
    query.bindValue(":category", data["category"].toString());
    query.bindValue(":start_date", data["startDate"].toString());
    query.bindValue(":end_date", data["endDate"].toString());
    query.bindValue(":title", data["title"].toString());
    query.bindValue(":color", data["color"].toString());
    query.bindValue(":description", data["description"].toString(""));

    // Handle optional reminder time (can be NULL)
    if (data.contains("reminderTime") && !data["reminderTime"].toString().isEmpty())
    {
        query.bindValue(":reminder_time", data["reminderTime"].toString());
    }
    else
    {
        query.bindValue(":reminder_time", QVariant(QVariant::String));
    }

    query.bindValue(":is_reminder_enabled", data["isReminderEnabled"].toBool(false) ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "ERROR updating event:" << query.lastError().text();
        return QJsonObject{{"error", "Failed to update event"}};
    }

    emit activityUpdated(id);
    return getActivityById(id);
}

bool ActivityManager::deleteActivity(const QString &id)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("DELETE FROM events WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << "ERROR deleting event:" << query.lastError().text();
        return false;
    }

    emit activityDeleted(id);
    return true;
}

QJsonArray ActivityManager::getActivitiesByDate(const QString &date)
{
    QSqlQuery query(Database::instance().db());
    query.prepare(R"(
        SELECT * FROM events 
        WHERE DATE(start_date) <= :date AND DATE(end_date) >= :date
        ORDER BY start_date ASC
    )");
    query.bindValue(":date", date);

    if (!query.exec())
    {
        qDebug() << "ERROR fetching events by date:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray events;
    while (query.next())
    {
        events.append(activityFromQuery(query));
    }

    return events;
}

QJsonArray ActivityManager::getUpcomingActivities()
{
    QSqlQuery query(Database::instance().db());
    query.prepare(R"(
        SELECT * FROM events 
        WHERE start_date >= :now
        ORDER BY start_date ASC
        LIMIT 10
    )");
    query.bindValue(":now", currentDateTime());

    if (!query.exec())
    {
        qDebug() << "ERROR fetching upcoming events:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray events;
    while (query.next())
    {
        events.append(activityFromQuery(query));
    }

    return events;
}

bool ActivityManager::markAsCompleted(const QString &id, bool completed)
{
    // This method is deprecated for calendar events
    // Keeping for backward compatibility but does nothing
    qDebug() << "markAsCompleted called but not implemented for events";
    return true;
}

QString ActivityManager::currentDateTime() const
{
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

QJsonObject ActivityManager::activityFromQuery(QSqlQuery &query)
{
    QJsonObject obj;
    obj["id"] = query.value("id").toString();
    obj["category"] = query.value("category").toString();
    obj["startDate"] = query.value("start_date").toString();
    obj["endDate"] = query.value("end_date").toString();
    obj["title"] = query.value("title").toString();
    obj["color"] = query.value("color").toString();
    obj["description"] = query.value("description").toString();
    obj["reminderTime"] = query.value("reminder_time").toString();
    obj["isReminderEnabled"] = query.value("is_reminder_enabled").toBool();
    return obj;
}