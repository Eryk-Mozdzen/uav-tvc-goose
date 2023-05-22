#include "altimeter.h"
#include <QDebug>

Altimeter::Altimeter(const QQmlApplicationEngine &engine, QObject *parent) : QObject{parent} {
    QObject *altimeter = engine.rootObjects().first()->findChild<QObject *>("batterAltimeterCol")->findChild<QObject *>("altimeter");

    if(!altimeter) {
        qDebug() << "cannot find altimeter object";
        return;
    }

	connect(this, SIGNAL(setPressure(QVariant)), altimeter, SLOT(setPressure(QVariant)));
    connect(this, SIGNAL(setDistance(QVariant)), altimeter, SLOT(setDistance(QVariant)));
    connect(this, SIGNAL(setAltitude(QVariant)), altimeter, SLOT(setAltitude(QVariant)));
}

void Altimeter::receive(const Transfer::FrameRX &frame) {
    switch(frame.id) {
        case Transfer::ID::TELEMETRY_SENSOR_PRESSURE: {
            float pressure;
            frame.getPayload(pressure);
            setPressure(pressure*0.01f);
        } break;
        case Transfer::ID::TELEMETRY_SENSOR_DISTANCE: {
            float distance;
            frame.getPayload(distance);
            setDistance(distance);
        } break;
        case Transfer::ID::TELEMETRY_ESTIMATION_ALTITUDE: {
            float altitude;
            frame.getPayload(altitude);
            setAltitude(altitude);
        } break;
        default: break;
    }
}
