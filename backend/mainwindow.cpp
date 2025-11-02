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
#include <QDebug>
#include <qcoreapplication.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Daily Activity Reminder");
    resize(1280, 800);

    if (!Database::instance().initialize())
    {
        qCritical() << "Failed to initialize database!";
        return;
    }

    // Initialize managers
    m_activityManager = new ActivityManager(this);
    m_alarmManager = new AlarmManager(this);

    // Start HTTP server
    m_httpServer = new HttpServer(m_activityManager, m_alarmManager, this);
    if (!m_httpServer->start(8080))
    {
        qCritical() << "Failed to start HTTP server!";
        return;
    }

    // Setup UI
    setupWebView();
}

MainWindow::~MainWindow()
{
    if (m_httpServer)
    {
        m_httpServer->stop();
    }
}

void MainWindow::setupWebView()
{
    m_webView = new QWebEngineView(this);
    setCentralWidget(m_webView);

    // Enable developer tools and local access
    QWebEngineSettings *settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);

    // Load from Next.js dev server
    qDebug() << "Loading frontend from http://localhost:3000";
    m_webView->load(QUrl("http://localhost:3000"));
}