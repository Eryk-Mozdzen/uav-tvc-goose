#pragma once

#include <QTimer>
#include "transfer.h"

class PrinterWriter : public QObject {
	Q_OBJECT

private:
	QTimer timer;

private slots:
	void sendSomething();

public slots:
	void onReceive(const Transfer::FrameRX &frame);

signals:
	void onTransmit(const Transfer::FrameTX &frame);

public:
	PrinterWriter(QObject *parent = nullptr);
};
