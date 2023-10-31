#include "telnet.h"
#include <QDebug>

Telnet::Telnet(const QString address, QObject *parent) : QObject{parent}, socket{new QTcpSocket(this)} {
    connect(socket, &QTcpSocket::errorOccurred, this, &Telnet::error);
    connect(socket, &QTcpSocket::readyRead, this, &Telnet::read);

    socket->connectToHost(address, port);
}

void Telnet::changeAddress(const QString ip) {
    socket->abort();
    socket->connectToHost(ip, port);
}

void Telnet::transmit(const Transfer::FrameTX &frame) {
	socket->write(reinterpret_cast<const char *>(frame.buffer), frame.length);
}

void Telnet::error(QAbstractSocket::SocketError error) {
    qDebug() << error;
}

void Telnet::read() {
    const QByteArray data = socket->readAll();

    for(int i=0; i<data.size(); i++) {
        transfer.consume(data[i]);

        Transfer::FrameRX frame;

        if(transfer.receive(frame)) {
            receive(frame);
        }
    }
}
