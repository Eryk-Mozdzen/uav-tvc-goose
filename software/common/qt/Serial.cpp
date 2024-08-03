#include <QSerialPort>
#include <QTimer>

#include "common/qt/Serial.h"
#include "common/protocol/protocol.h"

namespace common {

Serial::Serial(const char *port, QObject *parent) : QObject{parent} {
    protocol.ctx = this;
    protocol.callback_tx = [](void *ctx, const void *data, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(ctx);
        if(self->serial.isOpen()) {
            self->protocol.available = false;
            self->serial.write(reinterpret_cast<const char *>(data), size);
        }
    };
    protocol.callback_rx = [](void *ctx, const uint8_t id, const void* payload, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(ctx);
        self->receive(id, payload, size);
    };
    protocol.fifo_tx.buffer = buffer_tx;
    protocol.fifo_tx.size = sizeof(buffer_tx);
    protocol.fifo_rx.buffer = buffer_rx;
    protocol.fifo_rx.size = sizeof(buffer_rx);
    protocol.decoded = buffer_decode;
    protocol.max = sizeof(buffer_decode);

    connect(&serial, &QSerialPort::readyRead, [&]() {
        const QByteArray data = serial.readAll();

		for(const uint8_t byte : data) {
            protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
            protocol.fifo_rx.write++;
            protocol.fifo_rx.write %=protocol.fifo_rx.size;
		}
    });

    connect(&serial, &QSerialPort::bytesWritten, [&](qint64 bytes) {
        (void)bytes;
        protocol.available = true;
    });

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, [&]() {
        protocol.time +=10;
        protocol_process(&protocol);
    });
    timer->start(10);

    serial.setPortName(port);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.open(QIODevice::ReadWrite);
}

Serial::~Serial() {
    if(serial.isOpen()) {
        serial.close();
    }
}

void Serial::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

}
