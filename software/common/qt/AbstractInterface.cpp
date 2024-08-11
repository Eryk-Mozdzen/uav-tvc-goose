#include <QTimer>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSettings>

#include "common/protocol/protocol.h"
#include "common/qt/AbstractInterface.h"

namespace common {

AbstractInterface::AbstractInterface(const QString name, QWidget *parent) : QGroupBox{name, parent} {
    {
        protocol.user = this;
        protocol.callback_tx = [](void *user, const void *data, const uint32_t size) {
            AbstractInterface *self = reinterpret_cast<AbstractInterface *>(user);
            self->transmitBytes(QByteArray(reinterpret_cast<const char *>(data), size));
            self->uploadBytes +=size;
        };
        protocol.callback_rx = [](void *user, const uint8_t id, const void *payload, const uint32_t size) {
            AbstractInterface *self = reinterpret_cast<AbstractInterface *>(user);
            self->receive(id, payload, size);
        };
        protocol.callback_err = [](void *user, const protocol_error_t error) {
            (void)error;
            AbstractInterface *self = reinterpret_cast<AbstractInterface *>(user);
            self->errorNum++;
        };
        protocol.fifo_tx.buffer = buffer_tx;
        protocol.fifo_tx.size = sizeof(buffer_tx);
        protocol.fifo_rx.buffer = buffer_rx;
        protocol.fifo_rx.size = sizeof(buffer_rx);
        protocol.decoded = buffer_decode;
        protocol.max = sizeof(buffer_decode);

        QTimer *timer = new QTimer();
        connect(timer, &QTimer::timeout, [&]() {
            const auto end = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            protocol.time = duration;
            protocol_process(&protocol);
        });
        start = std::chrono::high_resolution_clock::now();
        timer->start(1);
    }

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

void AbstractInterface::transmit(const uint8_t id, const void *payload, const uint32_t size) {
    protocol_enqueue(&protocol, id, payload, size);
}

void AbstractInterface::transmitAvailable(const bool available) {
    protocol.available = available;
}

void AbstractInterface::receiveBytes(const QByteArray &bytes) {
    for(const uint8_t byte : bytes) {
        protocol.fifo_rx.buffer[protocol.fifo_rx.write] = byte;
        protocol.fifo_rx.write++;
        protocol.fifo_rx.write %=protocol.fifo_rx.size;
    }

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
