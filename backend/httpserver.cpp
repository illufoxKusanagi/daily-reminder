#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHttpHeaders>
#include <QHostAddress>
#include <QDebug>

HttpServer::HttpServer(ActivityManager *activityMgr, AlarmManager *alarmMgr, QObject *parent)
    : QObject(parent), m_activityManager(activityMgr), m_alarmManager(alarmMgr), m_port(0)
{
    m_server = new QHttpServer(this);
    setupRoutes();
}

bool HttpServer::start(quint16 port)
{
    m_tcpServer = std::make_unique<QTcpServer>();

    if (port == 0)
    {
        if (!m_tcpServer->listen(QHostAddress::LocalHost))
        {
            qCritical() << "Failed to listen on localhost:" << m_tcpServer->errorString();
            return false;
        }
    }
    else
    {
        if (!m_tcpServer->listen(QHostAddress::LocalHost, port))
        {
            qCritical() << "Failed to listen on port" << port << ":" << m_tcpServer->errorString();
            return false;
        }
    }

    if (!m_server->bind(m_tcpServer.get()))
    {
        qCritical() << "Failed to bind HTTP server to TCP server";
        return false;
    }

    m_port = m_tcpServer->serverPort();
    qInfo() << "🚀 Daily Reminder Backend Server is running on http://localhost:" << m_port;

    m_tcpServer.release();
    return true;
}

void HttpServer::stop()
{
    if (m_server)
    {
        qInfo() << "🛑 Stopping HTTP server...";
    }
}

quint16 HttpServer::getPort() const
{
    return m_port;
}

QJsonObject HttpServer::parseRequestBody(const QHttpServerRequest &request)
{
    QByteArray body = request.body();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(body, &error);

    if (error.error != QJsonParseError::NoError)
    {
        qWarning() << "JSON parse error:" << error.errorString();
        return QJsonObject();
    }

    return doc.object();
}

void HttpServer::setupRoutes()
{
    auto addCorsHeaders = [](QHttpServerResponse response)
    {
        QHttpHeaders headers = response.headers();
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowOrigin, "*");
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowMethods, "GET, POST, PUT, DELETE, PATCH, OPTIONS");
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowHeaders, "Content-Type, Authorization");
        headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
        headers.append("Access-Control-Max-Age", "86400");
        response.setHeaders(std::move(headers));
        return response;
    };

    // ============ OPTIONS HANDLERS (CORS Preflight) ============

    m_server->route("/api/event", QHttpServerRequest::Method::Options,
                    [addCorsHeaders]()
                    {
                        qDebug() << "📋 OPTIONS /api/event";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/event/<arg>", QHttpServerRequest::Method::Options,
                    [addCorsHeaders](const QString &)
                    {
                        qDebug() << "📋 OPTIONS /api/event/:id";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    // ============ EVENT ROUTES (Calendar API) ============

    m_server->route("/api/event", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders]()
                    {
                        qDebug() << "🔍 GET /api/event";
                        QJsonArray events = m_activityManager->getAllActivities();
                        return addCorsHeaders(jsonResponse(events));
                    });

    m_server->route("/api/event", QHttpServerRequest::Method::Post,
                    [this, addCorsHeaders](const QHttpServerRequest &request)
                    {
                        qDebug() << "📝 POST /api/event";
                        qDebug() << "📦 Request body:" << request.body();
                        QJsonObject data = parseRequestBody(request);
                        qDebug() << "🔧 Parsed JSON:" << data;

                        QJsonObject event = m_activityManager->createActivity(data);
                        if (event.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(event["error"].toString()));
                        }

                        if (data.value("isReminderEnabled").toBool(false))
                        {
                            m_alarmManager->reloadAlarms();
                            qInfo() << "⏰ Reloaded alarms after creating event with reminder";
                        }

                        return addCorsHeaders(jsonResponse(event, QHttpServerResponse::StatusCode::Created));
                    });

    // GET /api/event/:id - Get single event
    m_server->route("/api/event/<arg>", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders](const QString &id)
                    {
                        qDebug() << "🔍 GET /api/event/" << id;
                        QJsonObject event = m_activityManager->getActivityById(id);
                        if (event.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(event["error"].toString(), QHttpServerResponse::StatusCode::NotFound));
                        }
                        return addCorsHeaders(jsonResponse(event));
                    });

    // PUT /api/event/:id - Update event
    m_server->route("/api/event/<arg>", QHttpServerRequest::Method::Put,
                    [this, addCorsHeaders](const QString &id, const QHttpServerRequest &request)
                    {
                        qDebug() << "✏️ PUT /api/event/" << id;
                        QJsonObject data = parseRequestBody(request);

                        QJsonObject event = m_activityManager->updateActivity(id, data);
                        if (event.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(event["error"].toString()));
                        }

                        if (data.contains("isReminderEnabled") || data.contains("reminderTime"))
                        {
                            m_alarmManager->reloadAlarms();
                            qInfo() << "⏰ Reloaded alarms after updating event reminder";
                        }

                        return addCorsHeaders(jsonResponse(event));
                    });

    // DELETE /api/event/:id - Delete event
    m_server->route("/api/event/<arg>", QHttpServerRequest::Method::Delete,
                    [this, addCorsHeaders](const QString &id)
                    {
                        qDebug() << "🗑️ DELETE /api/event/" << id;
                        bool success = m_activityManager->deleteActivity(id);
                        if (!success)
                        {
                            return addCorsHeaders(errorResponse("Failed to delete event"));
                        }

                        m_alarmManager->reloadAlarms();
                        qInfo() << "⏰ Reloaded alarms after deleting event";

                        return addCorsHeaders(jsonResponse(QJsonObject{{"message", "Event deleted successfully"}}));
                    });

    m_server->route("/status", QHttpServerRequest::Method::Get,
                    [addCorsHeaders]()
                    {
                        qDebug() << "🔍 GET /status";
                        QJsonObject response;
                        response["status"] = "Daily Reminder Backend is running!";
                        response["service"] = "Qt Daily Reminder HTTP API";
                        return addCorsHeaders(QHttpServerResponse(response));
                    });

    qDebug() << "✅ HTTP routes configured";
}

QHttpServerResponse HttpServer::jsonResponse(const QJsonObject &obj, QHttpServerResponse::StatusCode code)
{
    QJsonDocument doc(obj);
    return QHttpServerResponse("application/json", doc.toJson(), code);
}

QHttpServerResponse HttpServer::jsonResponse(const QJsonArray &arr, QHttpServerResponse::StatusCode code)
{
    QJsonDocument doc(arr);
    return QHttpServerResponse("application/json", doc.toJson(), code);
}

QHttpServerResponse HttpServer::errorResponse(const QString &message, QHttpServerResponse::StatusCode code)
{
    QJsonObject obj;
    obj["error"] = message;
    return jsonResponse(obj, code);
}