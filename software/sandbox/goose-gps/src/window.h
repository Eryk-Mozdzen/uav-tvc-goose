#pragma once

#include <QWidget>
#include "map.h"

class Window : public QWidget {
    Q_OBJECT

    Map *map;

public:
    Window();
};
