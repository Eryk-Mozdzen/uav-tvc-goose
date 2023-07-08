#pragma once

#include <QWidget>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include "form.h"
#include "usb.h"
#include "telnet.h"
#include "gamepad.h"

class Window : public QWidget {
    Q_OBJECT

    static constexpr float pi = 3.1415f;
    static constexpr float rad2deg = 180.f/pi;
    static constexpr float deg2rad = pi/180.f;

    widgets::Form *source;
    widgets::Form *setpoint;
    widgets::Form *process;
    widgets::Form *actuators;
    widgets::Form *others;
    widgets::Form *power;
    widgets::Form *position;
    widgets::Form *velocity;
    widgets::Form *acceleration;
    widgets::Form *memory;

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
