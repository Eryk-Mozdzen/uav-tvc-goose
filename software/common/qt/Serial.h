#pragma once

#include <QSerialPort>

#include "common/qt/AbstractInterface.h"

namespace common {

class Serial : public AbstractInterface {
	QSerialPort serial;

	void transmitBytes(const QByteArray &bytes) override;
    void scanInput() override;
	void changeInput(const QString &input) override;

public:
	Serial(QWidget *parent = nullptr);
};

}
