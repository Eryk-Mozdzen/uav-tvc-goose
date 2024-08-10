#include <chrono>
#include <QSerialPort>
#include <QTimer>

#include "common/qt/Serial.h"
#include "common/protocol/protocol.h"

namespace common {

Serial::Serial(const char *port, QObject *parent) : QObject{parent} {
    protocol.user = this;
    protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(user);
        if(self->serial.isOpen()) {
            self->protocol.available = false;
            self->serial.write(reinterpret_cast<const char *>(data), size);
            self->serial.flush();
        }
    };
    protocol.callback_rx = [](void *user, const uint8_t id, const void *payload, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(user);
        self->receive(id, payload, size);
    };
    protocol.callback_err = [](void *user, const protocol_error_t error) {
        Serial *self = reinterpret_cast<Serial *>(user);
        self->error(error);
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
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        protocol.time = duration;
        protocol_process(&protocol);
    });
    start = std::chrono::high_resolution_clock::now();
    timer->start(1);

    serial.setPortName(port);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::TwoStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
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
