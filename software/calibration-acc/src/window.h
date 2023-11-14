#pragma once

#include <QWidget>
#include <QLineEdit>

#include "usb.h"
#include "telnet.h"
#include "transfer.h"
#include "utils.h"

class Window : public QWidget {
    Q_OBJECT

    Sample current;
    Sample samples[6];
    QLineEdit *current_line[3];
    QLineEdit *corrected_line[6][3];

    USB usb;
    Telnet telnet;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
