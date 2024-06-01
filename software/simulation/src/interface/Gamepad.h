#pragma once

#include <mutex>
#include <array>
#include <QObject>
#include <drake/systems/framework/leaf_system.h>

class Gamepad : public QObject, public drake::systems::LeafSystem<double> {
    Q_OBJECT

    std::array<float, 6> analog;
    std::array<bool, 15> digital;
    std::unique_ptr<std::mutex> mutex;

    enum Analog {
        LX,
        LY,
        RX,
        RY,
        L2,
        R2
    };

    enum Digital {
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

    void EvalAnalog(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const;

private slots:
    void updateA(const Analog input, const float value);
    void updateD(const Digital input, const bool value);

public:
    Gamepad(QObject *parent=nullptr);
};
