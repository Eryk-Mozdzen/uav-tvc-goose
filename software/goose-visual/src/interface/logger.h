#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include "transfer.h"

class Logger : public QObject {
    Q_OBJECT

signals:
    void add(QString type, QString text);

public slots:
	void receive(const Transfer::FrameRX &frame);

public:
    Logger(const QQmlApplicationEngine &engine, QObject *parent = nullptr);
};
