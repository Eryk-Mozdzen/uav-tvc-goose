#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QDebug>

#include "common/qt/Network.h"
#include "common/protocol/protocol.h"

namespace common {

Network::Network(QWidget *parent) : QWidget{parent} {
    {
        protocol.user = this;
        protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
            Network *self = reinterpret_cast<Network *>(user);
            if(self->socket.state()==QAbstractSocket::ConnectedState) {
                self->protocol.available = false;
                self->socket.write(reinterpret_cast<const char *>(data), size);
                self->socket.flush();
            }
        };
        protocol.callback_rx = [](void *user, const uint8_t id, const void *payload, const uint32_t size) {
            Network *self = reinterpret_cast<Network *>(user);
            self->receive(id, payload, size);
        };
        protocol.callback_err = [](void *user, const protocol_error_t error) {
            Network *self = reinterpret_cast<Network *>(user);
            self->error(error);
        };
        protocol.fifo_tx.buffer = buffer_tx;
        protocol.fifo_tx.size = sizeof(buffer_tx);
        protocol.fifo_rx.buffer = buffer_rx;
        protocol.fifo_rx.size = sizeof(buffer_rx);
        protocol.decoded = buffer_decode;
        protocol.max = sizeof(buffer_decode);

        connect(&socket, &QTcpSocket::readyRead, [&]() {
            const QByteArray data = socket.readAll();

            for(const uint8_t byte : data) {
                protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
                protocol.fifo_rx.write++;
                protocol.fifo_rx.write %=protocol.fifo_rx.size;
            }
        });

        connect(&socket, &QTcpSocket::bytesWritten, [&](qint64 bytes) {
            (void)bytes;
            protocol.available = true;
        });

        connect(&socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError error) {
            protocol.available = true;
            qDebug() << error;
        });

        connect(&socket, &QTcpSocket::stateChanged, [&](QAbstractSocket::SocketState state) {
            qDebug() << state;
            switch(state) {
                case QAbstractSocket::UnconnectedState:
                case QAbstractSocket::HostLookupState:
                case QAbstractSocket::ConnectingState:
                case QAbstractSocket::BoundState:
                case QAbstractSocket::ClosingState:
                case QAbstractSocket::ListeningState: {
                    protocol.available = false;
                } break;
                case QAbstractSocket::ConnectedState: {
                    protocol.available = true;
                } break;
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
    QGroupBox *group = new QGroupBox("Network Interface");
    QGridLayout *grid = new QGridLayout(group);

    addressComboBox = new QComboBox();
    connect(addressComboBox, &QComboBox::currentTextChanged, this, &Network::changeAddress);

    QPushButton *scanButton = new QPushButton("Scan");
    connect(scanButton, &QPushButton::pressed, this, &Network::scanAddresses);

    grid->addWidget(addressComboBox, 0, 0);
    grid->addWidget(scanButton, 1, 0);

    inside->addWidget(group, 0, 0);

    scanAddresses();
}

void Network::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

void Network::scanAddresses() {
    addressComboBox->clear();

    const QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();

    QList<QHostAddress> interfaces;

    for(const QNetworkInterface &interface : allInterfaces) {
        if(interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            for(const QNetworkAddressEntry &entry : interface.addressEntries()) {
                if(entry.ip().protocol()==QAbstractSocket::IPv4Protocol) {
                    interfaces.append(entry.ip());
                }
            }
        }
    }

    if(interfaces.isEmpty()) {
        return;
    }

    for(const QHostAddress &interface : interfaces) {
        const quint32 baseIp = interface.toIPv4Address() & 0xFFFFFF00;   // assuming /24 net mask

        for(int i=2; i<255; i++) {
            const QHostAddress address(baseIp | i);

            QProcess *pingProcess = new QProcess(this);

            connect(pingProcess, &QProcess::finished, [this, pingProcess](int exitCode, QProcess::ExitStatus exitStatus) {
                if(exitCode==0 && exitStatus==QProcess::NormalExit) {
                    const QString output = pingProcess->readAllStandardOutput();
                    if(output.contains("ttl=")) {
                        const QString addr = pingProcess->property("address").toString();
                        addressComboBox->addItem(addr);
                    }
                }
                pingProcess->deleteLater();
            });

            pingProcess->setProperty("address", address.toString());
            pingProcess->start("ping", QStringList() << "-c 1" << address.toString());
        }
    }
}

void Network::changeAddress(const QString &address) {
    if(!address.isEmpty() && (!socket.isOpen() || address!=socket.peerAddress().toString())) {
        protocol.available = false;
        socket.abort();
        socket.connectToHost(address, 23);
    }
}

}
