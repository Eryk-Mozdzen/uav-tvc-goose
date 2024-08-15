#include <QTimer>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QThread>

#include "common/protocol/protocol.h"
#include "common/qt/AbstractInterface.h"

namespace common {

AbstractInterface::AbstractInterface(const QString name, QWidget *parent) : QGroupBox{name, parent} {
    protocol = new Protocol();
    QThread *thread = new QThread();

    protocol->moveToThread(thread);

    connect(thread, &QThread::started, protocol, &Protocol::start);
    connect(thread, &QThread::finished, protocol, &Protocol::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(protocol, &Protocol::tx, [this](const QByteArray &bytes) {
        transmitBytes(bytes);
        uploadBytes +=bytes.size();
    });
    connect(protocol, &Protocol::receive, this, &AbstractInterface::receive);
    connect(this, &AbstractInterface::transmitAvailable, protocol, &Protocol::setAvailability);

    thread->start();

    QGridLayout *layout = new QGridLayout(this);
    QFormLayout *form = new QFormLayout();

    addressComboBox = new QComboBox();
    addressComboBox->setMinimumWidth(125);
    connect(addressComboBox, &QComboBox::currentTextChanged, this, &AbstractInterface::changeInput);

    scanButton = new QPushButton("Scan inputs");
    connect(scanButton, &QPushButton::pressed, this, &AbstractInterface::scanInput);

    saveButton = new QPushButton("Save as default");
    connect(saveButton, &QPushButton::pressed, [this]() {
        settings.setValue("defaultInput", addressComboBox->currentText());
    });

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &AbstractInterface::updateStats);
    timer->start(1000);

    uiLabels[0] = new QLabel("---");
    uiLabels[1] = new QLabel("--- kB/s");
    uiLabels[2] = new QLabel("--- kB/s");
    uiLabels[3] = new QLabel("---");

    layout->setAlignment(Qt::AlignCenter);
    form->setLabelAlignment(Qt::AlignRight);
    form->addRow("Status:", uiLabels[0]);
    form->addRow("Upload:", uiLabels[1]);
    form->addRow("Download:", uiLabels[2]);
    form->addRow("Frame errors:", uiLabels[3]);

    layout->addWidget(addressComboBox, 0, 0);
    layout->addWidget(scanButton, 1, 0);
    layout->addWidget(saveButton, 2, 0);
    layout->addLayout(form, 0, 1, 3, 2);

    setLayout(layout);
}

void AbstractInterface::transmit(const uint8_t id, const QByteArray &payload) {
    protocol->transmit(id, payload);
}

void AbstractInterface::receiveBytes(const QByteArray &bytes) {
    protocol->rx(bytes);
    downloadBytes +=bytes.size();
}

void AbstractInterface::updateStats() {
    uiLabels[1]->setText(QString::asprintf("%.3f kB/s", uploadBytes/1024.0));
    uiLabels[2]->setText(QString::asprintf("%.3f kB/s", downloadBytes/1024.0));
    uiLabels[3]->setText(QString::asprintf("%d", errorNum));

    uploadBytes = 0;
    downloadBytes = 0;
    errorNum = 0;
}

void AbstractInterface::setStatus(const QString &status) {
    uiLabels[0]->setText(status);
}

QString AbstractInterface::getDefaultInput() const {
    return settings.value("defaultInput").toString();
}

}
