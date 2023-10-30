#pragma once

#include <QWidget>

#include "usb.h"
#include "telnet.h"

class Window : public QWidget {
    Q_OBJECT

    USB usb;
    Telnet telnet;

    enum Position {
        MIN,
        ZERO,
        MAX
    };

    Position position;

signals:
    void transmit(const Transfer::FrameTX &frame);

public:
    Window(QWidget *parent = nullptr);
};
