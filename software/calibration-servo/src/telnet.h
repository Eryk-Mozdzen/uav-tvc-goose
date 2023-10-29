#pragma once

#include <QTcpSocket>
#include "transfer.h"

class Telnet : public QObject {
    Q_OBJECT

private:
    static constexpr int port = 23;
    QTcpSocket *socket = nullptr;
    Transfer transfer;

private slots:
    void error(QAbstractSocket::SocketError error);
    void read();

public slots:
	void transmit(const Transfer::FrameTX &frame);

signals:
	void receive(const Transfer::FrameRX &frame);

public:
    explicit Telnet(const QString address="10.42.0.220", QObject *parent = nullptr);

    void changeAddress(const QString ip);
};
