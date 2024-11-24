#include "Lemniscate.h"

Lemniscate::Lemniscate(const double c, const double T) : TrajectoryGenerator{4, 100}, a{c*sqrt2}, w{2*pi/T} {

}

Eigen::VectorX<double> Lemniscate::value(const double &time) const {
    /*Eigen::Vector<double, 20> trajectory;

    trajectory.segment(0, 4) = Eigen::Vector<double, 4>{
        a*cos(t*w)/(pow(sin(t*w), 2) + 1),
        a*sin(t*w)*cos(t*w)/(pow(sin(t*w), 2) + 1),
        1,
        fix(atan2(-a*w*pow(sin(t*w), 2)/(pow(sin(t*w), 2) + 1) + a*w*pow(cos(t*w), 2)/(pow(sin(t*w), 2) + 1) - 2*a*w*pow(sin(t*w), 2)*pow(cos(t*w), 2)/pow(pow(sin(t*w), 2) + 1, 2), -a*w*sin(t*w)/(pow(sin(t*w), 2) + 1) - 2*a*w*sin(t*w)*pow(cos(t*w), 2)/pow(pow(sin(t*w), 2) + 1, 2)))
    };

    trajectory.segment(4, 4) = Eigen::Vector<double, 4>{
        a*w*(pow(sin(t*w), 2) - 3)*sin(t*w)/pow(pow(sin(t*w), 2) + 1, 2),
        a*w*(1 - 3*pow(sin(t*w), 2))/pow(pow(sin(t*w), 2) + 1, 2),
        0,
        3*w*cos(t*w)/(pow(sin(t*w), 2) + 1)
    };

    trajectory.segment(8, 4) = Eigen::Vector<double, 4>{
        a*pow(w, 2)*(-pow(sin(t*w), 4) + 12*pow(sin(t*w), 2) - 3)*cos(t*w)/pow(pow(sin(t*w), 2) + 1, 3),
        2*a*pow(w, 2)*(14*sin(2*t*w) + 3*sin(4*t*w))/pow(cos(2*t*w) - 3, 3),
        0,
        pow(w, 2)*(-19*pow(sin(t*w), 10) + 31*pow(sin(t*w), 8) + 17*pow(sin(t*w), 6) - 33*pow(sin(t*w), 4)*pow(cos(t*w), 6) - 41*pow(sin(t*w), 4) - 10*pow(sin(t*w), 2) + 14*pow(cos(t*w), 10) - 2*pow(cos(t*w), 8) - 19*pow(cos(t*w), 6) - 2)*sin(t*w)/pow(pow(sin(t*w), 2) + 1, 4)
    };

    trajectory.segment(12, 4) = Eigen::Vector<double, 4>{
        a*pow(w, 3)*(-pow(sin(t*w), 6) + 43*pow(sin(t*w), 4) - 103*pow(sin(t*w), 2) + 45)*sin(t*w)/pow(pow(sin(t*w), 2) + 1, 4),
        2*a*pow(w, 3)*(6*pow(sin(t*w), 6) - 41*pow(sin(t*w), 4) + 44*pow(sin(t*w), 2) - 5)/pow(pow(sin(t*w), 2) + 1, 4),
        0,
        pow(w, 3)*(154*pow(sin(t*w), 16) - 117*pow(sin(t*w), 14) - 351*pow(sin(t*w), 12) + 370*pow(sin(t*w), 10)*pow(cos(t*w), 6) + 228*pow(sin(t*w), 10) + 385*pow(sin(t*w), 8)*pow(cos(t*w), 8) + 404*pow(sin(t*w), 8)*pow(cos(t*w), 6) + 276*pow(sin(t*w), 8) + 239*pow(sin(t*w), 6)*pow(cos(t*w), 10) + 197*pow(sin(t*w), 6)*pow(cos(t*w), 8) - 16*pow(sin(t*w), 6)*pow(cos(t*w), 6) - 21*pow(sin(t*w), 6) + 82*pow(sin(t*w), 4)*pow(cos(t*w), 12) + 21*pow(sin(t*w), 4)*pow(cos(t*w), 10) - 75*pow(sin(t*w), 4)*pow(cos(t*w), 8) - 52*pow(sin(t*w), 4)*pow(cos(t*w), 6) + 17*pow(sin(t*w), 4) + 6*pow(sin(t*w), 2) - 12*pow(cos(t*w), 16) + 24*pow(cos(t*w), 14) + 9*pow(cos(t*w), 12) - 11*pow(cos(t*w), 10) - 17*pow(cos(t*w), 8) - 2*pow(cos(t*w), 6))*cos(t*w)/pow(pow(sin(t*w), 2) + 1, 6)
    };

    trajectory.segment(16, 4) = Eigen::Vector<double, 4>{
        a*pow(w, 4)*(pow(sin(t*w), 8) - 136*pow(sin(t*w), 6) + 730*pow(sin(t*w), 4) - 624*pow(sin(t*w), 2) + 45)*cos(t*w)/pow(pow(sin(t*w), 2) + 1, 5),
        8*a*pow(w, 4)*(-3*pow(sin(t*w), 6) + 50*pow(sin(t*w), 4) - 107*pow(sin(t*w), 2) + 32)*sin(t*w)*cos(t*w)/pow(pow(sin(t*w), 2) + 1, 5),
        0,
        0
    };*/

    constexpr int N = 100;
    constexpr double T = 0.02;

    Eigen::Vector<double, 4*N> trajectory;

    for(int k=0; k<N; k++) {
        const double t = time + k*T;

        trajectory[4*k + 0] = a*cos(t*w)/(pow(sin(t*w), 2) + 1);
        trajectory[4*k + 1] = a*sin(t*w)*cos(t*w)/(pow(sin(t*w), 2) + 1);
        trajectory[4*k + 2] = 1;
        trajectory[4*k + 3] = fix(atan2(-a*w*pow(sin(t*w), 2)/(pow(sin(t*w), 2) + 1) + a*w*pow(cos(t*w), 2)/(pow(sin(t*w), 2) + 1) - 2*a*w*pow(sin(t*w), 2)*pow(cos(t*w), 2)/pow(pow(sin(t*w), 2) + 1, 2), -a*w*sin(t*w)/(pow(sin(t*w), 2) + 1) - 2*a*w*sin(t*w)*pow(cos(t*w), 2)/pow(pow(sin(t*w), 2) + 1, 2)));
    }

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
