#include "actuators.h"
#include <QDebug>
#include <QList>
#include <array>


Actuators::Actuators(const QQmlApplicationEngine &engine, QObject *parent) : QObject{parent} {
    QObject *actuators = engine.rootObjects().first()->findChild<QObject *>("actuators");

    if(!actuators) {
        qDebug() << "cannot find actuators object";
        return;
    }

	connect(this, SIGNAL(setActuators(QVariant)), actuators, SLOT(setActuators(QVariant)));
    connect(this, SIGNAL(setThrottle(QVariant)), actuators, SLOT(setThrottle(QVariant)));
}

void Actuators::receive(const Transfer::FrameRX &frame) {
    switch(frame.id) {
        case Transfer::ID::TELEMETRY_INPUTS_SERVOS: {
            std::array<float, 4> actuators = {0.0, 0.0, 0.0, 0.0};
            frame.getPayload(actuators);
            std::for_each(actuators.begin(), actuators.end(), [](float &x) { x = qRadiansToDegrees(x); });
            QVariantList qActuators;
            for (const auto& actuator : actuators) {
                qActuators.append(QVariant(actuator));
            }
            setActuators(qActuators);
        } break;
        case Transfer::ID::CONTROL_MOTOR_THROTTLE: {
            float throttle = 0.0;
            frame.getPayload(throttle);
            setThrottle(int(throttle));
        } break;
        default: break;
    }
}
