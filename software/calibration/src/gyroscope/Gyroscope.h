#pragma once

#include <QLineEdit>

#include "Interface.h"

class Gyroscope : public Interface {
    int n = 0;
    double mean[3] = {0, 0, 0};

    QLineEdit *line[4];

public:
    Gyroscope(QWidget *parent = nullptr);

    Interface * create() const;
    void receive(const msg_frame_sensor_t &sensor);
    void update(msg_frame_calibration_t &calibration) const;
};
