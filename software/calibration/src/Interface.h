#pragma once

#include <QWidget>

#include "common/protocol/msg.h"

class Interface : public QWidget {
    QString name;

public:
    Interface(QString name, QWidget *parent=nullptr) : QWidget{parent}, name{name} {

    }

    QString getName() const {
        return name;
    }

    virtual Interface * create() const = 0;
    virtual void receive(const msg_frame_sensor_t &sensor) = 0;
    virtual void update(msg_frame_calibration_t &calibration) const = 0;
};
