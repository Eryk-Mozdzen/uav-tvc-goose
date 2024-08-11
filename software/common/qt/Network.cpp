#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QTimer>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMetaEnum>
#include <QSettings>

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
            (void)error;
            Network *self = reinterpret_cast<Network *>(user);
            self->errorNum++;
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
    QGridLayout *layout = new QGridLayout(group);
    QFormLayout *form = new QFormLayout();

    addressComboBox = new QComboBox();
    addressComboBox->setMinimumWidth(125);
    connect(addressComboBox, &QComboBox::currentTextChanged, this, &Network::changeAddress);

    scanButton = new QPushButton("Scan network");
    connect(scanButton, &QPushButton::pressed, this, &Network::scanAddresses);

    saveButton = new QPushButton("Save as default");
    connect(saveButton, &QPushButton::pressed, [this]() {
        settings.setValue("hostAddress", addressComboBox->currentText());
    });

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Network::updateStats);
    timer->start(1000);

    uiLabels[0] = new QLabel("---");
    uiLabels[1] = new QLabel("--- kB/s");
    uiLabels[2] = new QLabel("--- kB/s");
    uiLabels[3] = new QLabel("---");

    layout->setAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("Status:", uiLabels[0]);
    form->addRow("Upload:", uiLabels[1]);
    form->addRow("Download:", uiLabels[2]);
    form->addRow("Frame errors:", uiLabels[3]);

    layout->addWidget(addressComboBox, 0, 0);
    layout->addWidget(scanButton, 1, 0);
    layout->addWidget(saveButton, 2, 0);
    layout->addLayout(form, 0, 1, 3, 2);

    inside->addWidget(group, 0, 0);

    scanAddresses();
}

void Network::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

void Network::scanAddresses() {
    addressComboBox->setDisabled(true);
    scanButton->setDisabled(true);
    saveButton->setDisabled(true);
    addressComboBox->clear();
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

    scanButton->setText(QString("1/%1").arg(localAddresses.size()));

    for(const QHostAddress &localAddress : localAddresses) {
        QProcess *nmapProcess = new QProcess(this);

        connect(nmapProcess, &QProcess::finished, [this, nmapProcess](int exitCode, QProcess::ExitStatus exitStatus) {
            (void)exitCode;
            (void)exitStatus;

            const QString output = nmapProcess->readAllStandardOutput();

            const QRegularExpression regex(R"(Host:\s+(\d+\.\d+\.\d+\.\d+)\s+)");
            QRegularExpressionMatchIterator i = regex.globalMatch(output);

            while(i.hasNext()) {
                const QRegularExpressionMatch match = i.next();
                const QString address = match.captured(1);

                addressComboBox->addItem(address);
            }

            const QStringList progress = scanButton->text().split("/");
            const int ready = progress[0].toInt();
            const int overall = progress[1].toInt();
            scanButton->setText(QString("%1/%2").arg(ready + 1).arg(overall));

            if(ready==overall) {
                const int index = addressComboBox->findText(settings.value("hostAddress").toString());
                if(index != -1) {
                    addressComboBox->setCurrentIndex(index);
                }

                addressComboBox->setDisabled(false);
                scanButton->setDisabled(false);
                saveButton->setDisabled(false);
                scanButton->setText("Scan network");
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

void Network::changeAddress(const QString &address) {
    if(!address.isEmpty() && (!socket.isOpen() || address!=socket.peerAddress().toString())) {
        protocol.available = false;
        socket.abort();
        socket.connectToHost(address, 23);
    }
}

void Network::updateStats() {
    if(socket.state()==QAbstractSocket::ConnectedState) {
        uiLabels[1]->setText(QString::asprintf("%.3f kB/s", uploadBytes/1024.0));
        uiLabels[2]->setText(QString::asprintf("%.3f kB/s", downloadBytes/1024.0));
        uiLabels[3]->setText(QString::asprintf("%d", errorNum));
    } else {
        uiLabels[1]->setText(QString::asprintf("--- kB/s"));
        uiLabels[2]->setText(QString::asprintf("--- kB/s"));
        uiLabels[3]->setText(QString::asprintf("---"));
    }

    uploadBytes = 0;
    downloadBytes = 0;
    errorNum = 0;
}

}
