#pragma once

#include <QWidget>
#include <QWebEngineView>

class Map : public QWidget {
    Q_OBJECT

    QWebEngineView view;
    bool loaded;

private slots:
    void pageLoaded();

public:
    explicit Map(QWidget *parent = nullptr);

    void mark(double latitude, double longitude);
};
