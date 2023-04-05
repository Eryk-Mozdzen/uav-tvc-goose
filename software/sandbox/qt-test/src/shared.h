#pragma once

#include <QObject>

class Shared : public QObject {
    Q_OBJECT
	Q_PROPERTY(QString data READ getData)

	QString data;

public:
    explicit Shared(QObject *parent = 0);

public slots:
    QString getData();
};
