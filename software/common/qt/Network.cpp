#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QMetaEnum>

#include "common/protocol/protocol.h"
#include "common/qt/Network.h"

namespace common {

Network::Network(QWidget *parent) : QObject{parent} {
    protocol.user = this;
    protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
        Network *self = reinterpret_cast<Network *>(user);

        const QByteArray bytes(reinterpret_cast<const char *>(data), size);

        if(self->socket->state()==QAbstractSocket::ConnectedState) {
            self->protocol.available = false;
            self->socket->write(bytes);
            self->socket->flush();
        }
    };
    protocol.callback_rx = [](void *user, const uint8_t id, const uint32_t time, const void *payload, const uint32_t size) {
        Network *self = reinterpret_cast<Network *>(user);
        self->receive(id, time/1000., QByteArray(reinterpret_cast<const char *>(payload), size));
    };
    protocol.callback_err = [](void *user, const protocol_error_t err) {
        (void)user;
        (void)err;
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

void Network::start() {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, [this]() {
        const QByteArray bytes = socket->readAll();

        for(const uint8_t byte : bytes) {
            protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
            protocol.fifo_rx.write++;
            protocol.fifo_rx.write %=protocol.fifo_rx.size;
        }

        downloadBytes +=bytes.size();
    });

    connect(socket, &QTcpSocket::bytesWritten, [this](qint64 bytes) {
        protocol.available = true;
        uploadBytes +=bytes;
    });

    connect(socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError error) {
        status(QString(QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(error)).replace("Error", ""));
    });

    connect(socket, &QTcpSocket::stateChanged, [&](QAbstractSocket::SocketState state) {
        status(QString(QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(state)).replace("State", ""));

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

void Network::transmit(const uint8_t id, const QByteArray &payload) {
    protocol_enqueue(&protocol, id, payload.data(), payload.size());
}

void Network::scanHosts() {
    const QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();

    QList<QHostAddress> localAddresses;

    for(const QNetworkInterface &interface : allInterfaces) {
        if(interface.flags().testFlag(QNetworkInterface::IsUp) &&
            interface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            for(const QNetworkAddressEntry &entry : interface.addressEntries()) {
                if(entry.ip().protocol()==QAbstractSocket::IPv4Protocol) {
                    localAddresses.append(entry.ip());
                }
            }
        }
    }

    QStringList *list = new QStringList();
    int *finished = new int(0);

    for(const QHostAddress &localAddress : localAddresses) {
        QProcess *nmapProcess = new QProcess(this);

        connect(nmapProcess, &QProcess::finished, [this, nmapProcess, list, finished, localAddresses](int exitCode, QProcess::ExitStatus exitStatus) {
            (void)exitCode;
            (void)exitStatus;

            const QString output = nmapProcess->readAllStandardOutput();

            const QRegularExpression regex(R"(Host:\s+(\d+\.\d+\.\d+\.\d+)\s+)");
            QRegularExpressionMatchIterator i = regex.globalMatch(output);

            while(i.hasNext()) {
                const QRegularExpressionMatch match = i.next();
                const QString address = match.captured(1);

                list->append(address);
            }

            (*finished)++;
            if(*finished==localAddresses.size()) {
                scanFinished(*list);
                delete finished;
                delete list;
            }

            nmapProcess->deleteLater();
        });

        QStringList nmapArgs;
        nmapArgs << "-sn";
        nmapArgs << "-n";
        nmapArgs << "-oG" << "-";
        nmapArgs << "--open";
        nmapArgs << "--noninteractive";
        nmapArgs << (localAddress.toString() + "/24");

        nmapProcess->start("nmap", nmapArgs);
    }
}

void Network::changeHost(const QString &host) {
    if(!host.isEmpty() && (!socket->isOpen() || host!=socket->peerAddress().toString())) {
        socket->abort();
        socket->connectToHost(host, 23);
    }
}

}
