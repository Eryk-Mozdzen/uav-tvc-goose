#pragma once

#include <QSerialPort>

#include "protocol/protocol.h"

namespace shared {

class Serial : public QObject {
    Q_OBJECT

	uint8_t decoder_buffer[1024];
	protocol_decoder_t decoder;
	QSerialPort serial;

public slots:
	void transmit(const protocol_message_t &message);

signals:
	void receive(const protocol_message_t &message);

public:
	Serial(const char *port="/dev/ttyACM0", QObject *parent = nullptr);
	~Serial();
};

}
