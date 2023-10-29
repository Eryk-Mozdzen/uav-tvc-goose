#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QLineEdit>

#include "window.h"

Window::Window(QWidget *parent) : QWidget{parent}, swipe_state{false} {

    setFixedWidth(1000);

    QGridLayout *layout = new QGridLayout(this);

    QSlider *sliders[8];
    QLineEdit *displays[8];

    {
        QGroupBox *group = new QGroupBox("low bound", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=4; i<8; i++) {
            sliders[i] = new QSlider(Qt::Horizontal, group);
            sliders[i]->setRange(0, 1500);
            sliders[i]->setValue(500);
            sliders[i]->setInvertedAppearance(true);

            displays[i] = new QLineEdit("1000", group);
            displays[i]->setReadOnly(true);
            displays[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
            displays[i]->setMaximumWidth(100);

            connect(sliders[i], &QSlider::valueChanged, [sliders, displays, i](int value) {
                displays[i]->setText(QString::asprintf("%d",  sliders[i]->maximum() - value));
            });
        }

        grid->addWidget(displays[4], 0, 0);
        grid->addWidget(displays[5], 1, 0);
        grid->addWidget(displays[6], 2, 0);
        grid->addWidget(displays[7], 3, 0);
        grid->addWidget(sliders[4], 0, 1);
        grid->addWidget(sliders[5], 1, 1);
        grid->addWidget(sliders[6], 2, 1);
        grid->addWidget(sliders[7], 3, 1);
        layout->addWidget(group, 0, 0);
    }

    {
        QGroupBox *group = new QGroupBox("high bound", this);
        QGridLayout *grid = new QGridLayout(group);

        for(int i=0; i<4; i++) {
            sliders[i] = new QSlider(Qt::Horizontal, group);
            sliders[i]->setRange(1500, 3000);
            sliders[i]->setValue(2000);

            displays[i] = new QLineEdit("2000", group);
            displays[i]->setReadOnly(true);
            displays[i]->setAlignment(Qt::AlignmentFlag::AlignCenter);
            displays[i]->setMaximumWidth(100);

            connect(sliders[i], &QSlider::valueChanged, [displays, i](int value) {
                displays[i]->setText(QString::asprintf("%d", value));
            });
        }

        grid->addWidget(displays[0], 0, 1);
        grid->addWidget(displays[1], 1, 1);
        grid->addWidget(displays[2], 2, 1);
        grid->addWidget(displays[3], 3, 1);
        grid->addWidget(sliders[0], 0, 0);
        grid->addWidget(sliders[1], 1, 0);
        grid->addWidget(sliders[2], 2, 0);
        grid->addWidget(sliders[3], 3, 0);
        layout->addWidget(group, 0, 1);
    }

    {
        QGroupBox *group = new QGroupBox("controls", this);
        QHBoxLayout *hbox = new QHBoxLayout(group);

        QPushButton *swipe = new QPushButton("Swipe", group);
        QPushButton *zero = new QPushButton("Zero", group);

        hbox->addWidget(swipe);
        hbox->addWidget(zero);
        layout->addWidget(group, 1, 0, 2, 0);

        connect(swipe, &QPushButton::clicked, [this, sliders]() {
            int values[4];

            if(swipe_state) {
                values[0] = sliders[0]->value();
                values[1] = sliders[1]->value();
                values[2] = sliders[2]->value();
                values[3] = sliders[3]->value();
            } else {
                values[0] = sliders[4]->maximum() - sliders[4]->value();
                values[1] = sliders[5]->maximum() - sliders[5]->value();
                values[2] = sliders[6]->maximum() - sliders[6]->value();
                values[3] = sliders[7]->maximum() - sliders[7]->value();
            }

            // send

            swipe_state = !swipe_state;
        });

        connect(zero, &QPushButton::clicked, [sliders]() {
            int values[4];

            values[0] = (sliders[0]->value() + sliders[4]->maximum() - sliders[4]->value())/2;
            values[1] = (sliders[1]->value() + sliders[5]->maximum() - sliders[5]->value())/2;
            values[2] = (sliders[2]->value() + sliders[6]->maximum() - sliders[6]->value())/2;
            values[3] = (sliders[3]->value() + sliders[7]->maximum() - sliders[7]->value())/2;

            // send
        });
    }
}
