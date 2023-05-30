#include "scene.h"
#include <QDebug>


Scene::Scene(const QQmlApplicationEngine &engine, QObject *parent) : QObject{parent} {
    QObject *scene = engine.rootObjects().first()->findChild<QObject *>("scene");

    if(!scene) {
        qDebug() << "cannot find scene object";
        return;
    }

	connect(this, SIGNAL(setState(QVariant)), scene, SLOT(setState(QVariant)));
    connect(this, SIGNAL(setCommand(QVariant)), scene, SLOT(setCommand(QVariant)));
    connect(this, SIGNAL(setAcceleration(QVariant)), scene, SLOT(setAcceleration(QVariant)));
    connect(this, SIGNAL(setGyration(QVariant)), scene, SLOT(setGyration(QVariant)));
    connect(this, SIGNAL(setMagnitude(QVariant)), scene, SLOT(setMagnitude(QVariant)));
}

void Scene::receive(const Transfer::FrameRX &frame) {
    switch(frame.id) {
        case Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE: {
            QQuaternion orientation;
            frame.getPayload(orientation);
            auto rpy = orientation.toEulerAngles();
            setState(rpy);
        } break;
        case Transfer::ID::CONTROL_ATTITUDE_SETPOINT: {
            QQuaternion orientation;
            frame.getPayload(orientation);
            auto rpy = orientation.toEulerAngles();
            setCommand(rpy);
        } break;
        case Transfer::ID::TELEMETRY_SENSOR_ACCELERATION:{
            QVector3D acceleration;
            frame.getPayload(acceleration);
            setAcceleration(acceleration);
        } break;
        case Transfer::ID::TELEMETRY_SENSOR_GYRATION:{
            QVector3D gyration;
            frame.getPayload(gyration);
            setGyration(gyration);
        } break;
        case Transfer::ID::TELEMETRY_SENSOR_MAGNETIC_FIELD:{
            QVector3D magnitude;
            frame.getPayload(magnitude);
            setMagnitude(magnitude);
        } break;

        default: break;
    }
}
