#pragma once

#include <QTcpSocket>
#include <QWidget>
#include <QComboBox>

#include "common/protocol/protocol.h"

namespace common {

class Network : public QWidget {
    Q_OBJECT

	uint8_t buffer_tx[10*1024];
    uint8_t buffer_rx[10*1024];
    uint8_t buffer_decode[10*1024];
	protocol_t protocol = PROTOCOL_INIT;
    QTcpSocket socket;
	std::chrono::_V2::system_clock::time_point start;

	QComboBox *addressComboBox;

	void scanAddresses();
	void changeAddress(const QString &port);

public slots:
	void transmit(const uint8_t id, const void *payload, const uint32_t size);

signals:
	void receive(const uint8_t id, const void *payload, const uint32_t size);
	void error(const protocol_error_t error);

public:
	Network(QWidget *parent = nullptr);
};

}
