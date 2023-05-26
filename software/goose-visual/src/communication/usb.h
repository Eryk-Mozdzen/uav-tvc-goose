/**
 * @file usb.h
 * @author Eryk Możdżeń (emozdzen@gmail.com)
 * @brief Implemetation of USB serial port reader and writer.
 * @date 2023-04-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <QSerialPort>
#include <QTimer>
#include "transfer.h"

/**
 * @brief Serial port class.
 *
 */
class USB : public QObject {
	Q_OBJECT

private:
	Transfer transfer;

	QSerialPort serial;
	QTimer timer;

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
	void handleTimeout();

public slots:
	/**
	 * @brief Transmit data frame over USB.
	 *
	 * @param frame Frame to transmit.
	 */
	void transmit(const Transfer::FrameTX &frame);

signals:
	/**
	 * @brief Data frame received signal.
	 *
	 * @param frame Received frame.
	 */
	void receive(const Transfer::FrameRX &frame);

public:
	/**
	 * @brief Open serial port.
	 *
	 * @param port Name of port.
	 * @param parent Parent object.
	 */
	USB(const QString port, QObject *parent = nullptr);

	/**
	 * @brief Close serial port.
	 *
	 */
	~USB();
};
