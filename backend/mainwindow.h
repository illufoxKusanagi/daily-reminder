#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>

class HttpServer;
class ActivityManager;
class AlarmManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupWebView();

    QWebEngineView *m_webView;
    HttpServer *m_httpServer;
    ActivityManager *m_activityManager;
    AlarmManager *m_alarmManager;
};

#endif