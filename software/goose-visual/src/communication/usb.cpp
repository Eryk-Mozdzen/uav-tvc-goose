#include "usb.h"
#include <QDebug>

USB::USB(const QString port, QObject *parent) : QObject{parent} {

	serial.setPortName(port);
	serial.setBaudRate(115200);

	connect(&serial, &QSerialPort::readyRead, this, &USB::handleReadyRead);
	connect(&serial, &QSerialPort::errorOccurred, this, &USB::handleError);
	connect(&timer, &QTimer::timeout, this, &USB::handleTimeout);

	if(!serial.open(QIODevice::OpenModeFlag::ReadWrite)) {
		timer.start(1000);
	}
}

USB::~USB() {
	timer.stop();
	serial.close();
}

void USB::transmit(const Transfer::FrameTX &frame) {
	if(!serial.isOpen()) {
		return;
	}

	serial.write(reinterpret_cast<const char *>(frame.buffer), frame.length);
}

void USB::handleReadyRead() {
	const QByteArray data = serial.readAll();

	for(int i=0; i<data.size(); i++) {
		transfer.consume(data[i]);

		Transfer::FrameRX frame;

		if(transfer.receive(frame)) {
			receive(frame);
		}
	}
}

void USB::handleError(QSerialPort::SerialPortError error) {
	if(error==QSerialPort::SerialPortError::OpenError || error==QSerialPort::SerialPortError::ResourceError) {
		serial.close();
	}

	if(error!=QSerialPort::SerialPortError::NoError) {
		timer.start(1000);
	}
}

void USB::handleTimeout() {
	if(serial.open(QIODevice::OpenModeFlag::ReadWrite)) {
		timer.stop();
		qDebug() << "connected to" << serial.portName();
		return;
	}

	qDebug() << "waiting for" << serial.portName() << "...";
}
