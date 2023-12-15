#include <QDebug>

#include "telnet.h"

Telnet::Telnet(const QString address, QObject *parent) : QObject{parent} {
    connect(&socket, &QTcpSocket::readyRead, this, &Telnet::read);

    socket.connectToHost(address, port);
}

void Telnet::changeAddress(const QString ip) {
    socket.abort();
    socket.connectToHost(ip, port);
}

void Telnet::transmit(const Transfer::FrameTX &frame) {
    if(socket.state()!=QAbstractSocket::ConnectedState) {
        return;
    }

	socket.write(reinterpret_cast<const char *>(frame.buffer), frame.length);
    socket.flush();
    socket.waitForBytesWritten(1000);
}

void Telnet::read() {
    const QByteArray data = socket.readAll();

    for(int i=0; i<data.size(); i++) {
        transfer.consume(data[i]);

        Transfer::FrameRX frame;

        if(transfer.receive(frame)) {
            receive(frame);
        }
    }
}
