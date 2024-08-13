#include <cassert>

#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>

#include "Form.h"

Form::Form(const QString name, const QVector<QString> labels, QWidget *parent) : QGroupBox{name, parent}, labels{labels} {
    QFormLayout *form = new QFormLayout(this);

    form->setLabelAlignment(Qt::AlignmentFlag::AlignRight);

    for(const QString &label : labels) {
        QLineEdit *field = new QLineEdit("???", this);
        field->setReadOnly(true);
        field->setAlignment(Qt::AlignmentFlag::AlignRight);

        form->addRow(label + ":", field);
        fields.append(field);
    }
}

void Form::set(const QString label, const QString value) {
    assert(labels.contains(label));

    const int index = labels.indexOf(label);

    fields[index]->setText(value);
}

void Form::set(const QString label, const char *format, const float value) {
    assert(labels.contains(label));

    const int index = labels.indexOf(label);

    fields[index]->setText(QString::asprintf(format, value));
}

void Form::reset() {
    for(QLineEdit *field : fields) {
        field->setText("???");
    }
}
