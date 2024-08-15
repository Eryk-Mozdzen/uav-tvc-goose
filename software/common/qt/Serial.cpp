#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMetaEnum>

#include "common/qt/AbstractInterface.h"
#include "common/qt/Serial.h"

namespace common {

Serial::Serial(QWidget *parent) : AbstractInterface{"Serial interface", parent} {
    connect(&serial, &QSerialPort::readyRead, [&]() {
        receiveBytes(serial.readAll());
    });

    connect(&serial, &QSerialPort::bytesWritten, [&](qint64 bytes) {
        (void)bytes;
        transmitAvailable(true);
    });

    connect(&serial, &QSerialPort::errorOccurred, [&](QSerialPort::SerialPortError error) {
        setStatus(QString(QMetaEnum::fromType<QSerialPort::SerialPortError>().valueToKey(error)));

        if(error==QSerialPort::NoError) {
            transmitAvailable(true);
        }
    });

    scanInput();
}

void Serial::transmitBytes(const QByteArray &bytes) {
    if(serial.isOpen()) {
        transmitAvailable(false);
        serial.write(bytes);
        serial.flush();
    }
}

void Serial::scanInput() {
    scanButton->setText("Scan ports");

    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();

    QList<QString> ports;
    for(const QSerialPortInfo &info : infos) {
        ports.append(info.portName());
    }

    for(const QString &port : ports) {
        if(addressComboBox->findText(port)==-1) {
            addressComboBox->addItem(port);
        }
    }

    for(int i=0; i<addressComboBox->count(); i++) {
        if(!ports.contains(addressComboBox->itemText(i))) {
            addressComboBox->removeItem(i);
            i--;
        }
    }

    const int index = addressComboBox->findText(getDefaultInput());
    if(index!=-1) {
        addressComboBox->setCurrentIndex(index);
    }
}

void Serial::changeInput(const QString &input) {
    if(!input.isEmpty() && (!serial.isOpen() || input!=serial.portName())) {
        if(serial.isOpen()) {
            serial.close();
        }

        serial.setPortName(input);
        serial.setBaudRate(QSerialPort::Baud115200);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::TwoStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        serial.open(QIODevice::ReadWrite);
    }
}

}
