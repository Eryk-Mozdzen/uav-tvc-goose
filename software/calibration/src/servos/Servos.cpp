#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QLineEdit>
#include <QTimer>
#include <QDebug>

#include "Servos.h"

Servos::Servos(Window *window, QWidget *parent) : Interface{"servos", parent}, window{window}, position{Position::ZERO}, direction{false} {
    setFixedWidth(1000);

    QGridLayout *layout = new QGridLayout(this);

    {
        QGroupBox *group = new QGroupBox("lower bound", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=0; i<3; i++) {
            sliders[i] = new QSlider(Qt::Horizontal, group);
            sliders[i]->setRange(0, 1200);
            sliders[i]->setValue(500);

            displays[i] = new QLineEdit("500", group);
            displays[i]->setReadOnly(true);
            displays[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
            displays[i]->setMaximumWidth(80);

            connect(sliders[i], &QSlider::valueChanged, [this, i](int value) {
                displays[i]->setText(QString::asprintf("%d", value));
            });

            grid->addWidget(displays[i], i, 0);
            grid->addWidget(sliders[i], i, 1);
        }

        layout->addWidget(group, 0, 0);
    }

    {
        QGroupBox *group = new QGroupBox("center", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=3; i<6; i++) {
            sliders[i] = new QSlider(Qt::Horizontal, group);
            sliders[i]->setRange(1200, 1800);
            sliders[i]->setValue(1500);

            displays[i] = new QLineEdit("1500", group);
            displays[i]->setReadOnly(true);
            displays[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
            displays[i]->setMaximumWidth(80);

            connect(sliders[i], &QSlider::valueChanged, [this, i](int value) {
                displays[i]->setText(QString::asprintf("%d", value));
            });

            grid->addWidget(displays[i], i-3, 0);
            grid->addWidget(sliders[i], i-3, 1);
        }

        layout->addWidget(group, 0, 1);
    }

    {
        QGroupBox *group = new QGroupBox("upper bound", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=6; i<9; i++) {
            sliders[i] = new QSlider(Qt::Horizontal, group);
            sliders[i]->setRange(1800, 3000);
            sliders[i]->setValue(2500);

            displays[i] = new QLineEdit("2500", group);
            displays[i]->setReadOnly(true);
            displays[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
            displays[i]->setMaximumWidth(80);

            connect(sliders[i], &QSlider::valueChanged, [this, i](int value) {
                displays[i]->setText(QString::asprintf("%d", value));
            });

            grid->addWidget(displays[i], i-6, 0);
            grid->addWidget(sliders[i], i-6, 1);
        }

        layout->addWidget(group, 0, 2);
    }

    {
        QPushButton *swipe = new QPushButton("Swipe", this);

        layout->addWidget(swipe, 1, 0, 2, 0);

        connect(swipe, &QPushButton::clicked, [this]() {
            if(position==Position::MIN || position==Position::MAX) {
                position = Position::ZERO;
            } else {
                position = direction ? Position::MIN : Position::MAX;
                direction = !direction;
            }
        });
    }
}

Interface * Servos::create() const {
    Servos *instance = new Servos(window);
    instance->startTransmit();
    return instance;
}

void Servos::receive(const msg_frame_sensor_t &sensor) {
    (void)sensor;
}

void Servos::update(msg_frame_calibration_t &calibration) const {
    calibration.servos[0] = sliders[0]->value();
    calibration.servos[3] = sliders[1]->value();
    calibration.servos[6] = sliders[2]->value();

    calibration.servos[1]  = sliders[3]->value();
    calibration.servos[4]  = sliders[4]->value();
    calibration.servos[7]  = sliders[5]->value();

    calibration.servos[2]  = sliders[6]->value();
    calibration.servos[5]  = sliders[7]->value();
    calibration.servos[8]  = sliders[8]->value();
}

void Servos::startTransmit() {
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [this]() {
        msg_frame_manual_t frame;
        frame.is_compare = 1;
        frame.motor.compare = 0;

        switch(position) {
            case Position::MIN: {
                frame.servos.compare[0] = sliders[0]->value();
                frame.servos.compare[1] = sliders[1]->value();
                frame.servos.compare[2] = sliders[2]->value();
            } break;
            case Position::ZERO: {
                frame.servos.compare[0] = sliders[3]->value();
                frame.servos.compare[1] = sliders[4]->value();
                frame.servos.compare[2] = sliders[5]->value();
            } break;
            case Position::MAX: {
                frame.servos.compare[0] = sliders[6]->value();
                frame.servos.compare[1] = sliders[7]->value();
                frame.servos.compare[2] = sliders[8]->value();
            } break;
        }

        this->window->transmit(MSG_ID_MANUAL, QByteArray(reinterpret_cast<const char *>(&frame), sizeof(frame)));
    });

    timer->start(50);
}
