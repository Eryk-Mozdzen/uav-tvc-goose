#include <iostream>
#include <iomanip>

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <Eigen/Dense>

#include "window.h"
#include "utils.h"

Window::Window(QWidget *parent) : QWidget(parent) {
    QGridLayout *grid = new QGridLayout(this);

    for(int j=0; j<3; j++) {
        current_line[j] = new QLineEdit(this);
        current_line[j]->setReadOnly(true);
        current_line[j]->setAlignment(Qt::AlignHCenter);

        grid->addWidget(current_line[j], 0, j+1);
    }

    for(int i=0; i<6; i++) {
        QPushButton *button = new QPushButton("sample", this);

        grid->addWidget(button, i+1, 0);

        QLineEdit *line[3];

        for(int j=0; j<3; j++) {
            line[j] = new QLineEdit(this);
            line[j]->setReadOnly(true);
            line[j]->setAlignment(Qt::AlignHCenter);

            grid->addWidget(line[j], i+1, j+1);
        }

        connect(button, &QPushButton::clicked, [this, line, i]() {
            line[0]->setText(QString::asprintf("%5.4f", this->current.x));
            line[1]->setText(QString::asprintf("%5.4f", this->current.y));
            line[2]->setText(QString::asprintf("%5.4f", this->current.z));

            this->samples[i] = this->current;
        });
    }

    connect(&usb, &USB::receive, this, &Window::callback);
    connect(&telnet, &Telnet::receive, this, &Window::callback);
}

void leastSquares(const Sample *s, Eigen::Matrix3d &scale, Eigen::Vector3d &offset) {
    constexpr double g = 9.80665;

    Eigen::MatrixXd X(6, 4);
    X << s[0].x, s[0].y, s[0].z, 1,
         s[1].x, s[1].y, s[1].z, 1,
         s[2].x, s[2].y, s[2].z, 1,
         s[3].x, s[3].y, s[3].z, 1,
         s[4].x, s[4].y, s[4].z, 1,
         s[5].x, s[5].y, s[5].z, 1;

    Eigen::MatrixXd b(6, 3);
    b <<  g,  0,  0,
          0,  g,  0,
          0,  0,  g,
         -g,  0,  0,
          0, -g,  0,
          0,  0, -g;

    const Eigen::MatrixXd coeff = (X.transpose() * X).inverse() * X.transpose() * b;

    scale = coeff.topRows(3).transpose();
    offset = coeff.row(3);
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_ACCELERATION) {
        return;
    }

    frame.getPayload(current);

    current_line[0]->setText(QString::asprintf("%5.4f", current.x));
    current_line[1]->setText(QString::asprintf("%5.4f", current.y));
    current_line[2]->setText(QString::asprintf("%5.4f", current.z));

    Eigen::Matrix3d scale;
    Eigen::Vector3d offset;

    leastSquares(samples, scale, offset);

    std::cout << std::setprecision(4) << std::showpos << std::fixed << std::setw(10);
    std::cout << "constexpr Vector offset = {\n";
    std::cout << "    " << offset(0) << "f,\n";
    std::cout << "    " << offset(1) << "f,\n";
    std::cout << "    " << offset(2) << "f\n";
    std::cout << "};\n";
    std::cout << "constexpr Matrix<3, 3> scale = {\n";
    std::cout << "    " << scale(0, 0) << "f, " << scale(0, 1) << "f, " << scale(0, 2) << "f,\n";
    std::cout << "    " << scale(1, 0) << "f, " << scale(1, 1) << "f, " << scale(1, 2) << "f,\n";
    std::cout << "    " << scale(2, 0) << "f, " << scale(2, 1) << "f, " << scale(2, 2) << "f\n";
    std::cout << "};\n";
    std::cout << std::endl;
}
