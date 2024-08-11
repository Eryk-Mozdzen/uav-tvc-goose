#pragma once

#include <QTcpSocket>

#include "common/qt/AbstractInterface.h"

namespace common {

class Network : public AbstractInterface {
    QTcpSocket socket;

    void transmitBytes(const QByteArray &bytes) override;
    void scanInput() override;
	void changeInput(const QString &input) override;

public:
	Network(QWidget *parent = nullptr);
};

}
