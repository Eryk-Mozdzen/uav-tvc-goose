#pragma once

#include <QMainWindow>
#include <QWebEngineView>
#include <QTimer>

class Window : public QMainWindow {
    Q_OBJECT

    QWebEngineView *view;
    QTimer timer;

    void addMarker(double latitude, double longitude);
    void toCenter();

private slots:
    void timerCallback();

public:
    explicit Window(QWidget *parent = nullptr);
};
