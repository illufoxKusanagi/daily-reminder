#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QMimeDatabase>

HttpServer::HttpServer(ActivityManager *activityMgr, AlarmManager *alarmMgr, QObject *parent)
    : QObject(parent), m_activityManager(activityMgr), m_alarmManager(alarmMgr), m_port(0)
{
    m_server = new QHttpServer(this);
    m_frontendPath = findFrontendPath();
    setupRoutes();
    setupStaticRoutes();
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
    auto addCorsHeaders = [](QHttpServerResponse response) -> QHttpServerResponse
    {
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

QString HttpServer::findFrontendPath()
{
    // Try to find frontend build directory
    QStringList frontendPaths = {
        QCoreApplication::applicationDirPath() + "/frontend",                // Relative to binary
        QCoreApplication::applicationDirPath() + "/../share/daily-reminder", // System install
        QDir::currentPath() + "/frontend/out",                               // Development
        "/usr/share/daily-reminder",                                         // Linux system install
        "/usr/local/share/daily-reminder"                                    // Alternative install
    };

    for (const QString &path : frontendPaths)
    {
        QString indexPath = path + "/index.html";
        if (QFile::exists(indexPath))
        {
            qInfo() << "✅ Found frontend at:" << path;
            return path;
        }
    }

    qWarning() << "❌ Frontend not found! Tried paths:";
    for (const QString &path : frontendPaths)
    {
        qWarning() << "  -" << path + "/index.html";
    }
    return QString();
}

QString HttpServer::getMimeType(const QString &filePath)
{
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);
    QString mime = mimeType.name();

    // Override for specific extensions that might not be detected correctly
    if (filePath.endsWith(".js"))
        return "application/javascript";
    if (filePath.endsWith(".mjs"))
        return "application/javascript";
    if (filePath.endsWith(".css"))
        return "text/css";
    if (filePath.endsWith(".html"))
        return "text/html";
    if (filePath.endsWith(".json"))
        return "application/json";
    if (filePath.endsWith(".svg"))
        return "image/svg+xml";
    if (filePath.endsWith(".woff2"))
        return "font/woff2";
    if (filePath.endsWith(".woff"))
        return "font/woff";

    return mime;
}

void HttpServer::setupStaticRoutes()
{
    if (m_frontendPath.isEmpty())
    {
        qWarning() << "⚠️  Static file serving disabled - frontend path not found";
        return;
    }

    // Serve static files with proper routing
    m_server->route("<arg>", [this](const QUrl &url)
                    {
        QString path = url.path();
        
        // Skip API routes
        if (path.startsWith("/api/"))
        {
            return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound);
        }
        
        // Default to index.html for root
        if (path == "/" || path.isEmpty())
        {
            path = "/index.html";
        }

        QString filePath = m_frontendPath + path;
        QFileInfo fileInfo(filePath);

        // If file doesn't exist and no extension, try .html
        if (!fileInfo.exists() && !path.contains("."))
        {
            filePath = m_frontendPath + path + ".html";
            fileInfo.setFile(filePath);
        }

        // Try to open and serve the file
        if (fileInfo.exists() && fileInfo.isFile())
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray content = file.readAll();
                file.close();
                
                QString mimeType = getMimeType(filePath);
                
                QHttpServerResponse response(mimeType.toUtf8(), content);
                
                return response;
            }
        }

        // For client-side routing, return index.html
        if (!path.contains("."))
        {
            QFile indexFile(m_frontendPath + "/index.html");
            if (indexFile.open(QIODevice::ReadOnly))
            {
                QByteArray content = indexFile.readAll();
                indexFile.close();
                return QHttpServerResponse("text/html", content);
            }
        }

        // 404 for missing files
        return QHttpServerResponse(QHttpServerResponse::StatusCode::NotFound); });

    qInfo() << "✅ Static file serving enabled from:" << m_frontendPath;
}