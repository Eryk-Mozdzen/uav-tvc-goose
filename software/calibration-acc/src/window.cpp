#include <iostream>
#include <iomanip>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>

#include "window.h"

Window::Window(QWidget *parent) : QWidget(parent) {

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
        });

        layout->addWidget(button);
        layout->addWidget(line);
        grid->addWidget(group, i%3, i/3);
    }

    connect(&usb, &USB::receive, this, &Window::callback);
    connect(&telnet, &Telnet::receive, this, &Window::callback);
}

void Window::leastSquares(Eigen::Matrix3d &scale, Eigen::Vector3d &offset) const {
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

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_ACCELERATION) {
        return;
    }

    struct {
        float x, y, z;
    } s;

    frame.getPayload(s);

    current = Eigen::Vector3d(s.x, s.y, s.z);
}
