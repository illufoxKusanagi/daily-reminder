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

QJsonObject ActivityManager::createActivity(const QJsonObject &data)
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

    if (!query.exec())
    {
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

    if (!query.exec("SELECT * FROM activities ORDER BY scheduled_time DESC"))
    {
        qDebug() << "ERROR fetching activities:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray activities;
    while (query.next())
    {
        activities.append(activityFromQuery(query));
    }

    return activities;
}

QJsonObject ActivityManager::getActivityById(int id)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM activities WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next())
    {
        qDebug() << "ERROR fetching activity:" << query.lastError().text();
        return QJsonObject{{"error", "Activity not found"}};
    }

    return activityFromQuery(query);
}

QJsonObject ActivityManager::updateActivity(int id, const QJsonObject &data)
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

    if (!query.exec())
    {
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

    if (!query.exec())
    {
        qDebug() << "ERROR deleting activity:" << query.lastError().text();
        return false;
    }

    emit activityDeleted(id);
    return true;
}

QJsonArray ActivityManager::getActivitiesByDate(const QString &date)
{
    QSqlQuery query(Database::instance().db());
    query.prepare("SELECT * FROM activities WHERE DATE(scheduled_time) = :date");
    query.bindValue(":date", date);

    if (!query.exec())
    {
        qDebug() << "ERROR fetching activities by date:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray activities;
    while (query.next())
    {
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

    if (!query.exec())
    {
        qDebug() << "ERROR fetching upcoming activities:" << query.lastError().text();
        return QJsonArray();
    }

    QJsonArray activities;
    while (query.next())
    {
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

    if (!query.exec())
    {
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

QJsonObject ActivityManager::activityFromQuery(QSqlQuery &query)
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