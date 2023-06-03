#include "form.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

namespace widgets {

Form::Form(QString name, QVector<QString> labels, bool read_only, QWidget *parent) : QWidget{parent}, fields{labels.size()} {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QGroupBox *group = new QGroupBox(name);
    QFormLayout *form = new QFormLayout(group);

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

    layout->addWidget(group);
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

}
