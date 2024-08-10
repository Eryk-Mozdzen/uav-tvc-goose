#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDebug>

#include "common/qt/Serial.h"
#include "common/protocol/protocol.h"

namespace common {

Serial::Serial(QWidget *parent) : QWidget{parent} {
    {
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

        connect(&serial, &QSerialPort::errorOccurred, [&](QSerialPort::SerialPortError error) {
            if(error!=QSerialPort::NoError) {
                protocol.available = true;
                //qDebug() << error;
            }
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
    }

    QGridLayout *inside = new QGridLayout(this);
    QGroupBox *group = new QGroupBox("Serial Interface");
    QGridLayout *grid = new QGridLayout(group);

    portComboBox = new QComboBox();
    connect(portComboBox, &QComboBox::currentTextChanged, this, &Serial::changePort);

    grid->addWidget(portComboBox, 0, 0);

    inside->addWidget(group, 0, 0);

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Serial::updatePortComboBox);
    timer->start(1000);

    updatePortComboBox();
}

void Serial::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

void Serial::updatePortComboBox() {
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

    QList<QString> ports;
    for(const QSerialPortInfo &info : infos) {
        ports.append(info.portName());
    }

    for(const QString &port : ports) {
        if(portComboBox->findText(port) == -1) {
            portComboBox->addItem(port);
        }
    }

    for(int i=0; i<portComboBox->count(); i++) {
        if(!ports.contains(portComboBox->itemText(i))) {
            portComboBox->removeItem(i);
            i--;
        }
    }
}

void Serial::changePort(const QString &port) {
    if(!port.isEmpty() && (!serial.isOpen() || port!=serial.portName())) {
        //qDebug() << port;

        if(serial.isOpen()) {
            serial.close();
        }

        serial.setPortName(port);
        serial.setBaudRate(QSerialPort::Baud115200);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::TwoStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        if(!serial.open(QIODevice::ReadWrite)) {
            qDebug() << "unable to open port" << port;
        }
    }
}

}
