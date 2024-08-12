#pragma once

#include <QWidget>

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

signals:
    void transmit(const uint8_t id, const void *payload, const uint32_t size);

private slots:
    void receive(const uint8_t id, const void *payload, const uint32_t size);

public:
    Window(QWidget *parent = nullptr);
};
