#include "Circle.h"

Circle::Circle(const double x, const double y, const double R, const double T) : TrajectoryGenerator{4, 5}, x{x}, y{y}, R{R}, w{2*pi/T} {

}

Eigen::VectorX<double> Circle::value(const double &t) const {
    Eigen::Vector<double, 4*5> trajectory;

    trajectory.segment(0, 4) = Eigen::Vector<double, 4>{
        x + R*cos(w*t),
        y + R*sin(w*t),
        1,
        w*t + pi/2
    };

    trajectory.segment(4, 4) = Eigen::Vector<double, 4>{
        -R*w*sin(w*t),
        R*w*cos(w*t),
        0,
        w
    };

    trajectory.segment(8, 4) = Eigen::Vector<double, 4>{
        -R*w*w*cos(w*t),
        -R*w*w*sin(w*t),
        0,
        0
    };

    trajectory.segment(12, 4) = Eigen::Vector<double, 4>{
        R*w*w*w*sin(w*t),
        -R*w*w*w*cos(w*t),
        0,
        0
    };

    trajectory.segment(16, 4) = Eigen::Vector<double, 4>{
        R*w*w*w*w*cos(w*t),
        R*w*w*w*w*sin(w*t),
        0,
        0
    };

    return trajectory;
}
