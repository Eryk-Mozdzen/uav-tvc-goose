#pragma once

#include <QWidget>
#include "usb.h"
#include "transfer.h"
#include "axis.h"

class Window : public QWidget {
    Q_OBJECT

    Axis *axis[3];
    USB usb;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
