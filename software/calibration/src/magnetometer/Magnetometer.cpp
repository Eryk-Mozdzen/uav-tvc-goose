#include <iostream>
#include <iomanip>

#include <QHBoxLayout>
#include <Eigen/Dense>

#include "Magnetometer.h"

Magnetometer::Magnetometer(QWidget *parent) : Interface("magnetometer", parent), raw{samples, this}, calibrated{samples, this} {
    scale = Eigen::Matrix3d::Identity();
    offset = Eigen::Vector3d::Zero();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(&raw);
    layout->addWidget(&calibrated);
}

Interface * Magnetometer::create() const {
    return new Magnetometer();
}

Eigen::VectorXd bestFitEllipsoid(const std::vector<Sample> &samples) {
    Eigen::MatrixXd x(samples.size(), 1);
    Eigen::MatrixXd y(samples.size(), 1);
    Eigen::MatrixXd z(samples.size(), 1);

    for(size_t i=0; i<samples.size(); i++) {
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

    return poly;
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

void Magnetometer::receive(const protocol_readings_t &readings) {
    if(!readings.valid.magnetometer) {
        return;
    }

    const Sample s = {
        readings.raw.magnetometer[0],
        readings.raw.magnetometer[1],
        readings.raw.magnetometer[2]
    };

    samples.push_back(s);

    const Eigen::VectorXd poly = bestFitEllipsoid(samples);
    const Params params = polyToParams3D(poly);

    offset = -params.getM()*params.offset;
    scale = params.getM();

    calibrated.set(params);
}

void Magnetometer::update(protocol_calibration_t &calibration) const {
    calibration.magnetometer[0] = scale(0, 0);
    calibration.magnetometer[1] = scale(0, 1);
    calibration.magnetometer[2] = scale(0, 2);
    calibration.magnetometer[3] = scale(1, 0);
    calibration.magnetometer[4] = scale(1, 1);
    calibration.magnetometer[5] = scale(1, 2);
    calibration.magnetometer[6] = scale(2, 0);
    calibration.magnetometer[7] = scale(2, 1);
    calibration.magnetometer[8] = scale(2, 2);

    calibration.magnetometer[9] = offset(0);
    calibration.magnetometer[10] = offset(1);
    calibration.magnetometer[11] = offset(2);
}
