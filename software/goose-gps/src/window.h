#pragma once

#include <QMainWindow>
#include <QWebEngineView>
#include <QTimer>

class Window : public QMainWindow {
    Q_OBJECT

    QWebEngineView *view;
    bool loaded;
    QTimer timer;

    void addMarker(double latitude, double longitude);

private slots:
    void pageLoaded();
    void timerCallback();

public:
    explicit Window(QWidget *parent = nullptr);
};
