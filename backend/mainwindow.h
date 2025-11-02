#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QSystemTrayIcon>
#include <QMenu>

class HttpServer;
class ActivityManager;
class AlarmManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void quitApplication();

private:
    void setupWebView();
    void setupSystemTray();

    QWebEngineView *m_webView;
    HttpServer *m_httpServer;
    ActivityManager *m_activityManager;
    AlarmManager *m_alarmManager;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
};

#endif