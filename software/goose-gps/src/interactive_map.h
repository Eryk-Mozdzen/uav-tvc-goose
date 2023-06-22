#pragma once

#include <QWidget>
#include <QWebEngineView>

class InteractiveMap : public QWidget {
    Q_OBJECT

    QWebEngineView view;
    bool loaded;

private slots:
    void pageLoaded();

public:
    explicit InteractiveMap(QWidget *parent = nullptr);

    void mark(double latitude, double longitude);
};
