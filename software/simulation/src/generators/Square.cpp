#include "Square.h"

Square::Square(const double a, const double R, const double T) : TrajectoryGenerator{4, 3}, a{a}, R{R}, v{(4*a - 8*R + 2*pi*R)/T}, w{(4*a - 8*R + 2*pi*R)/(T*R)}, T{T} {

}

Eigen::VectorX<double> Square::value(const double &time) const {
    double t = time;
    double k = 0;
    while(t>T) {
        t -=T;
        k++;
    }

    const double tv = (a-2*R)/v;
    const double tw = (pi*R)/(2*v);

    Eigen::Vector<double, 12> trajectory;

    if(t<1*tv+0*tw) {
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{0.5*a, -0.5*a + R + v*t, 1, 0.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{0, v, 0, 0};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{0, 0, 0, 0};
    } else if(t<1*tv+1*tw) {
        const double s = std::sin(w*(t - tv));
        const double c = std::cos(w*(t - tv));
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{R*c + 0.5*a - R, R*s + 0.5*a - R, 1, w*(t-tv) + 0.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{-R*w*s, R*w*c, 0, w};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{-R*w*w*c, -R*w*w*s, 0, 0};
    } else if(t<2*tv+1*tw) {
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{0.5*a - R - v*(t-tv-tw), 0.5*a, 1, pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{-v, 0, 0, 0};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{0, 0, 0, 0};
    } else if(t<2*tv+2*tw) {
        const double s = std::sin(w*(t - 2*tv));
        const double c = std::cos(w*(t - 2*tv));
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{R*c - 0.5*a + R, R*s + 0.5*a - R, 1, w*(t-2*tv) + 0.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{-R*w*s, R*w*c, 0, w};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{-R*w*w*c, -R*w*w*s, 0, 0};
    } else if(t<3*tv+2*tw) {
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{-0.5*a, 0.5*a - R - v*(t-2*tv-2*tw), 1, 1.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{0, -v, 0, 0};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{0, 0, 0, 0};
    } else if(t<3*tv+3*tw) {
        const double s = std::sin(w*(t - 3*tv));
        const double c = std::cos(w*(t - 3*tv));
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{R*c - 0.5*a + R, R*s - 0.5*a + R, 1, w*(t-3*tv) + 0.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{-R*w*s, R*w*c, 0, w};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{-R*w*w*c, -R*w*w*s, 0, 0};
    } else if(t<4*tv+3*tw) {
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{-0.5*a + R + v*(t-3*tv-3*tw), -0.5*a, 1, 2*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{v, 0, 0, 0};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{0, 0, 0, 0};
    } else {
        const double s = std::sin(w*(t - 4*tv));
        const double c = std::cos(w*(t - 4*tv));
        trajectory.segment(0, 4) = Eigen::Vector<double, 4>{R*c + 0.5*a - R, R*s - 0.5*a + R, 1, w*(t-4*tv) + 0.5*pi + 2*k*pi};
        trajectory.segment(4, 4) = Eigen::Vector<double, 4>{-R*w*s, R*w*c, 0, w};
        trajectory.segment(8, 4) = Eigen::Vector<double, 4>{-R*w*w*c, -R*w*w*s, 0, 0};
    }

    return trajectory;
}
