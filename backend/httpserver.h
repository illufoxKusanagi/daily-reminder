#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QTcpServer>
#include <QHttpServerRequest>
#include <memory>

class ActivityManager;
class AlarmManager;

class HttpServer : public QObject
{
    Q_OBJECT

public:
    explicit HttpServer(ActivityManager *activityMgr, AlarmManager *alarmMgr, QObject *parent = nullptr);
    bool start(quint16 port = 8080);
    void stop();
    quint16 getPort() const;

private:
    void setupRoutes();
    QJsonObject parseRequestBody(const QHttpServerRequest &request);

    // Helper methods
    QHttpServerResponse jsonResponse(const QJsonObject &obj, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::Ok);
    QHttpServerResponse jsonResponse(const QJsonArray &arr, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::Ok);
    QHttpServerResponse errorResponse(const QString &message, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest);

    QHttpServer *m_server;
    std::unique_ptr<QTcpServer> m_tcpServer;
    ActivityManager *m_activityManager;
    AlarmManager *m_alarmManager;
    quint16 m_port;
};

#endif