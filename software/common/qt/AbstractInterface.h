#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

#include "common/protocol/protocol.h"

namespace common {

class AbstractInterface : public QWidget {
    Q_OBJECT

	uint8_t buffer_tx[10*1024];
    uint8_t buffer_rx[10*1024];
    uint8_t buffer_decode[10*1024];
	protocol_t protocol = PROTOCOL_INIT;
	std::chrono::_V2::system_clock::time_point start;

	QSettings settings;
	QLabel *uiLabels[4];
	int uploadBytes;
	int downloadBytes;
	int errorNum;

	void updateStats();

protected:
	QComboBox *addressComboBox;
	QPushButton *scanButton;
	QPushButton *saveButton;

	QString getDefaultInput() const;
	void setStatus(const QString &status);
    void receiveBytes(const QByteArray &bytes);
	void transmitAvailable(const bool available);
    virtual void transmitBytes(const QByteArray &bytes) = 0;

protected slots:
    virtual void scanInput() = 0;
	virtual void changeInput(const QString &input) = 0;

public slots:
	void transmit(const uint8_t id, const void *payload, const uint32_t size);

signals:
	void receive(const uint8_t id, const void *payload, const uint32_t size);

public:
	AbstractInterface(const QString name, QWidget *parent = nullptr);
};

}
