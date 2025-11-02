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
        QCoreApplication app(argc, argv);
        QCoreApplication::setOrganizationName("DailyReminder");
        QCoreApplication::setApplicationName("Daily Activity Reminder");

        qInfo() << "🚀 Starting Daily Reminder Backend (Headless Mode)";

        if (!Database::instance().initialize())
        {
            qCritical() << "❌ Failed to initialize database!";
            return 1;
        }

        ActivityManager activityManager;
        AlarmManager alarmManager;

        HttpServer server(&activityManager, &alarmManager);
        if (server.start(port))
        {
            qInfo() << "✅ Backend Server started on port" << server.getPort();
            qInfo() << "📋 Available endpoints:";
            qInfo() << "   GET    /status";
            qInfo() << "   GET    /api/event";
            qInfo() << "   POST   /api/event";
            qInfo() << "   GET    /api/event/:id";
            qInfo() << "   PUT    /api/event/:id";
            qInfo() << "   DELETE /api/event/:id";
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
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-web-security --allow-running-insecure-content");
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