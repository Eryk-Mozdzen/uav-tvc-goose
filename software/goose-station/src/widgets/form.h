#pragma once

#include <QWidget>
#include <QLineEdit>

namespace widgets {

class Form : public QWidget {
    Q_OBJECT

    QVector<QLineEdit *> fields;

signals:
    void change(int index, QString value);

private slots:
    void textEdited(int index, QString value);

public:
    Form(QString name, QVector<QString> labels, bool read_only=true, QWidget *parent=nullptr);

    void set(int index, QString value);
    void set(int index, const char *format, float value);
};

}
