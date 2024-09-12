#pragma once

#include <QSlider>
#include <QLineEdit>
#include <QLabel>

#include "Interface.h"
#include "Window.h"

class Esc : public Interface {
    Window *window;
    QSlider *sliders[2];
    QLineEdit *displays[2];
    QLabel *speed;
    bool state;

    void startTransmit();

public:
    Esc(Window *window, QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const msg_frame_sensor_t &sensor);
    void update(msg_frame_calibration_t &calibration) const;
};
