#pragma once

#include <QWidget>
#include "interactive_map.h"
#include <QTimer>

class Window : public QWidget {
    Q_OBJECT

    InteractiveMap *map;
    QTimer timer;

private slots:
    void timerCallback();

public:
    Window();
};
