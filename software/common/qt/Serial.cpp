#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QMetaEnum>

#include "common/protocol/protocol.h"
#include "common/qt/Serial.h"

namespace common {

Serial::Serial(QWidget *parent) : QObject{parent} {
    protocol.user = this;
    protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(user);

        const QByteArray bytes(reinterpret_cast<const char *>(data), size);

        if(self->serial->isOpen()) {
            self->protocol.available = false;
            self->serial->write(bytes);
            self->serial->flush();
        }
    };
    protocol.callback_rx = [](void *user, const uint8_t id, const uint32_t time, const void *payload, const uint32_t size) {
        Serial *self = reinterpret_cast<Serial *>(user);
        self->receive(id, time/1000., QByteArray(reinterpret_cast<const char *>(payload), size));
    };
    protocol.callback_err = [](void *user, const protocol_error_t err) {
        (void)err;
        Serial *self = reinterpret_cast<Serial *>(user);
        self->errorNum++;
    };
    protocol.callback_time = [](void *user) {
        (void)user;
        return (uint32_t)QDateTime::currentMSecsSinceEpoch();
    };
    protocol.fifo_tx.buffer = buffer_tx;
    protocol.fifo_tx.size = sizeof(buffer_tx);
    protocol.fifo_rx.buffer = buffer_rx;
    protocol.fifo_rx.size = sizeof(buffer_rx);
    protocol.decoded = buffer_decode;
    protocol.max = sizeof(buffer_decode);
}

void Serial::start() {
    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, [this]() {
        const QByteArray bytes = serial->readAll();

        for(const uint8_t byte : bytes) {
            protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
            protocol.fifo_rx.write++;
            protocol.fifo_rx.write %=protocol.fifo_rx.size;
        }

        downloadBytes +=bytes.size();
    });

    connect(serial, &QSerialPort::bytesWritten, [this](qint64 bytes) {
        protocol.available = true;
        uploadBytes +=bytes;
    });

    connect(serial, &QSerialPort::errorOccurred, [this](QSerialPort::SerialPortError error) {
        status(QString(QMetaEnum::fromType<QSerialPort::SerialPortError>().valueToKey(error)));

        if(error==QSerialPort::NoError) {
            protocol.available = true;
        }
    });

    {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            protocol_process(&protocol);
        });
        timer->start(1);
    }

    {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            stats(downloadBytes, uploadBytes, errorNum);
            downloadBytes = 0;
            uploadBytes = 0;
            errorNum = 0;
        });
        timer->start(1000);
    }
}

void Serial::transmit(const uint8_t id, const QByteArray &payload) {
    protocol_enqueue(&protocol, id, payload.data(), payload.size());
}

void Serial::scanPorts() {
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

    QStringList list;

    for(const QSerialPortInfo &info : infos) {
        list.append(info.portName());
    }

    scanFinished(list);
}

void Serial::changePort(const QString &port) {
    if(!port.isEmpty() && (!serial->isOpen() || port!=serial->portName())) {
        if(serial->isOpen()) {
            serial->close();
        }

        serial->setPortName(port);
        serial->setBaudRate(230400);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        serial->open(QIODevice::ReadWrite);
    }
}

}
