#pragma once

#include <QObject>

class MyHandler : public QObject {
    Q_OBJECT

public:
    explicit MyHandler(QObject *parent = 0);

public slots:
    void click(const QString &str);

signals:
	void doSomething();
};
