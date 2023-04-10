#pragma once

#include <QSerialPort>
#include <QTimer>
#include "transfer.h"

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
	void transmit(const Transfer::FrameTX &frame);

signals:
	void receive(const Transfer::FrameRX &frame);

public:
	USB(const QString port, QObject *parent = nullptr);
	~USB();
};
