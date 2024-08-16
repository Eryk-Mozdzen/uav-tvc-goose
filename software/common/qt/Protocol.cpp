#include <QObject>
#include <QTimer>
#include <QDateTime>

#include "common/protocol/protocol.h"
#include "common/qt/Protocol.h"

namespace common {

const qint64 Protocol::startTime = QDateTime::currentMSecsSinceEpoch();

Protocol::Protocol(QObject *parent) : QObject{parent} {
    protocol.user = this;
    protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
        Protocol *self = reinterpret_cast<Protocol *>(user);
        self->tx(QByteArray(reinterpret_cast<const char *>(data), size));
    };
    protocol.callback_rx = [](void *user, const uint8_t id, const uint32_t time, const void *payload, const uint32_t size) {
        Protocol *self = reinterpret_cast<Protocol *>(user);
        self->receive(id, time/1000., QByteArray(reinterpret_cast<const char *>(payload), size));
    };
    protocol.callback_err = [](void *user, const protocol_error_t err) {
        Protocol *self = reinterpret_cast<Protocol *>(user);
        self->error(err);
    };
    protocol.callback_time = [](void *user) {
        (void)user;
        return (uint32_t)(QDateTime::currentMSecsSinceEpoch() - Protocol::startTime);
    };
    protocol.fifo_tx.buffer = buffer_tx;
    protocol.fifo_tx.size = sizeof(buffer_tx);
    protocol.fifo_rx.buffer = buffer_rx;
    protocol.fifo_rx.size = sizeof(buffer_rx);
    protocol.decoded = buffer_decode;
    protocol.max = sizeof(buffer_decode);
}

void Protocol::transmit(const uint8_t id, const QByteArray &payload) {
    protocol_enqueue(&protocol, id, payload, payload.size());
}

void Protocol::rx(const QByteArray &bytes) {
    for(const uint8_t byte : bytes) {
        protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
        protocol.fifo_rx.write++;
        protocol.fifo_rx.write %=protocol.fifo_rx.size;
    }
}

void Protocol::process() {
    protocol_process(&protocol);
}

void Protocol::start() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Protocol::process);
    timer->start(1);
}

void Protocol::setAvailability(const bool availability) {
    protocol.available = availability;
}

}
