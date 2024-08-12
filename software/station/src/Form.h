#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>

class Form : public QGroupBox {
    Q_OBJECT

    QVector<QLineEdit *> fields;

public:
    Form(QString name, QVector<QString> labels, QWidget *parent = nullptr);
    void set(int index, QString value);
    void set(int index, const char *format, float value);
    void reset();
};
