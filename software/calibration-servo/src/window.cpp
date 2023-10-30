#include <string>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QLineEdit>
#include <QTimer>
#include <QDebug>

#include "window.h"

Window::Window(QWidget *parent) : QWidget{parent}, position{Position::ZERO}, direction{false} {

    setFixedWidth(1000);

    QGridLayout *layout = new QGridLayout(this);

    QSlider *sliders[12];
    QLineEdit *displays[12];

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

            connect(sliders[i], &QSlider::valueChanged, [sliders, displays, i](int value) {
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

            connect(sliders[i], &QSlider::valueChanged, [displays, i](int value) {
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

            connect(sliders[i], &QSlider::valueChanged, [displays, i](int value) {
                displays[i]->setText(QString::asprintf("%d", value));
            });

            grid->addWidget(displays[i], i-8, 0);
            grid->addWidget(sliders[i], i-8, 1);
        }

        layout->addWidget(group, 0, 2);
    }

    {
        QGroupBox *group = new QGroupBox("controls", this);
        QHBoxLayout *hbox = new QHBoxLayout(group);

        QPushButton *swipe = new QPushButton("Swipe", group);

        hbox->addWidget(swipe);
        layout->addWidget(group, 1, 0, 2, 0);

        connect(swipe, &QPushButton::clicked, [this, sliders]() {
            if(position==Position::MIN || position==Position::MAX) {
                position = Position::ZERO;
            } else {
                position = direction ? Position::MIN : Position::MAX;
                direction = !direction;
            }
        });
    }

    connect(this, &Window::transmit, &usb, &USB::transmit);
    connect(this, &Window::transmit, &telnet, &Telnet::transmit);
    connect(&usb, &USB::receive, this, &Window::callback);
    connect(&telnet, &Telnet::receive, this, &Window::callback);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, sliders]() {

        comm::Manual data;
        data.type = comm::Manual::Type::CALIBRATION;
        data.throttle = 0;

        switch(position) {
            case Position::MIN: {
                data.angles[0] = sliders[0]->value();
                data.angles[1] = sliders[1]->value();
                data.angles[2] = sliders[2]->value();
                data.angles[3] = sliders[3]->value();
            } break;
            case Position::ZERO: {
                data.angles[0] = sliders[4]->value();
                data.angles[1] = sliders[5]->value();
                data.angles[2] = sliders[6]->value();
                data.angles[3] = sliders[7]->value();
            } break;
            case Position::MAX: {
                data.angles[0] = sliders[8]->value();
                data.angles[1] = sliders[9]->value();
                data.angles[2] = sliders[10]->value();
                data.angles[3] = sliders[11]->value();
            } break;
        }

        //qDebug() << data.angles[0] << data.angles[1] << data.angles[2] << data.angles[3];
        transmit(Transfer::encode(data, Transfer::ID::CONTROL_MANUAL));
    });
    timer->start(50);
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id<=Transfer::ID::LOG_ERROR) {
        const std::string msg(reinterpret_cast<const char *>(frame.payload), frame.length);

		qDebug() << QString::fromStdString(msg);
	}
}
