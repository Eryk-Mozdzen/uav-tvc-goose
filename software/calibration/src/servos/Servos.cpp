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

        for(int i=0; i<4; i++) {
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

        for(int i=4; i<8; i++) {
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

            grid->addWidget(displays[i], i-4, 0);
            grid->addWidget(sliders[i], i-4, 1);
        }

        layout->addWidget(group, 0, 1);
    }

    {
        QGroupBox *group = new QGroupBox("upper bound", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=8; i<12; i++) {
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

            grid->addWidget(displays[i], i-8, 0);
            grid->addWidget(sliders[i], i-8, 1);
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

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        protocol_control_t control;
        control.motor = 0;

        switch(position) {
            case Position::MIN: {
                control.servos[0] = sliders[0]->value();
                control.servos[1] = sliders[1]->value();
                control.servos[2] = sliders[2]->value();
                control.servos[3] = sliders[3]->value();
            } break;
            case Position::ZERO: {
                control.servos[0] = sliders[4]->value();
                control.servos[1] = sliders[5]->value();
                control.servos[2] = sliders[6]->value();
                control.servos[3] = sliders[7]->value();
            } break;
            case Position::MAX: {
                control.servos[0] = sliders[8]->value();
                control.servos[1] = sliders[9]->value();
                control.servos[2] = sliders[10]->value();
                control.servos[3] = sliders[11]->value();
            } break;
        }

        const protocol_message_t control_frame = {
            &control,
            sizeof(control),
            PROTOCOL_ID_CONTROL
        };

        this->window->transmit(control_frame);
    });

    timer->start(50);
}

Interface * Servos::create() const {
    return new Servos(window);
}

void Servos::receive(const protocol_readings_t &readings) {
    (void)readings;
}

void Servos::update(protocol_calibration_t &calibration) const {
    calibration.servos[0] = sliders[0]->value();
    calibration.servos[3] = sliders[1]->value();
    calibration.servos[6] = sliders[2]->value();
    calibration.servos[9] = sliders[3]->value();

    calibration.servos[1]  = sliders[4]->value();
    calibration.servos[4]  = sliders[5]->value();
    calibration.servos[7]  = sliders[6]->value();
    calibration.servos[10] = sliders[7]->value();

    calibration.servos[2]  = sliders[8]->value();
    calibration.servos[5]  = sliders[9]->value();
    calibration.servos[8]  = sliders[10]->value();
    calibration.servos[11] = sliders[11]->value();
}
