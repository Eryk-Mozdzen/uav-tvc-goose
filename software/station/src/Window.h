#pragma once

#include <QWidget>
#include <QSettings>

#include "Form.h"
#include "LiveChart.h"
#include "Gamepad.h"
#include "Visualizer.h"

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
    Visualizer visualizer;
    QSettings settings;

signals:
    void transmit(const uint8_t id, const QByteArray &payload);

private slots:
    void receive(const uint8_t id, const QByteArray &payload);

public:
    Window(QWidget *parent = nullptr);
};
