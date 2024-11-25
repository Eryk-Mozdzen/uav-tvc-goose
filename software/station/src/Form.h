#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>

class Form : public QGroupBox {
    Q_OBJECT

    const QVector<QString> labels;
    QVector<QLineEdit *> fields;

public:
    Form(const QString name, const QVector<QString> labels, QWidget *parent = nullptr);
    void set(const QString label, const QString value);
    void set(const QString label, const char *format, const float value);
    void reset();
};
