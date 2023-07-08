#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QFormLayout>

namespace widgets {

class Form : public QWidget {
    Q_OBJECT

    QVector<QLineEdit *> fields;
    QFormLayout *form;

signals:
    void change(int index, QString value);

private slots:
    void textEdited(int index, QString value);

public:
    Form(QVector<QString> labels, bool read_only, QWidget *parent=nullptr);
    QFormLayout * getLayout();

    void set(int index, QString value);
    void set(int index, const char *format, float value);
    QString get(int index) const;
    void reset();
};

}
