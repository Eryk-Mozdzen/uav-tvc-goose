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
 * @brief Scene interface class
 *
 */
class Scene : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Set voltage level inside battery window in QML.
     *
     * @param voltage power supply voltage in Volts
     */
    void setState(QVariant state);

    /**
     * @brief Set current consumption inside battery window in QML.
     *
     * @param current power supply current in Amps
     */
    void setCommand(QVariant command);

    void setAcceleration(QVariant acceleration);
    void setGyration(QVariant gyration);
    void setMagnitude(QVariant magnitude);


public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame od data
     */
	void receive(const Transfer::FrameRX &frame);

public:
    /**
     * @brief Construct a new Battery object
     *
     * @param engine QML engine of the appliation
     * @param parent QObject parent object
     */
    Scene(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
