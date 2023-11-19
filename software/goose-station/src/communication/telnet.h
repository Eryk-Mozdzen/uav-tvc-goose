#pragma once

#include <QTcpSocket>

#include "transfer.h"

class Telnet : public QObject {
    Q_OBJECT

private:
    static constexpr int port = 23;
    QTcpSocket socket;
    Transfer transfer;

private slots:
    void read();

public slots:
	void transmit(const Transfer::FrameTX &frame);

signals:
	void receive(const Transfer::FrameRX &frame);

public:
    explicit Telnet(const QString address="192.168.79.29", QObject *parent = nullptr);

    void changeAddress(const QString ip);
};
