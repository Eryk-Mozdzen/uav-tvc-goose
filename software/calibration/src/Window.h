#pragma once

#include <vector>

#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>

#include "Interface.h"

class Window : public QWidget {
    Q_OBJECT

    msg_frame_calibration_t calibration = {
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0},
        {500, 1500, 2500, 500, 1500, 2500, 500, 1500, 2500}
    };
    std::vector<Interface *> interfaces;
    Interface *current;
    QTextEdit *calibration_text;
    QHBoxLayout *interface_layout;

    void setCurrent(Interface *interface);

signals:
    void transmit(const uint8_t id, const QByteArray &payload);

public slots:
    void receive(const uint8_t id, const double time, const QByteArray &payload);

public:
    Window(QWidget *parent = nullptr);
};
