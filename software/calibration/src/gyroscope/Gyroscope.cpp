#include <QGridLayout>
#include <QLineEdit>

#include "Gyroscope.h"

Gyroscope::Gyroscope(QWidget *parent) : Interface{"gyroscope", parent} {
    QGridLayout *grid = new QGridLayout(this);

    line[0] = new QLineEdit();
	line[1] = new QLineEdit();
	line[2] = new QLineEdit();
    line[3] = new QLineEdit();

    line[0]->setReadOnly(true);
	line[0]->setPlaceholderText("mean x");
	line[0]->setAlignment(Qt::AlignHCenter);
    line[1]->setReadOnly(true);
	line[1]->setPlaceholderText("mean y");
	line[1]->setAlignment(Qt::AlignHCenter);
    line[2]->setReadOnly(true);
	line[2]->setPlaceholderText("mean z");
	line[2]->setAlignment(Qt::AlignHCenter);
    line[3]->setReadOnly(true);
	line[3]->setPlaceholderText("number of samples");
	line[3]->setAlignment(Qt::AlignHCenter);

    grid->addWidget(line[0], 0, 0);
    grid->addWidget(line[1], 0, 1);
    grid->addWidget(line[2], 0, 2);
    grid->addWidget(line[3], 1, 1);
}

Interface * Gyroscope::create() const {
    return new Gyroscope();
}

void Gyroscope::receive(const protocol_readings_t &readings) {
    if(readings.valid.gyroscope) {
        const double w1 = static_cast<double>(n)/static_cast<double>(n + 1);
        const double w2 = 1./static_cast<double>(n + 1);

        mean[0] = w1*mean[0] + w2*readings.raw.gyroscope[0];
        mean[1] = w1*mean[1] + w2*readings.raw.gyroscope[1];
        mean[2] = w1*mean[2] + w2*readings.raw.gyroscope[2];
        n++;

        line[0]->setText(QString::asprintf("%+7.4f", mean[0]));
        line[1]->setText(QString::asprintf("%+7.4f", mean[1]));
        line[2]->setText(QString::asprintf("%+7.4f", mean[2]));
        line[3]->setText(QString::asprintf("%d", n));
    }
}

void Gyroscope::update(protocol_calibration_t &calibration) const {
    calibration.gyroscope[0] = -mean[0];
    calibration.gyroscope[1] = -mean[1];
    calibration.gyroscope[2] = -mean[2];
}
