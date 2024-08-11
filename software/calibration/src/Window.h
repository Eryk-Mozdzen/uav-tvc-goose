#pragma once

#include <vector>

#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>

#include "common/qt/Serial.h"
#include "common/qt/Network.h"
#include "Interface.h"

class Window : public QWidget {
    Q_OBJECT

    protocol_calibration_t calibration = {
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0},
        {500, 1500, 2500, 500, 1500, 2500, 500, 1500, 2500, 500, 1500, 2500}
    };
    std::vector<Interface *> interfaces;
    Interface *current;
    QTextEdit *calibration_text;
    QHBoxLayout *interface_layout;

    void setCurrent(Interface *interface);

signals:
    void transmit(const uint8_t id, const void *payload, const uint32_t size);

public slots:
    void receive(const uint8_t id, const void *payload, const uint32_t size);

public:
    Window(QWidget *parent = nullptr);
};
