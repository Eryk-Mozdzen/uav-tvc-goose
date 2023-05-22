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
#include "transfer.h"

/**
 * @brief Battery interface class
 *
 */
class Battery : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Set voltage level inside battery window in QML.
     *
     * @param voltage power supply voltage in Volts
     */
    void setVoltage(QVariant voltage);

    /**
     * @brief Set current consumption inside battery window in QML.
     *
     * @param current power supply current in Amps
     */
    void setCurrent(QVariant current);

    /**
     * @brief Set battery level inside window in QML.
     *
     * @param level battery level from 0 to 1
     */
    void setLevel(QVariant level);

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
    Battery(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
