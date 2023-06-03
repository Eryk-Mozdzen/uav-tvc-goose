#pragma once

#include <QWidget>
#include <QTimer>
#include <QComboBox>
#include <QPushButton>
#include "form.h"
#include "usb.h"
#include "telnet.h"

class Window : public QWidget {
    Q_OBJECT

    QComboBox *freq;
    QPushButton *cmd_start;
    QPushButton *cmd_land;

    widgets::Form source;
    widgets::Form setpoint;
    widgets::Form process;
    widgets::Form actuators;
    widgets::Form others;

    QTimer timer;

    USB usb;
	Telnet telnet;

signals:
    void transmit(const Transfer::FrameTX &frame);

private slots:
    void sendSetpoint();
    void sendCommand(Transfer::Command cmd);
    void freqChanged(QString value);
    void sourceChanged(int index, QString value);
    void frameReceived(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
