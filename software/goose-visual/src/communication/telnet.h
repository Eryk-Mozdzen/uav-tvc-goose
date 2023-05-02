#pragma once

#include <QHostAddress>
#include <QTcpSocket>
#include "transfer.h"

class Telnet : public QObject {
    Q_OBJECT

private:
    Transfer transfer;

    const QHostAddress address;
    const int port;
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
    Telnet(const QString address, const int port, QObject *parent = nullptr);
};
