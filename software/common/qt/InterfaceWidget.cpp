#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

#include "common/protocol/protocol.h"
#include "common/qt/InterfaceWidget.h"

namespace common {

InterfaceWidget::InterfaceWidget(const QString name, QWidget *parent) : QGroupBox{name, parent} {
    QGridLayout *layout = new QGridLayout(this);
    QFormLayout *form = new QFormLayout();

    listComboBox = new QComboBox();
    listComboBox->setMinimumWidth(125);
    connect(listComboBox, &QComboBox::currentTextChanged, [this](QString value) {
        change(value);
    });

    scanButton = new QPushButton("Scan inputs");
    connect(scanButton, &QPushButton::pressed, [this]() {
        listComboBox->setDisabled(true);
        scanButton->setDisabled(true);
        saveButton->setDisabled(true);
        listComboBox->clear();
        scan();
    });

    saveButton = new QPushButton("Save as default");
    connect(saveButton, &QPushButton::pressed, [this]() {
        settings.setValue("defaultInput", listComboBox->currentText());
    });

    uiLabels[0] = new QLabel("---");
    uiLabels[1] = new QLabel("--- kB/s");
    uiLabels[2] = new QLabel("--- kB/s");
    uiLabels[3] = new QLabel("---");
    uiLabels[0]->setMinimumWidth(150);

    layout->setAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("Status:", uiLabels[0]);
    form->addRow("Upload:", uiLabels[1]);
    form->addRow("Download:", uiLabels[2]);
    form->addRow("Frame errors:", uiLabels[3]);

    layout->addWidget(listComboBox, 0, 0);
    layout->addWidget(scanButton, 1, 0);
    layout->addWidget(saveButton, 2, 0);
    layout->addLayout(form, 0, 1, 3, 2);

    setLayout(layout);
}

void InterfaceWidget::forceScan() {
    listComboBox->setDisabled(true);
    scanButton->setDisabled(true);
    saveButton->setDisabled(true);
    listComboBox->clear();
    scan();
}

void InterfaceWidget::scanFinished(const QStringList &list) {
    listComboBox->addItems(list);

    const int index = listComboBox->findText(settings.value("defaultInput").toString());

    if(index != -1) {
        listComboBox->setCurrentIndex(index);
    }

    listComboBox->setDisabled(false);
    scanButton->setDisabled(false);
    saveButton->setDisabled(false);
}

void InterfaceWidget::status(const QString st) {
    uiLabels[0]->setText(st);
}

void InterfaceWidget::stats(const int download, const int upload, const int errors) {
    uiLabels[1]->setText(QString::asprintf("%.3f kB/s", upload/1024.0));
    uiLabels[2]->setText(QString::asprintf("%.3f kB/s", download/1024.0));
    uiLabels[3]->setText(QString::asprintf("%d", errors));
}

}
