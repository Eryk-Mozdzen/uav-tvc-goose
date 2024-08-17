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
    QSlider *sliders[9];
    QLineEdit *displays[9];
    Position position;
    bool direction;

    void startTransmit();

public:
    Servos(Window *window, QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const msg_frame_sensor_t &sensor);
    void update(msg_frame_calibration_t &calibration) const;
};
