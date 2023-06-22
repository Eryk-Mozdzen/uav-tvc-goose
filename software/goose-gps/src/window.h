#pragma once

#include <QWidget>
#include "interactive_map.h"

class Window : public QWidget {
    Q_OBJECT

    InteractiveMap *map;

public:
    Window();
};
