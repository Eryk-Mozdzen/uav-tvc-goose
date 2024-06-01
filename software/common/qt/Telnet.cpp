#include <QTcpSocket>

#include "Telnet.h"
#include "protocol/protocol.h"

namespace shared {

Telnet::Telnet(const char *address, QObject *parent) : QObject{parent} {
    decoder.buffer = decoder_buffer;
    decoder.size = sizeof(decoder_buffer);
    decoder.counter = 0;

    connect(&socket, &QTcpSocket::readyRead, [&]() {
        const QByteArray data = socket.readAll();

        for(const uint8_t byte : data) {
			protocol_message_t message;

			if(protocol_decode(&decoder, byte, &message)) {
				receive(message);
			}
		}
    });

    socket.connectToHost(address, 23);
}

void Telnet::transmit(const protocol_message_t &message) {
    if(socket.state()==QAbstractSocket::ConnectedState) {
        uint8_t buffer[1024];
        const uint16_t size = protocol_encode(buffer, &message);

        socket.write(reinterpret_cast<char *>(buffer), size);
        socket.flush();
        socket.waitForBytesWritten();
    }
}

}
