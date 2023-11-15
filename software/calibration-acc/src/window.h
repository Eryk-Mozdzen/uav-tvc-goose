#pragma once

#include <vector>

#include <Eigen/Dense>
#include <QWidget>

#include "usb.h"
#include "telnet.h"
#include "transfer.h"

class Window : public QWidget {
    Q_OBJECT

    static constexpr double g = 9.80665;

    Eigen::Vector3d current;
    std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> samples;

    USB usb;
    Telnet telnet;

    void leastSquares(Eigen::Matrix3d &scale, Eigen::Vector3d &offset) const;

private slots:
    void callback(Transfer::FrameRX frame);

public:
    Window(QWidget *parent = nullptr);
};
