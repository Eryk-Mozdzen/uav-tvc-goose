#pragma once

#include <QWidget>
#include "form.h"
#include "livechart.h"
#include "usb.h"
#include "telnet.h"
#include "gamepad.h"

class Window : public QWidget {
    Q_OBJECT

    static constexpr float pi = 3.1415f;
    static constexpr float rad2deg = 180.f/pi;
    static constexpr float deg2rad = pi/180.f;

    widgets::Form *source;
    widgets::Form *others;
    widgets::Form *power;

    widgets::LiveChart *altitude;
    widgets::LiveChart *attitude;
    widgets::LiveChart *throttle;
    widgets::LiveChart *fins;
    widgets::LiveChart *angular_vel;
    widgets::LiveChart *vertical_vel;

    USB usb;
	Telnet telnet;
    Gamepad gamepad;

signals:
    void transmit(const Transfer::FrameTX &frame);

private slots:
    void receiveCallback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
