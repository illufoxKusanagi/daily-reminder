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

    QJsonObject createActivity(const QJsonObject &data);
    QJsonArray getAllActivities();
    QJsonObject getActivityById(const QString &id);
    QJsonObject updateActivity(const QString &id, const QJsonObject &data);
    bool deleteActivity(const QString &id);

    QJsonArray getActivitiesByDate(const QString &date);
    QJsonArray getUpcomingActivities();
    bool markAsCompleted(const QString &id, bool completed);

signals:
    void activityCreated(const QString &id);
    void activityUpdated(const QString &id);
    void activityDeleted(const QString &id);

private:
    QString currentDateTime() const;
    QJsonObject activityFromQuery(class QSqlQuery &query);
};

#endif