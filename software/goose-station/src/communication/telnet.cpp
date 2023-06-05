#include "telnet.h"
#include <QDebug>

Telnet::Telnet(const QString address, QObject *parent) : QObject{parent}, address{address} {

    connect(&socket, &QTcpSocket::connected, this, &Telnet::handleConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &Telnet::handleDisconnected);
    connect(&socket, &QTcpSocket::readyRead, this, &Telnet::handleReadyRead);

    socket.connectToHost(address, port);
}

void Telnet::changeAddress(const QString ip) {
    address = QHostAddress(ip);

    socket.abort();
    socket.connectToHost(address, port);
}

void Telnet::transmit(const Transfer::FrameTX &frame) {
    if(!socket.isOpen()) {
        return;
    }

	socket.write(reinterpret_cast<const char *>(frame.buffer), frame.length);
}

void Telnet::handleConnected() {
    qDebug() << "connected to" <<  address.toString();
}

void Telnet::handleDisconnected() {
    qDebug() << "disconnected from" <<  address.toString();

    socket.connectToHost(address, port);
}

void Telnet::handleReadyRead() {
    const QByteArray data = socket.readAll();

    for(int i=0; i<data.size(); i++) {
        transfer.consume(data[i]);

        Transfer::FrameRX frame;

        if(transfer.receive(frame)) {
            receive(frame);
        }
    }
}
