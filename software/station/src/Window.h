#pragma once

#include <QWidget>
#include <QSettings>

#include "common/protocol/msg.h"
#include "Form.h"
#include "LiveChart.h"
#include "Gamepad.h"

class Window : public QWidget {
    Q_OBJECT

    Form *others;
    LiveChart *position;
    LiveChart *attitude;
    LiveChart *throttle;
    LiveChart *fins;
    LiveChart *angular_vel;
    LiveChart *linear_vel;

    Gamepad gamepad;
    QSettings settings;

    msg_frame_setpoint_t last_setpoint = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    };

signals:
    void transmit(const uint8_t id, const QByteArray &payload);

private slots:
    void receive(const uint8_t id, const double time, const QByteArray &payload);

public:
    Window(QWidget *parent = nullptr);
};
