#pragma once

#include "viewer.h"
#include "usb.h"
#include "utils.h"
#include "telnet.h"
#include <QVector>

class Window : public QWidget {
    Q_OBJECT

    Viewer *raw;
    Viewer *calibrated;
    USB usb;
    Telnet telnet;

    QVector<Sample> samples;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
