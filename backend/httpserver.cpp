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
    // Helper function to add CORS headers to any response
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

    m_server->route("/api/activities", QHttpServerRequest::Method::Options,
                    [addCorsHeaders]()
                    {
                        qDebug() << "📋 OPTIONS /api/activities";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Options,
                    [addCorsHeaders](int)
                    {
                        qDebug() << "📋 OPTIONS /api/activities/:id";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/activities/upcoming", QHttpServerRequest::Method::Options,
                    [addCorsHeaders]()
                    {
                        qDebug() << "📋 OPTIONS /api/activities/upcoming";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/activities/<arg>/complete", QHttpServerRequest::Method::Options,
                    [addCorsHeaders](int)
                    {
                        qDebug() << "📋 OPTIONS /api/activities/:id/complete";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/alarms", QHttpServerRequest::Method::Options,
                    [addCorsHeaders]()
                    {
                        qDebug() << "📋 OPTIONS /api/alarms";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/alarms/<arg>", QHttpServerRequest::Method::Options,
                    [addCorsHeaders](int)
                    {
                        qDebug() << "📋 OPTIONS /api/alarms/:id";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    m_server->route("/api/alarms/activity/<arg>", QHttpServerRequest::Method::Options,
                    [addCorsHeaders](int)
                    {
                        qDebug() << "📋 OPTIONS /api/alarms/activity/:id";
                        return addCorsHeaders(QHttpServerResponse(QHttpServerResponse::StatusCode::Ok));
                    });

    // ============ ACTIVITY ROUTES ============

    // GET /api/activities - List all activities
    m_server->route("/api/activities", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders]()
                    {
                        qDebug() << "🔍 GET /api/activities";
                        QJsonArray activities = m_activityManager->getAllActivities();
                        return addCorsHeaders(jsonResponse(activities));
                    });

    // POST /api/activities - Create new activity
    m_server->route("/api/activities", QHttpServerRequest::Method::Post,
                    [this, addCorsHeaders](const QHttpServerRequest &request)
                    {
                        qDebug() << "📝 POST /api/activities";
                        qDebug() << "📦 Request body:" << request.body();
                        QJsonObject data = parseRequestBody(request);
                        qDebug() << "🔧 Parsed JSON:" << data;

                        QJsonObject activity = m_activityManager->createActivity(data);
                        if (activity.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(activity["error"].toString()));
                        }

                        return addCorsHeaders(jsonResponse(activity, QHttpServerResponse::StatusCode::Created));
                    });

    // GET /api/activities/upcoming - Get upcoming activities (BEFORE :id route!)
    m_server->route("/api/activities/upcoming", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders]()
                    {
                        qDebug() << "📅 GET /api/activities/upcoming";
                        QJsonArray activities = m_activityManager->getUpcomingActivities();
                        return addCorsHeaders(jsonResponse(activities));
                    });

    // GET /api/activities/:id - Get single activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders](int id)
                    {
                        qDebug() << "🔍 GET /api/activities/" << id;
                        QJsonObject activity = m_activityManager->getActivityById(id);
                        if (activity.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(activity["error"].toString(), QHttpServerResponse::StatusCode::NotFound));
                        }
                        return addCorsHeaders(jsonResponse(activity));
                    });

    // PUT /api/activities/:id - Update activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Put,
                    [this, addCorsHeaders](int id, const QHttpServerRequest &request)
                    {
                        qDebug() << "✏️ PUT /api/activities/" << id;
                        QJsonObject data = parseRequestBody(request);

                        QJsonObject activity = m_activityManager->updateActivity(id, data);
                        if (activity.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(activity["error"].toString()));
                        }

                        return addCorsHeaders(jsonResponse(activity));
                    });

    // DELETE /api/activities/:id - Delete activity
    m_server->route("/api/activities/<arg>", QHttpServerRequest::Method::Delete,
                    [this, addCorsHeaders](int id)
                    {
                        qDebug() << "🗑️ DELETE /api/activities/" << id;
                        bool success = m_activityManager->deleteActivity(id);
                        if (!success)
                        {
                            return addCorsHeaders(errorResponse("Failed to delete activity"));
                        }

                        return addCorsHeaders(jsonResponse(QJsonObject{{"message", "Activity deleted successfully"}}));
                    });

    // PATCH /api/activities/:id/complete - Mark as completed
    m_server->route("/api/activities/<arg>/complete", QHttpServerRequest::Method::Patch,
                    [this, addCorsHeaders](int id, const QHttpServerRequest &request)
                    {
                        qDebug() << "✅ PATCH /api/activities/" << id << "/complete";
                        QJsonObject data = parseRequestBody(request);
                        bool completed = data["completed"].toBool();

                        bool success = m_activityManager->markAsCompleted(id, completed);
                        if (!success)
                        {
                            return addCorsHeaders(errorResponse("Failed to update activity"));
                        }

                        return addCorsHeaders(jsonResponse(QJsonObject{{"message", "Activity updated successfully"}}));
                    });

    // ============ ALARM ROUTES ============

    // POST /api/alarms - Create alarm
    m_server->route("/api/alarms", QHttpServerRequest::Method::Post,
                    [this, addCorsHeaders](const QHttpServerRequest &request)
                    {
                        qDebug() << "⏰ POST /api/alarms";
                        QJsonObject data = parseRequestBody(request);

                        int activityId = data["activityId"].toInt();
                        QString alarmTime = data["alarmTime"].toString();

                        QJsonObject alarm = m_alarmManager->createAlarm(activityId, alarmTime);
                        if (alarm.contains("error"))
                        {
                            return addCorsHeaders(errorResponse(alarm["error"].toString()));
                        }

                        return addCorsHeaders(jsonResponse(alarm, QHttpServerResponse::StatusCode::Created));
                    });

    // GET /api/alarms/activity/:id - Get alarms for activity
    m_server->route("/api/alarms/activity/<arg>", QHttpServerRequest::Method::Get,
                    [this, addCorsHeaders](int activityId)
                    {
                        qDebug() << "🔍 GET /api/alarms/activity/" << activityId;
                        QJsonArray alarms = m_alarmManager->getAlarmsForActivity(activityId);
                        return addCorsHeaders(jsonResponse(alarms));
                    });

    // DELETE /api/alarms/:id - Delete alarm
    m_server->route("/api/alarms/<arg>", QHttpServerRequest::Method::Delete,
                    [this, addCorsHeaders](int id)
                    {
                        qDebug() << "🗑️ DELETE /api/alarms/" << id;
                        bool success = m_alarmManager->deleteAlarm(id);
                        if (!success)
                        {
                            return addCorsHeaders(errorResponse("Failed to delete alarm"));
                        }

                        return addCorsHeaders(jsonResponse(QJsonObject{{"message", "Alarm deleted successfully"}}));
                    });

    // Simple status endpoint
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