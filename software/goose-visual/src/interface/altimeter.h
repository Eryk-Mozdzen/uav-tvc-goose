/**
 * @file altimeter.h
 * @author Eryk Możdżeń (ermozdzen@@gmail.com)
 * @brief Implementation of altimeter interface class
 * @date 2023-05-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include "transfer.h"

/**
 * @brief Altimeter interface class
 *
 */
class Altimeter : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Set pressure inside altimeter window in QML.
     *
     * @param pressure barometeric pressure in hPa
     */
    void setPressure(QVariant pressure);

    /**
     * @brief Set distance from TOF sensor inside altimeter window in QML.
     *
     * @param distance distance in meters
     */
    void setDistance(QVariant distance);

    /**
     * @brief Set altitude inside window in QML.
     *
     * @param altitude height above ground in meters
     */
    void setAltitude(QVariant altitude);

public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame od data
     */
	void receive(const Transfer::FrameRX &frame);

public:
    /**
     * @brief Construct a new Altimeter object
     *
     * @param engine QML engine of the appliation
     * @param parent QObject parent object
     */
    Altimeter(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
