#pragma once

#include <QObject>

class Gamepad : public QObject {
    Q_OBJECT

    QVector<float> analogs;
    QVector<bool> buttons;

public:
    enum Analog {
        LX,
        LY,
        RX,
        RY,
        L2,
        R2
    };

    enum Button {
        CROSS_LEFT,
        CROSS_RIGHT,
        CROSS_UP,
        CROSS_DOWN,
        CIRCLE_A,
        CIRCLE_B,
        CIRCLE_X,
        CIRCLE_Y,
        L1,
        R1,
        L3,
        R3,
        SELECT,
        START,
        HOME
    };

    Gamepad(QObject *parent=nullptr);

    float get(Analog analog) const;
    bool get(Button button) const;
};
