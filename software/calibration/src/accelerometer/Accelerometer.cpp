#include <vector>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>

#include "Accelerometer.h"

Accelerometer::Accelerometer(QWidget *parent) : Interface{"accelerometer", parent}, scale{Eigen::Matrix3d::Identity()}, offset{Eigen::Vector3d::Zero()} {
    const std::vector<std::pair<QString, Eigen::Vector3d>> predefined = {
        std::make_pair("+X", Eigen::Vector3d( g,  0,  0)),
        std::make_pair("+Y", Eigen::Vector3d( 0,  g,  0)),
        std::make_pair("+Z", Eigen::Vector3d( 0,  0,  g)),
        std::make_pair("-X", Eigen::Vector3d(-g,  0,  0)),
        std::make_pair("-Y", Eigen::Vector3d( 0, -g,  0)),
        std::make_pair("-Z", Eigen::Vector3d( 0,  0, -g))
    };

    QGridLayout *grid = new QGridLayout(this);

    for(int i=0; i<6; i++) {
        const Eigen::Vector3d orientation = predefined[i].second;

        QGroupBox *group = new QGroupBox(predefined[i].first);
        QHBoxLayout *layout = new QHBoxLayout(group);
        QTextEdit *line = new QTextEdit(this);
        QPushButton *button = new QPushButton("sample");

        line->setReadOnly(true);
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(button, &QPushButton::clicked, [this, orientation, line]() {
            samples.push_back(std::make_pair(orientation, current));

            line->append(QString::asprintf("% 5.4f % 5.4f % 5.4f", current(0), current(1), current(2)));

            leastSquares();
        });

        layout->addWidget(button);
        layout->addWidget(line);
        grid->addWidget(group, i%3, i/3);
    }
}

void Accelerometer::leastSquares() {
    const int N = samples.size();

    Eigen::MatrixXd X(N, 4);
    Eigen::MatrixXd b(N, 3);

    for(int i=0; i<N; i++) {
        const Eigen::Vector3d &orientation = samples[i].first;
        const Eigen::Vector3d &reading = samples[i].second;

        X(i, 3) = 1.;
        X.block(i, 0, 1, 3) = reading.transpose();
        b.block(i, 0, 1, 3) = orientation.transpose();
    }

    const Eigen::MatrixXd coeff = (X.transpose() * X).inverse() * X.transpose() * b;

    scale = coeff.topRows(3).transpose();
    offset = coeff.row(3);
}

Interface * Accelerometer::create() const {
    return new Accelerometer();
}

void Accelerometer::receive(const protocol_readings_t &readings) {
    if(readings.valid.accelerometer) {
        current = Eigen::Vector3d(
            readings.raw.accelerometer[0],
            readings.raw.accelerometer[1],
            readings.raw.accelerometer[2]
        );
    }
}

void Accelerometer::update(protocol_calibration_t &calibration) const {
    calibration.accelerometer[0] = scale(0, 0);
    calibration.accelerometer[1] = scale(0, 1);
    calibration.accelerometer[2] = scale(0, 2);
    calibration.accelerometer[3] = scale(1, 0);
    calibration.accelerometer[4] = scale(1, 1);
    calibration.accelerometer[5] = scale(1, 2);
    calibration.accelerometer[6] = scale(2, 0);
    calibration.accelerometer[7] = scale(2, 1);
    calibration.accelerometer[8] = scale(2, 2);

    calibration.accelerometer[9] = offset(0);
    calibration.accelerometer[10] = offset(1);
    calibration.accelerometer[11] = offset(2);
}
