#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>

#include "Form.h"

Form::Form(QString name, QVector<QString> labels, QWidget *parent) : QGroupBox{name, parent}, fields{labels.size()} {
    QFormLayout *form = new QFormLayout(this);

    form->setLabelAlignment(Qt::AlignmentFlag::AlignRight);

    for(int i=0; i<labels.size(); i++) {
        fields[i] = new QLineEdit("???");
        fields[i]->setReadOnly(true);
        fields[i]->setAlignment(Qt::AlignmentFlag::AlignRight);

        form->addRow(labels[i] + ":", fields[i]);
    }
}

void Form::set(int index, QString value) {
    fields[index]->setText(value);
}

void Form::set(int index, const char *format, float value) {
    fields[index]->setText(QString::asprintf(format, value));
}

void Form::reset() {
    for(QLineEdit *field : fields) {
        field->setText("???");
    }
}
