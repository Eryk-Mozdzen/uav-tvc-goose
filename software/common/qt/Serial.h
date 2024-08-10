#pragma once

#include <QSerialPort>

#include "common/protocol/protocol.h"

namespace common {

class Serial : public QObject {
    Q_OBJECT

	uint8_t buffer_tx[10*1024];
    uint8_t buffer_rx[10*1024];
    uint8_t buffer_decode[10*1024];
	protocol_t protocol = PROTOCOL_INIT;
	QSerialPort serial;
	std::chrono::_V2::system_clock::time_point start;

public slots:
	void transmit(const uint8_t id, const void *payload, const uint32_t size);

signals:
	void receive(const uint8_t id, const void *payload, const uint32_t size);
	void error(const protocol_error_t error);

public:
	Serial(const char *port="/dev/ttyACM0", QObject *parent = nullptr);
	~Serial();
};

}
