#include <iostream>
#include <iomanip>

#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>

#include "window.h"
#include "utils.h"

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

                leastSquares(scale, offset);

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

void Window::leastSquares(Eigen::Matrix3d &scale, Eigen::Vector3d &offset) const {
    const Sample *s = samples;

    Eigen::MatrixXd X(6, 6);
    X << s[0].x, 0,      0,      1, 0, 0,
         0,      s[1].y, 0,      0, 1, 0,
         0,      0,      s[2].z, 0, 0, 1,
         s[3].x, 0,      0,      1, 0, 0,
         0,      s[4].y, 0,      0, 1, 0,
         0,      0,      s[5].z, 0, 0, 1;

    Eigen::VectorXd b(6);
    b << g, g, g, -g, -g, -g;

    const Eigen::VectorXd coeff = (X.transpose() * X).inverse() * X.transpose() * b;

    scale.setZero();
    scale.diagonal() << coeff(0), coeff(1), coeff(2);
    offset << coeff(3), coeff(4), coeff(5);
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
