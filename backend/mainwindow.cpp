#include "mainwindow.h"
#include "httpserver.h"
#include "activitymanager.h"
#include "alarmmanager.h"
#include "database.h"
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QCloseEvent>
#include <QAction>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_trayIcon(nullptr), m_trayMenu(nullptr)
{
    setWindowTitle("Daily Activity Reminder");
    resize(1280, 800);

    if (!Database::instance().initialize())
    {
        qCritical() << "Failed to initialize database!";
        return;
    }

    m_activityManager = new ActivityManager(this);
    m_alarmManager = new AlarmManager(this);

    m_httpServer = new HttpServer(m_activityManager, m_alarmManager, this);
    if (!m_httpServer->start(8080))
    {
        qCritical() << "Failed to start HTTP server!";
        return;
    }

    setupSystemTray();

    // Connect alarm manager to system tray for notifications
    m_alarmManager->setSystemTrayIcon(m_trayIcon);

    setupWebView();
}

MainWindow::~MainWindow()
{
    if (m_httpServer)
    {
        m_httpServer->stop();
    }
    if (m_trayIcon)
    {
        delete m_trayIcon;
    }
}

void MainWindow::setupSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(this);
    QIcon trayIconImage = QIcon::fromTheme("appointment-soon");
    if (trayIconImage.isNull())
    {
        trayIconImage = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    }
    m_trayIcon->setIcon(trayIconImage);
    m_trayIcon->setToolTip("Daily Reminder - Running in background");

    m_trayMenu = new QMenu(this);
    QAction *showAction = m_trayMenu->addAction("Show Window");
    QAction *quitAction = m_trayMenu->addAction("Quit");

    connect(showAction, &QAction::triggered, this, &MainWindow::showWindow);
    connect(quitAction, &QAction::triggered, this, &MainWindow::quitApplication);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();

    qInfo() << "💡 System tray icon enabled - application can run in background";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isVisible())
    {
        hide();
        event->ignore();
        m_trayIcon->showMessage("Daily Reminder",
                                "Application minimized to tray. Alarms will continue to work.",
                                QSystemTrayIcon::Information, 2000);
    }
    else
    {
        event->accept();
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
    {
        showWindow();
    }
}

void MainWindow::showWindow()
{
    show();
    raise();
    activateWindow();
}

void MainWindow::quitApplication()
{
    if (m_trayIcon)
    {
        m_trayIcon->hide();
    }
    QApplication::quit();
}

void MainWindow::setupWebView()
{
    m_webView = new QWebEngineView(this);
    setCentralWidget(m_webView);

    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);

    // Try to find frontend build directory
    QStringList frontendPaths = {
        QCoreApplication::applicationDirPath() + "/frontend/index.html",
        QCoreApplication::applicationDirPath() + "/../share/daily-reminder/index.html",
        QDir::currentPath() + "/frontend/out/index.html",
        "/usr/share/daily-reminder/index.html",
        "/usr/local/share/daily-reminder/index.html"};

    QString frontendPath;
    for (const QString &path : frontendPaths)
    {
        if (QFile::exists(path))
        {
            frontendPath = path;
            qDebug() << "✅ Found frontend at:" << frontendPath;
            break;
        }
    }

    if (!frontendPath.isEmpty())
    {
        QUrl url = QUrl::fromLocalFile(frontendPath);
        qDebug() << "Loading frontend from:" << url.toString();
        m_webView->load(url);
    }
    else
    {
        qWarning() << "❌ Frontend not found! Tried paths:";
        for (const QString &path : frontendPaths)
        {
            qWarning() << "  -" << path;
        }
        qDebug() << "Falling back to http://localhost:3000";
        m_webView->load(QUrl("http://localhost:3000"));
    }
}