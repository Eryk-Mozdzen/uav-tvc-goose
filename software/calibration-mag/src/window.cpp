#include "window.h"
#include <QHBoxLayout>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <Eigen/Dense>

Window::Window(QWidget *parent) : QWidget(parent) {
    raw = new Viewer(samples, this);
    calibrated = new Viewer(samples, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(raw);
    layout->addWidget(calibrated);

    connect(&usb, &USB::receive, this, &Window::callback);
    connect(&telnet, &Telnet::receive, this, &Window::callback);
}

Params polyToParams3D(const Eigen::VectorXd &vec) {
    Eigen::Matrix4d Amat;
    Amat << vec(0), vec(3) / 2.0, vec(4) / 2.0, vec(6) / 2.0,
            vec(3) / 2.0, vec(1), vec(5) / 2.0, vec(7) / 2.0,
            vec(4) / 2.0, vec(5) / 2.0, vec(2), vec(8) / 2.0,
            vec(6) / 2.0, vec(7) / 2.0, vec(8) / 2.0, vec(9);

    const Eigen::Matrix3d A3 = Amat.block<3, 3>(0, 0);
    const Eigen::Matrix3d A3inv = A3.inverse();
    const Eigen::Vector3d ofs = vec.segment(6, 3) / 2.0;
    const Eigen::Vector3d center = -A3inv * ofs;

    Eigen::Matrix4d Tofs = Eigen::Matrix4d::Identity();
    Tofs.block<1, 3>(3, 0) = center.transpose();
    const Eigen::Matrix4d R = Tofs * Amat * Tofs.transpose();

    const Eigen::Matrix3d R3 = R.block<3, 3>(0, 0);
    const double s1 = -R(3, 3);
    const Eigen::Matrix3d R3S = R3 / s1;
    const Eigen::EigenSolver<Eigen::Matrix3d> es(R3S);
    const Eigen::Vector3d el = es.eigenvalues().real();
    const Eigen::Matrix3d ec = es.eigenvectors().real();

    const Eigen::Vector3d recip = Eigen::Vector3d::Ones().array() / el.array().abs();
    const Eigen::Vector3d axes = recip.array().sqrt();

    const Eigen::Matrix3d inve = ec.transpose();

    Params params;
    params.offset = center;
    params.scale = axes;
    params.rotation = inve;

    return params;
}

void Window::callback(Transfer::FrameRX frame) {
    if(frame.id!=Transfer::ID::SENSOR_MAGNETIC_FIELD) {
        return;
    }

    Sample s;
    if(!frame.getPayload(s)) {
        return;
    }

    samples.push_back(s);

    Eigen::MatrixXd x(samples.size(), 1);
    Eigen::MatrixXd y(samples.size(), 1);
    Eigen::MatrixXd z(samples.size(), 1);

    for(int i=0; i<samples.size(); i++) {
        x(i, 0) = samples[i].x;
        y(i, 0) = samples[i].y;
        z(i, 0) = samples[i].z;
    }

    Eigen::MatrixXd J(samples.size(), 9);
    Eigen::MatrixXd K(samples.size(), 1);
    K.setOnes();

    J <<    x.array().square(), y.array().square(), z.array().square(),
            (x.array() * y.array()), (x.array() * z.array()), (y.array() * z.array()),
            x, y, z;

    const Eigen::MatrixXd JT = J.transpose();
    const Eigen::MatrixXd JTJ = JT * J;
    const Eigen::VectorXd ABC = JTJ.inverse() * JT * K;

    Eigen::VectorXd poly(10);
    poly.head(9) = ABC;
    poly(9) = -1;

    const Params params = polyToParams3D(poly);

    //std::cout << std::setprecision(4) << std::showpos << std::fixed;
    //std::cout << "Center:    " << params.offset.transpose() << "\n";
    //std::cout << "Axes:      " << params.scale.transpose() << "\n";
    //std::cout << "Rotation:\n" << params.rotation << "\n";
    //std::cout << std::endl;

    const Eigen::Vector3d &offset = params.offset;
    const Eigen::Matrix3d &scale = params.getM();

    std::cout << std::setprecision(4) << std::showpos << std::fixed;
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

    calibrated->set(params);

    update();
}
