#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QProcess>
#include <QMetaEnum>

#include "common/qt/AbstractInterface.h"
#include "common/qt/Network.h"

namespace common {

Network::Network(QWidget *parent) : AbstractInterface{"Network interface", parent} {
    connect(&socket, &QTcpSocket::readyRead, [&]() {
        receiveBytes(socket.readAll());
    });

    connect(&socket, &QTcpSocket::bytesWritten, [&](qint64 bytes) {
        (void)bytes;
        transmitAvailable(true);
    });

    connect(&socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError error) {
        setStatus(QString(QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(error)).replace("Error", ""));
    });

    connect(&socket, &QTcpSocket::stateChanged, [&](QAbstractSocket::SocketState state) {
        setStatus(QString(QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(state)).replace("State", ""));

        switch(state) {
            case QAbstractSocket::UnconnectedState:
            case QAbstractSocket::HostLookupState:
            case QAbstractSocket::ConnectingState:
            case QAbstractSocket::BoundState:
            case QAbstractSocket::ClosingState:
            case QAbstractSocket::ListeningState: {
                transmitAvailable(false);
            } break;
            case QAbstractSocket::ConnectedState: {
                transmitAvailable(true);
            } break;
        }
    });

    scanInput();
}

void Network::transmitBytes(const QByteArray &bytes) {
    if(socket.state()==QAbstractSocket::ConnectedState) {
        transmitAvailable(false);
        socket.write(bytes);
        socket.flush();
    }
}

void Network::scanInput() {
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
                const int index = addressComboBox->findText(getDefaultInput());
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

void Network::changeInput(const QString &input) {
    if(!input.isEmpty() && (!socket.isOpen() || input!=socket.peerAddress().toString())) {
        socket.abort();
        socket.connectToHost(input, 23);
    }
}

}
