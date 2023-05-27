/**
 * @file actuators.h
 * @author Jakub Delicat (delicat.kuba@gmail.com)
 * @brief Implementation of actuators interface class
 * @date 2023-05-27
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
    /**
     * @brief Set throttle inside actuators window in QML.
     *
     * @param throttle throttle fill in percentages
     */
    void setThrottle(QVariant throttle);

    /**
     * @brief Set actuators angles inside actuators window in QML.
     *
     * @param actuators list of actuators' angles in degees
     */
    void setActuators(QVariant actuators);

public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame of the data
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
