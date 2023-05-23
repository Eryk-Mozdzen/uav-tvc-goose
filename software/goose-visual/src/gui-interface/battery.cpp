#include "battery.h"
#include <QDebug>

Battery::Battery(const QQmlApplicationEngine &engine, QObject *parent) : QObject{parent} {
    QObject *battery = engine.rootObjects().first()->findChild<QObject *>("batterAltimeterCol")->findChild<QObject *>("battery");

    if(!battery) {
        qDebug() << "cannot find battery object";
        return;
    }

	connect(this, SIGNAL(setVoltage(QVariant)), battery, SLOT(setVoltage(QVariant)));
    connect(this, SIGNAL(setCurrent(QVariant)), battery, SLOT(setCurrent(QVariant)));
    connect(this, SIGNAL(setLevel(QVariant)), battery, SLOT(setLevel(QVariant)));
}

void Battery::receive(const Transfer::FrameRX &frame) {
    switch(frame.id) {
        case Transfer::ID::TELEMETRY_SENSOR_VOLTAGE: {
            float voltage;
            frame.getPayload(voltage);
            setVoltage(voltage);
        } break;
        case Transfer::ID::TELEMETRY_SENSOR_CURRENT: {
            float current;
            frame.getPayload(current);
            setCurrent(current);
        } break;
        case Transfer::ID::TELEMETRY_ESTIMATION_BATTERY_LEVEL: {
            float level;
            frame.getPayload(level);
            setLevel(level*0.01f);
        } break;
        default: break;
    }
}
