#include "form.h"
#include <QLabel>

namespace widgets {

Form::Form(QVector<QString> labels, bool read_only, QWidget *parent) : QWidget{parent}, fields{labels.size()} {
    form = new QFormLayout(parent);

    form->setLabelAlignment(Qt::AlignmentFlag::AlignRight);

    for(int i=0; i<labels.size(); i++) {
        fields[i] = new QLineEdit("???");
        fields[i]->setReadOnly(read_only);
        fields[i]->setAlignment(Qt::AlignmentFlag::AlignRight);

        if(!read_only) {
            connect(fields[i], &QLineEdit::textEdited, this, std::bind(&Form::textEdited, this, i, std::placeholders::_1));
        }

        form->addRow(new QLabel(labels[i] + ":"), fields[i]);
    }
}

QFormLayout * Form::getLayout() {
    return form;
}

void Form::set(int index, QString value) {
    fields[index]->setText(value);
}

void Form::set(int index, const char *format, float value) {
    fields[index]->setText(QString::asprintf(format, value));
}

void Form::textEdited(int index, QString value) {
    emit change(index, value);
}

QString Form::get(int index) const {
    return fields[index]->text();
}

void Form::reset() {
    for(QLineEdit *field : fields) {
        field->setText("???");
    }
}

}
