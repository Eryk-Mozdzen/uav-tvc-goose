#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "Load.h"

Load::Load(QWidget *parent) : Interface{"load cell", parent} {
    QGridLayout *layout = new QGridLayout(this);

    load_line = new QLineEdit("");
    raw_line = new QLineEdit("");
    avg_line = new QLineEdit("");
    load_line->setReadOnly(true);
    raw_line->setReadOnly(true);
    avg_line->setReadOnly(true);
    load_line->setAlignment(Qt::AlignHCenter);
    raw_line->setAlignment(Qt::AlignHCenter);
    avg_line->setAlignment(Qt::AlignHCenter);

    min_line = new QLineEdit("");
    max_line = new QLineEdit("");
    min_line->setReadOnly(true);
    max_line->setReadOnly(true);
    min_line->setAlignment(Qt::AlignHCenter);
    max_line->setAlignment(Qt::AlignHCenter);

    QPushButton *min_button = new QPushButton("update");
    QPushButton *max_button = new QPushButton("update");

    connect(min_button, &QPushButton::clicked, [&]() {
        min = static_cast<double>(sum)/static_cast<double>(num);
        min_line->setText(QString::asprintf("%d", min));
    });

    connect(max_button, &QPushButton::clicked, [&]() {
        max = static_cast<double>(sum)/static_cast<double>(num);
        max_line->setText(QString::asprintf("%d", max));
    });

    layout->addWidget(new QLabel("current (calibrated)"), 0, 0);
    layout->addWidget(new QLabel("current (raw)"), 1, 0);
    layout->addWidget(new QLabel(QString::asprintf("average from last %d samples", num)), 2, 0);
    layout->addWidget(load_line, 0, 1);
    layout->addWidget(raw_line, 1, 1);
    layout->addWidget(avg_line, 2, 1);
    layout->addWidget(new QLabel("kg"), 0, 2);
    layout->addWidget(new QLabel("LSB"), 1, 2);
    layout->addWidget(new QLabel("LSB"), 2, 2);
    layout->addWidget(new QLabel("reference (0 kg)"), 3, 0);
    layout->addWidget(new QLabel("reference (1 kg)"), 4, 0);
    layout->addWidget(min_line, 3, 1);
    layout->addWidget(max_line, 4, 1);
    layout->addWidget(min_button, 3, 2);
    layout->addWidget(max_button, 4, 2);
}

Interface * Load::create() const {
    return new Load();
}

void Load::receive(const msg_frame_sensor_t &sensor) {
    if(sensor.valid.load) {
        sum -=data[counter];
        data[counter] = sensor.load.raw;
        sum +=data[counter];

        counter++;
        counter %=num;

        load_line->setText(QString::asprintf("%.3f", sensor.load.calib));
        raw_line->setText(QString::asprintf("%d", sensor.load.raw));

        const int avg = static_cast<double>(sum)/static_cast<double>(num);

        avg_line->setText(QString::asprintf("%d", avg));
    }
}

void Load::update(msg_frame_calibration_t &calibration) const {
    calibration.load[0] = min;
    calibration.load[1] = max;
}
