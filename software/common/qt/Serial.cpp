#include <QSerialPort>

#include "Serial.h"
#include "protocol/protocol.h"

namespace shared {

Serial::Serial(const char *port, QObject *parent) : QObject{parent} {
    decoder.buffer = decoder_buffer;
    decoder.size = sizeof(decoder_buffer);
    decoder.counter = 0;

    serial.setPortName(port);
    serial.setBaudRate(QSerialPort::Baud115200);

    connect(&serial, &QSerialPort::readyRead, [&]() {
        const QByteArray data = serial.readAll();

		for(const uint8_t byte : data) {
			protocol_message_t message;

			if(protocol_decode(&decoder, byte, &message)) {
				receive(message);
			}
		}
    });

    serial.open(QIODevice::ReadWrite);
}

Serial::~Serial() {
    if(serial.isOpen()) {
        serial.close();
    }
}

void Serial::transmit(const protocol_message_t &message) {
    if(serial.isOpen()) {
        uint8_t buffer[1024];
        const uint16_t size = protocol_encode(buffer, &message);

        serial.write(reinterpret_cast<char *>(buffer), size);
        serial.waitForBytesWritten();
	}
}

}
