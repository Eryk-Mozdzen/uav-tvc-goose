#pragma once

#include <QWidget>

#include "usb.h"
#include "telnet.h"

class Window : public QWidget {
    Q_OBJECT

    USB usb;
    Telnet telnet;

    bool swipe_state;

public:
    Window(QWidget *parent = nullptr);
};
