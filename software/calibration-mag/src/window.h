#pragma once

#include <vector>

#include "viewer.h"
#include "usb.h"
#include "utils.h"
#include "telnet.h"

class Window : public QWidget {
    Q_OBJECT

    Viewer raw;
    Viewer calibrated;
    USB usb;
    Telnet telnet;

    std::vector<Sample> samples;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
