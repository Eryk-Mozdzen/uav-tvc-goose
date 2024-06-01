#include "Lemniscate.h"

Lemniscate::Lemniscate(const double c, const double T) : TrajectoryGenerator{4, 3}, a{c*sqrt2}, w{2*pi/T} {

}

Eigen::VectorX<double> Lemniscate::value(const double &time) const {
    const double s = std::sin(w*time);
    const double c = std::cos(w*time);
    const double s2 = std::sin(2*w*time);
    const double c2 = std::cos(2*w*time);
    const double c4 = std::cos(4*w*time);

    Eigen::Vector<double, 12> trajectory;

    trajectory.segment(0, 4) = Eigen::Vector<double, 4>{
        a*c/(1 + s*s),
        a*s*c/(1 + s*s),
        1,
        fix(std::atan2(-1 + 3*c2, -(5 + c2)*s))
    };

    trajectory.segment(4, 4) = Eigen::Vector<double, 4>{
        -2*a*w*(5 + c2)*s/((-3 + c2)*(-3 + c2)),
        2*a*w*(-1 + 3*c2)/((-3 + c2)*(-3 + c2)),
        0,
        -6*w*c/(-3 + c2)
    };

    trajectory.segment(8, 4) = Eigen::Vector<double, 4>{
        a*w*w*c*(-21 + 44*c2 + c4)/((-3 + c2)*(-3 + c2)*(-3 + c2)),
        4*a*w*w*s2*(7 + 3*c2)/((-3 + c2)*(-3 + c2)*(-3 + c2)),
        0,
        -6*w*w*(5 + c2)*s/((-3 + c2)*(-3 + c2))
    };

    return trajectory;
}

double Lemniscate::fix(double angle) {
    while(angle>(270*deg2rad)) {
        angle -=2*pi;
    }

    while(angle<(-90*deg2rad)) {
        angle +=2*pi;
    }

    return angle;
}
