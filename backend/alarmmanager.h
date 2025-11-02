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

    QJsonObject createAlarm(int activityId, const QString &alarmTime);
    QJsonArray getAlarmsForActivity(int activityId);
    bool deleteAlarm(int alarmId);
    void checkAlarms(); // Called periodically

signals:
    void alarmTriggered(int activityId, const QString &title);

private slots:
    void onTimerTimeout();

private:
    QTimer *m_checkTimer;
    QMap<int, QDateTime> m_activeAlarms; // alarmId -> trigger time

    void loadActiveAlarms();
    void showNotification(int activityId, const QString &title);
};

#endif