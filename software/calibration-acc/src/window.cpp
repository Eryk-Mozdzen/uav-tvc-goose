#include <iostream>
#include <iomanip>

#include <Eigen/Dense>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>

#include "window.h"
#include "utils.h"

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

Window::Window(QWidget *parent) : QWidget(parent) {
    QGridLayout *grid = new QGridLayout(this);

    const QString axes[6] = {
        "+X",
        "+Y",
        "+Z",
        "-X",
        "-Y",
        "-Z"
    };

    {
        QGroupBox *group = new QGroupBox("realtime", this);
        QGridLayout *layout = new QGridLayout(group);
        layout->setColumnMinimumWidth(0, 100);

        for(int j=0; j<3; j++) {
            current_line[j] = new QLineEdit(this);
            current_line[j]->setReadOnly(true);
            current_line[j]->setAlignment(Qt::AlignHCenter);
            current_line[j]->setFixedWidth(100);

            layout->addWidget(current_line[j], 0, j+1);
        }

        grid->addWidget(group, 0, 0);
    }

    {
        QGroupBox *group = new QGroupBox("samples", this);
        QGridLayout *layout = new QGridLayout(group);

        for(int i=0; i<6; i++) {
            QPushButton *button = new QPushButton("sample " + axes[i], this);
            button->setFixedWidth(100);

            layout->addWidget(button, i, 0);

            QLineEdit *line[3];

            for(int j=0; j<3; j++) {
                line[j] = new QLineEdit(this);
                line[j]->setReadOnly(true);
                line[j]->setAlignment(Qt::AlignHCenter);
                line[j]->setFixedWidth(100);

                layout->addWidget(line[j], i, j+1);
            }

            connect(button, &QPushButton::clicked, [this, line, i]() {
                line[0]->setText(QString::asprintf("%5.4f", this->current.x));
                line[1]->setText(QString::asprintf("%5.4f", this->current.y));
                line[2]->setText(QString::asprintf("%5.4f", this->current.z));

                this->samples[i] = this->current;

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

                for(int j=0; j<6; j++) {
                    Eigen::Vector3d sample;
                    sample << this->samples[j].x, this->samples[j].y, this->samples[j].z;

                    const Eigen::Vector3d corrected = scale*sample + offset;

                    this->corrected_line[j][0]->setText(QString::asprintf("%5.4f", corrected(0)));
                    this->corrected_line[j][1]->setText(QString::asprintf("%5.4f", corrected(1)));
                    this->corrected_line[j][2]->setText(QString::asprintf("%5.4f", corrected(2)));
                }
            });
        }

        grid->addWidget(group, 1, 0);
    }

    {
        QGroupBox *group = new QGroupBox("corrected", this);
        QGridLayout *layout = new QGridLayout(group);

        for(int i=0; i<6; i++) {
            for(int j=0; j<3; j++) {
                corrected_line[i][j] = new QLineEdit(this);
                corrected_line[i][j]->setReadOnly(true);
                corrected_line[i][j]->setAlignment(Qt::AlignHCenter);
                corrected_line[i][j]->setFixedWidth(100);

                layout->addWidget(corrected_line[i][j], i, j);
            }
        }

        grid->addWidget(group, 1, 1);
    }

    connect(&usb, &USB::receive, this, &Window::callback);
    connect(&telnet, &Telnet::receive, this, &Window::callback);
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_ACCELERATION) {
        return;
    }

    frame.getPayload(current);

    current_line[0]->setText(QString::asprintf("%5.4f", current.x));
    current_line[1]->setText(QString::asprintf("%5.4f", current.y));
    current_line[2]->setText(QString::asprintf("%5.4f", current.z));
}
