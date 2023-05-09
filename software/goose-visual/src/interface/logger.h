/**
 * @file logger.h
 * @author Eryk Możdżeń (ermozdzen@@gmail.com)
 * @brief Implementation of logger interface class
 * @date 2023-05-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include "transfer.h"

/**
 * @brief Logger interface class
 *
 */
class Logger : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Add new log message to logger window in QML.
     *
     * @param type type of message, possible: "DEBUG", "INFO", "WARNING", "ERROR"
     * @param text text of the message
     */
    void add(QString type, QString text);

public slots:
    /**
     * @brief New frame received callback
     *
     * @param frame received frame od data
     */
	void receive(const Transfer::FrameRX &frame);

public:
    /**
     * @brief Construct a new Logger object
     *
     * @param engine QML engine of the appliation
     * @param parent QObject parent object
     */
    Logger(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
