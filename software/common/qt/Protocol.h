#pragma once

#include <QObject>

#include "common/protocol/protocol.h"

namespace common {

class Protocol : public QObject {
	Q_OBJECT

	static const qint64 startTime;
	uint8_t buffer_tx[10*1024];
    uint8_t buffer_rx[10*1024];
    uint8_t buffer_decode[10*1024];
	protocol_t protocol = PROTOCOL_INIT;

	void process();

signals:
	void receive(const uint8_t id, const double time, const QByteArray &payload);
	void error(const protocol_error_t &err);
	void tx(const QByteArray &bytes);

public slots:
	void transmit(const uint8_t id, const QByteArray &payload);
	void rx(const QByteArray &bytes);

public:
	Protocol(QObject *parent = nullptr);
	void start();
	void setAvailability(const bool availability);
};

}
