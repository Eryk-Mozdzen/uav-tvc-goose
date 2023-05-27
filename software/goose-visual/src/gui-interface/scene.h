/**
 * @file scene.h
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
 * @brief Scene interface class
 *
 */
class Scene : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Set state attitude inside scene window in QML.
     *
     * @param state state attitude roll, pith, yaw vector in degrees
     */
    void setState(QVariant state);

    /**
     * @brief Set command attitude inside scene window in QML.
     *
     * @param command command attitude roll, pith, yaw vector in degrees
     */
    void setCommand(QVariant command);

    /**
     * @brief Set acceleration inside scene window in QML.
     *
     * @param acceleration acceleration vector in m/s^2
     */
    void setAcceleration(QVariant acceleration);

    /**
     * @brief Set gyration inside scene window in QML.
     *
     * @param gyration gyration vector in rad/s
     */
    void setGyration(QVariant gyration);

    /**
     * @brief Set magnitude inside scene window in QML.
     *
     * @param magnitude magnitude vector in mGa
     */
    void setMagnitude(QVariant magnitude);


public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame of the data
     */
	void receive(const Transfer::FrameRX &frame);

public:
    /**
     * @brief Construct a new Scene object
     *
     * @param engine QML engine of the appliation
     * @param parent QObject parent object
     */
    Scene(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
