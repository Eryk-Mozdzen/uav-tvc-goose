#include "Circle.h"

Circle::Circle(const double x, const double y, const double R, const double T) : TrajectoryGenerator{4, 3}, x{x}, y{y}, R{R}, w{2*pi/T} {

}

Eigen::VectorX<double> Circle::value(const double &time) const {
    const double s = std::sin(w*time);
    const double c = std::cos(w*time);

    Eigen::Vector<double, 12> trajectory;

    trajectory.segment(0, 4) = Eigen::Vector<double, 4>{
        R*c + x,
        R*s + y,
        1,
        w*time + pi/2
    };

    trajectory.segment(4, 4) = Eigen::Vector<double, 4>{
        -R*w*s,
        R*w*c,
        0,
        w
    };

    trajectory.segment(8, 4) = Eigen::Vector<double, 4>{
        -R*w*w*c,
        -R*w*w*s,
        0,
        0
    };

    return trajectory;
}
