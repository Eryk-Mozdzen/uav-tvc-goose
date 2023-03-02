#pragma once

#include <QObject>

class MyClass : public QObject {
    Q_OBJECT

public:
    explicit MyClass(QObject *parent = 0);

public slots:
    void onMenuClicked(const QString &str);
};
