#pragma once

#include <QSlider>
#include <QLineEdit>

#include "Interface.h"
#include "Window.h"

class Servos : public Interface {
    enum Position {
        MIN,
        MAX,
        ZERO
    };

    Window *window;
    QSlider *sliders[12];
    QLineEdit *displays[12];
    Position position;
    bool direction;

public:
    Servos(Window *window, QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const protocol_readings_t &readings);
    void update(protocol_calibration_t &calibration) const;
};
