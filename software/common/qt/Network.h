#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QComboBox>
#include <QPushButton>

#include "common/protocol/protocol.h"

namespace common {

class Network : public QObject {
    Q_OBJECT

    uint8_t buffer_tx[10*1024];
    uint8_t buffer_rx[10*1024];
    uint8_t buffer_decode[10*1024];
	protocol_t protocol = PROTOCOL_INIT;

    QTcpSocket *socket;
	int uploadBytes = 0;
	int downloadBytes = 0;
	int errorNum = 0;

public slots:
	void transmit(const uint8_t id, const QByteArray &payload);
	void scanHosts();
	void changeHost(const QString &host);
	void start();

signals:
	void receive(const uint8_t id, const double time, const QByteArray &payload);
	void status(const QString status);
	void stats(const int download, const int upload, const int errors);
	void scanFinished(const QStringList &list);

public:
	Network(QWidget *parent = nullptr);
};

}
