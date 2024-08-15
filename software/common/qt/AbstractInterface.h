#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

#include "common/qt/Protocol.h"

namespace common {

class AbstractInterface : public QGroupBox {
    Q_OBJECT

	Protocol *protocol;

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

signals:
	void transmitBytes(const QByteArray &bytes);
	void transmitAvailable(const bool available);

protected slots:
    virtual void scanInput() = 0;
	virtual void changeInput(const QString &input) = 0;

public slots:
	void transmit(const uint8_t id, const QByteArray &payload);

signals:
	void receive(const uint8_t id, const QByteArray &payload);

public:
	AbstractInterface(const QString name, QWidget *parent = nullptr);
};

}
