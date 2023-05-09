#include "logger.h"
#include <QDebug>

Logger::Logger(const QQmlApplicationEngine &engine, QObject *parent) : QObject{parent} {
    QObject *logger = engine.rootObjects().first()->findChild<QObject *>("logger");

    if(!logger) {
        qDebug() << "cannot find logger object";
        return;
    }

	connect(this, SIGNAL(add(QString, QString)), logger, SLOT(add(QString, QString)));
}

void Logger::receive(const Transfer::FrameRX &frame) {

    if(frame.id>Transfer::ID::LOG_ERROR) {
        return;
    }

    const std::string text(reinterpret_cast<const char *>(frame.payload), frame.length);

    switch(frame.id) {
        case Transfer::ID::LOG_DEBUG:	add("DEBUG",    QString::fromStdString(text)); break;
        case Transfer::ID::LOG_INFO:	add("INFO",     QString::fromStdString(text)); break;
        case Transfer::ID::LOG_WARNING:	add("WARNING",  QString::fromStdString(text)); break;
        case Transfer::ID::LOG_ERROR:	add("ERROR",    QString::fromStdString(text)); break;
        default: break;
    }
}
