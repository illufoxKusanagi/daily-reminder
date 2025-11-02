#include "mainwindow.h"
#include "database.h"
#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Parse command line arguments
    bool headless = false;
    quint16 port = 8080;

    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);
        if (arg == "--headless")
        {
            headless = true;
        }
        else if (arg.startsWith("--port="))
        {
            port = arg.mid(7).toInt();
        }
    }

    if (headless)
    {
        // Headless mode: Backend server only (no GUI)
        QCoreApplication app(argc, argv);
        QCoreApplication::setOrganizationName("DailyReminder");
        QCoreApplication::setApplicationName("Daily Activity Reminder");

        qInfo() << "🚀 Starting Daily Reminder Backend (Headless Mode)";

        // Initialize database
        if (!Database::instance().initialize())
        {
            qCritical() << "❌ Failed to initialize database!";
            return 1;
        }

        // Create managers
        ActivityManager activityManager;
        AlarmManager alarmManager;

        // Start HTTP server
        HttpServer server(&activityManager, &alarmManager);
        if (server.start(port))
        {
            qInfo() << "✅ Backend Server started on port" << server.getPort();
            qInfo() << "📋 Available endpoints:";
            qInfo() << "   GET    /status";
            qInfo() << "   GET    /api/activities";
            qInfo() << "   POST   /api/activities";
            qInfo() << "   GET    /api/activities/:id";
            qInfo() << "   PUT    /api/activities/:id";
            qInfo() << "   DELETE /api/activities/:id";
            qInfo() << "   GET    /api/activities/upcoming";
            qInfo() << "   PATCH  /api/activities/:id/complete";
            qInfo() << "   POST   /api/alarms";
            qInfo() << "   GET    /api/alarms/activity/:id";
            qInfo() << "   DELETE /api/alarms/:id";
            qInfo() << "";
            qInfo() << "💡 Usage:";
            qInfo() << "   --headless        Run backend only (no GUI)";
            qInfo() << "   --port=8080       Set backend port";
            return app.exec();
        }
        else
        {
            qCritical() << "❌ Failed to start server on port" << port;
            return 1;
        }
    }
    else
    {
        // GUI mode: Qt Application with WebEngine
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
        QApplication app(argc, argv);

        QCoreApplication::setOrganizationName("DailyReminder");
        QCoreApplication::setApplicationName("Daily Activity Reminder");
        QCoreApplication::setApplicationVersion("1.0.0");

        qInfo() << "🚀 Starting Daily Reminder (Desktop Mode)";

        MainWindow window;
        window.show();

        return app.exec();
    }
}