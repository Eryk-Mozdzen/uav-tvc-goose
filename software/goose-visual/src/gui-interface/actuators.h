/**
 * @file battery.h
 * @author Eryk Możdżeń (ermozdzen@@gmail.com)
 * @brief Implementation of batter interface class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QVector3D>
#include <QQuaternion>
#include <QtMath>
#include "transfer.h"

/**
 * @brief Actuators interface class
 *
 */
class Actuators : public QObject {
    Q_OBJECT

signals:

    void setThrottle(QVariant throttle);

    void setActuators(QVariant actuators);

public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame od data
     */
	void receive(const Transfer::FrameRX &frame);

public:
    /**
     * @brief Construct a new Actuators object
     *
     * @param engine QML engine of the appliation
     * @param parent QObject parent object
     */
    Actuators(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
