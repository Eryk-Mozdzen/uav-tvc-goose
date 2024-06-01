#pragma once

#include <QTcpSocket>

#include "protocol/protocol.h"

namespace shared {

class Telnet : public QObject {
    Q_OBJECT

	uint8_t decoder_buffer[1024];
	protocol_decoder_t decoder;
    QTcpSocket socket;

public slots:
	void transmit(const protocol_message_t &message);

signals:
	void receive(const protocol_message_t &message);

public:
	Telnet(const char *address="10.42.0.194", QObject *parent = nullptr);
};

}
