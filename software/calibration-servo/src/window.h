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
        MAX,
        ZERO
    };

    Position position;
    bool direction;

private slots:
    void callback(Transfer::FrameRX frame);

signals:
    void transmit(const Transfer::FrameTX &frame);

public:
    Window(QWidget *parent = nullptr);
};
