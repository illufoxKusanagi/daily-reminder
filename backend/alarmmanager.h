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

    void checkAlarms();  // Check and trigger alarms
    void reloadAlarms(); // Reload alarms from database (call after event changes)

signals:
    void alarmTriggered(const QString &eventId, const QString &title);

private slots:
    void onTimerTimeout();

private:
    QTimer *m_checkTimer;
    QMap<QString, QDateTime> m_activeAlarms; // eventId -> trigger time

    void loadActiveAlarms();
    void showNotification(const QString &eventId, const QString &title, const QString &category, const QString &startTime);
};

#endif