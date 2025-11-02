#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QSystemTrayIcon>

class AlarmManager : public QObject
{
    Q_OBJECT

public:
    explicit AlarmManager(QObject *parent = nullptr);
    void setSystemTrayIcon(QSystemTrayIcon *trayIcon);

    void checkAlarms();
    void reloadAlarms();

signals:
    void alarmTriggered(const QString &eventId, const QString &title);

private slots:
    void onTimerTimeout();

private:
    QTimer *m_checkTimer;
    QMap<QString, QDateTime> m_activeAlarms;
    QSystemTrayIcon *m_trayIcon;

    void loadActiveAlarms();
    void showNotification(const QString &eventId, const QString &title, const QString &category, const QString &startTime);
    void playAlarmSound();
};

#endif