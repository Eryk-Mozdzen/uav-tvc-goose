#pragma once

#include <QHostAddress>
#include <QTcpSocket>
#include "transfer.h"

class Telnet : public QObject {
    Q_OBJECT

private:
    Transfer transfer;

    QHostAddress address;
    static constexpr int port = 23;
    QTcpSocket socket;

private slots:
    void handleConnected();
    void handleDisconnected();
    void handleReadyRead();

public slots:
	void transmit(const Transfer::FrameTX &frame);

signals:
	void receive(const Transfer::FrameRX &frame);

public:
    Telnet(const QString address="192.168.0.13", QObject *parent = nullptr);

    void changeAddress(const QString ip);
};
