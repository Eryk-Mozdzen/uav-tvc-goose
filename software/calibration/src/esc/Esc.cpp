#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QFont>

#include "Esc.h"

Esc::Esc(Window *window, QWidget *parent) : Interface{"esc", parent}, window{window}, state{false} {
    QGridLayout *layout = new QGridLayout(this);

    QFont font;
    font.setBold(true);
    font.setPointSize(20);
    speed = new QLabel("--- rad/s", this);
    speed->setFont(font);
    speed->setAlignment(Qt::AlignmentFlag::AlignCenter);

    layout->addWidget(speed, 0, 0, 1, 2);

    sliders[0] = new QSlider(Qt::Horizontal, this);
    sliders[0]->setRange(500, 1500);
    sliders[0]->setValue(1000);

    displays[0] = new QLineEdit("1000", this);
    displays[0]->setReadOnly(true);
    displays[0]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    displays[0]->setMaximumWidth(80);

    connect(sliders[0], &QSlider::valueChanged, [this](int value) {
        displays[0]->setText(QString::asprintf("%d", value));
    });

    layout->addWidget(displays[0], 1, 0);
    layout->addWidget(sliders[0], 1, 1);

    sliders[1] = new QSlider(Qt::Horizontal, this);
    sliders[1]->setRange(1500, 2500);
    sliders[1]->setValue(2000);

    displays[1] = new QLineEdit("2000", this);
    displays[1]->setReadOnly(true);
    displays[1]->setAlignment(Qt::AlignmentFlag::AlignCenter);
    displays[1]->setMaximumWidth(80);

    connect(sliders[1], &QSlider::valueChanged, [this](int value) {
        displays[1]->setText(QString::asprintf("%d", value));
    });

    layout->addWidget(displays[1], 2, 0);
    layout->addWidget(sliders[1], 2, 1);

    QPushButton *swipe = new QPushButton("Swipe", this);

    connect(swipe, &QPushButton::clicked, [this]() {
        state = !state;
    });

    layout->addWidget(swipe, 3, 0, 1, 2);
}

Interface * Esc::create() const {
    Esc *instance = new Esc(window);
    instance->startTransmit();
    return instance;
}

void Esc::receive(const msg_frame_sensor_t &sensor) {
    (void)sensor;

    if(sensor.valid.tachometer) {
        speed->setText(QString::asprintf("%.3f rad/s", sensor.tachometer));
    }
}

void Esc::update(msg_frame_calibration_t &calibration) const {
    calibration.esc[0] = sliders[0]->value();
    calibration.esc[1] = sliders[1]->value();
}

void Esc::startTransmit() {
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this]() {
        msg_frame_manual_t frame;
        frame.is_compare = 1;
        frame.servos.compare[0] = 0;
        frame.servos.compare[1] = 0;
        frame.servos.compare[2] = 0;

        if(state) {
            frame.motor.compare = sliders[1]->value();
        } else {
            frame.motor.compare = sliders[0]->value();
        }

        window->transmit(MSG_ID_MANUAL, QByteArray(reinterpret_cast<const char *>(&frame), sizeof(frame)));
    });

    timer->start(50);
}
