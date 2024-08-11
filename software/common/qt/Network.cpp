#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QMetaEnum>

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

            downloadBytes +=data.size();
        });

        connect(&socket, &QTcpSocket::bytesWritten, [&](qint64 bytes) {
            protocol.available = true;

            uploadBytes +=bytes;
        });

        connect(&socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError error) {
            uiLabels[0]->setText(QString(QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(error)).replace("Error",""));

            protocol.available = true;
        });

        connect(&socket, &QTcpSocket::stateChanged, [&](QAbstractSocket::SocketState state) {
            uiLabels[0]->setText(QString(QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(state)).replace("State",""));

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
    QVBoxLayout *layout = new QVBoxLayout(group);
    QFormLayout *form = new QFormLayout();

    addressComboBox = new QComboBox();
    connect(addressComboBox, &QComboBox::currentTextChanged, this, &Network::changeAddress);

    scanButton = new QPushButton("Scan");
    connect(scanButton, &QPushButton::pressed, this, &Network::scanAddresses);

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, [&]() {
        if(socket.state()==QAbstractSocket::ConnectedState) {
            uiLabels[1]->setText(QString::asprintf("%.3f kB/s", uploadBytes/1024.0));
            uiLabels[2]->setText(QString::asprintf("%.3f kB/s", downloadBytes/1024.0));
        } else {
            uiLabels[1]->setText(QString::asprintf("--- kB/s"));
            uiLabels[2]->setText(QString::asprintf("--- kB/s"));
        }

        uploadBytes = 0;
        downloadBytes = 0;
    });
    start = std::chrono::high_resolution_clock::now();
    timer->start(1000);

    uiLabels[0] = new QLabel("---");
    uiLabels[1] = new QLabel("--- kB/s");
    uiLabels[2] = new QLabel("--- kB/s");

    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("Status:", uiLabels[0]);
    form->addRow("Upload:", uiLabels[1]);
    form->addRow("Download:", uiLabels[2]);

    layout->addLayout(form);
    layout->addWidget(addressComboBox);
    layout->addWidget(scanButton);

    inside->addWidget(group, 0, 0);

    scanAddresses();
}

void Network::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

void Network::scanAddresses() {
    addressComboBox->clear();
    scanButton->setDisabled(true);
    scanButton->setText("0/0");

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

    const int overall = localAddresses.size()*253;
    scanButton->setText(QString::asprintf("0/%d", overall));

    for(const QHostAddress &localAddress : localAddresses) {
        const quint32 netmask = 0xFFFFFF00;   // assuming 255.255.255.0 netmask
        const quint32 subnet = localAddress.toIPv4Address() & netmask;

        for(int i=2; i<255; i++) {
            const QHostAddress address(subnet | i);

            QProcess *pingProcess = new QProcess(this);

            connect(pingProcess, &QProcess::finished, [this, pingProcess, address](int exitCode, QProcess::ExitStatus exitStatus) {
                if(exitCode==0 && exitStatus==QProcess::NormalExit) {
                    const QString output = pingProcess->readAllStandardOutput();
                    if(output.contains("ttl=")) {
                        addressComboBox->addItem(address.toString());
                    }
                }
                pingProcess->deleteLater();

                const QStringList progress = scanButton->text().split("/");
                const int ready = progress[0].toInt() + 1;
                const int overall = progress[1].toInt();
                scanButton->setText(QString::asprintf("%d/%d", ready, overall));

                if(ready==overall) {
                    scanButton->setDisabled(false);
                    scanButton->setText("Scan");
                }
            });

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
