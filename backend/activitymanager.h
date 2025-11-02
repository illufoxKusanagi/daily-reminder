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
    QJsonObject createActivity(const QJsonObject &data);
    QJsonArray getAllActivities();
    QJsonObject getActivityById(int id);
    QJsonObject updateActivity(int id, const QJsonObject &data);
    bool deleteActivity(int id);

    // Additional queries
    QJsonArray getActivitiesByDate(const QString &date);
    QJsonArray getUpcomingActivities();
    bool markAsCompleted(int id, bool completed);

signals:
    void activityCreated(int id);
    void activityUpdated(int id);
    void activityDeleted(int id);

private:
    QString currentDateTime() const;
    QJsonObject activityFromQuery(class QSqlQuery &query);
};

#endif