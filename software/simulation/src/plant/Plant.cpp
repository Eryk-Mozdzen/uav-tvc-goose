#include "Plant.h"
#include "Params.h"

double Plant::current_w = 1000;

Plant::Plant() {
    DeclareContinuousState(12);
    DeclareVectorInputPort("control", 5);
    DeclareVectorOutputPort("state", 12, &Plant::EvalOutput, {this->all_state_ticket()});
}

void Plant::DoCalcTimeDerivatives(const drake::systems::Context<double> &context, drake::systems::ContinuousState<double> *derivatives) const {
    const Eigen::Vector<double, 5> real = EvalVectorInput(context, 0)->CopyToVector();
    const Eigen::Vector<double, 4> U = Plant::encodeControls(real);
    const Eigen::Vector<double, 12> x = context.get_continuous_state_vector().CopyToVector();

    const Eigen::Vector<double, 6> q = x.segment(0, 6);
    const Eigen::Vector<double, 6> dq = x.segment(6, 6);
    Eigen::Vector<double, 6> u;
    u.segment(0, 2).setZero();
    u.segment(2, 4) = U;

    Eigen::Vector<double, 12> dx;
    dx.segment(0, 6) = dq;
    dx.segment(6, 6) = M(q).inverse()*(u - C(q, dq)*dq - D(q));

    current_w = real(0);

    derivatives->get_mutable_vector().SetFromVector(dx);
}

void Plant::EvalOutput(const drake::systems::Context<double> &context, drake::systems::BasicVector<double> *output) const {
    const drake::systems::VectorBase<double> &state = context.get_continuous_state_vector();

    output->SetFrom(state);
}

Eigen::Matrix<double, Plant::dimQ, Plant::dimQ> Plant::M(const Q &q) {
    const Eigen::DiagonalMatrix<double, 3, 3> J(Params::J_xx, Params::J_yy, Params::J_zz);
    const Eigen::DiagonalMatrix<double, 3, 3> L(1/Params::l, 1/Params::l, -1/Params::r);

    Eigen::Matrix<double, dimQ, dimQ> M;

    M.setZero();
    M.block(0, 0, 3, 3) = Params::m*R(q).inverse();
    M.block(3, 3, 3, 3) = J*W(q).inverse()*L;

    M.row(0) +=M.row(4);
    M.row(1) -=M.row(3);

    return M;
}

Eigen::Matrix<double, Plant::dimQ, Plant::dimQ> Plant::C(const Q &q, const Q &dq) {
    const Eigen::DiagonalMatrix<double, 3, 3> J(Params::J_xx, Params::J_yy, Params::J_zz);
    const Eigen::DiagonalMatrix<double, 3, 3> L(1/Params::l, 1/Params::l, -1/Params::r);

    const double phi = q(3);
    const double theta = q(4);

    const double d_phi = dq(3);
    const double d_theta = dq(4);
    const double d_psi = dq(5);

    const double s_phi = std::sin(phi);
    const double c_phi = std::cos(phi);
    const double s_theta = std::sin(theta);
    const double c_theta = std::cos(theta);

    const Eigen::Matrix3d Winv = W(q).inverse();
    const Eigen::Matrix3d Rinv = R(q).inverse();

    const Eigen::Matrix3d C1 {
        {0, (Params::J_zz - Params::J_yy)*(-s_phi*d_theta + c_phi*c_theta*d_psi), 0},
        {0, 0, (Params::J_xx - Params::J_zz)*(d_phi - s_theta*d_psi)},
        {(Params::J_yy - Params::J_xx)*(c_phi*d_theta + c_theta*s_phi*d_psi), 0, 0}
    };

    const Eigen::Matrix3d C2 {
        { 0,                     Params::J_r*current_w, 0},
        {-Params::J_r*current_w, 0,                     0},
        { 0,                     0,                     0}
    };

    Eigen::Matrix<double, dimQ, dimQ> C;
    C.setZero();
    C.block(0, 0, 3, 3) = Params::m*Rinv*dR(q, dq)*Rinv;
    C.block(3, 3, 3, 3) = J*Winv*dW(q, dq)*Winv*L + C1*Winv*L + C2*Winv*L;

    C.row(0) +=C.row(4);
    C.row(1) -=C.row(3);

    return C;
}

Eigen::Vector<double, Plant::dimQ> Plant::D(const Q &q) {
    Eigen::Vector<double, dimQ> D;
    D.setZero();
    D.block(0, 0, 3, 1) = Params::m*R(q).inverse()*Eigen::Vector3d(0, 0, Params::g);

    const double Ft = Params::K_w*current_w*current_w;
    const double Fs = Params::K_l*Ft*Params::a_s;
    const double Mr = Params::K_m*current_w*current_w;
    D(5) +=(4*Fs + Mr/Params::r);

    D.row(0) +=D.row(4);
    D.row(1) -=D.row(3);

    return D;
}

Eigen::Matrix3d Plant::R(const Q &q) {
    const double phi = q(3);
    const double theta = q(4);
    const double psi = q(5);

    const double s_phi = std::sin(phi);
    const double c_phi = std::cos(phi);
    const double s_theta = std::sin(theta);
    const double c_theta = std::cos(theta);
    const double s_psi = std::sin(psi);
    const double c_psi = std::cos(psi);

    const Eigen::Matrix3d Rz {
        {c_psi, -s_psi, 0},
        {s_psi,  c_psi, 0},
        {0,      0,     1},
    };

    const Eigen::Matrix3d Ry {
        { c_theta, 0, s_theta},
        { 0,       1, 0      },
        {-s_theta, 0, c_theta},
    };

    const Eigen::Matrix3d Rx {
        {1, 0,      0    },
        {0, c_phi, -s_phi},
        {0, s_phi,  c_phi},
    };

    const Eigen::Matrix3d R = Rz*Ry*Rx;

    return R;
}

Eigen::Matrix3d Plant::W(const Q &q) {
    const double phi = q(3);
    const double theta = q(4);

    const double s_phi = std::sin(phi);
    const double c_phi = std::cos(phi);
    const double c_theta = std::cos(theta);
    const double t_theta = std::tan(theta);

    const Eigen::Matrix3d W {
        {1, s_phi*t_theta,  c_phi*t_theta},
        {0, c_phi,         -s_phi        },
        {0, s_phi/c_theta,  c_phi/c_theta},
    };

    return W;
}

Eigen::Matrix3d Plant::dR(const Q &q, const Q &dq) {
    const double phi = q(3);
    const double theta = q(4);
    const double psi = q(5);

    const double d_phi = dq(3);
    const double d_theta = dq(4);
    const double d_psi = dq(5);

    const double s_phi = std::sin(phi);
    const double c_phi = std::cos(phi);
    const double s_theta = std::sin(theta);
    const double c_theta = std::cos(theta);
    const double s_psi = std::sin(psi);
    const double c_psi = std::cos(psi);

    const Eigen::Matrix3d Rz {
        {c_psi, -s_psi, 0},
        {s_psi,  c_psi, 0},
        {0,      0,     1},
    };

    const Eigen::Matrix3d Ry {
        { c_theta, 0, s_theta},
        { 0,       1, 0      },
        {-s_theta, 0, c_theta},
    };

    const Eigen::Matrix3d Rx {
        {1, 0,      0    },
        {0, c_phi, -s_phi},
        {0, s_phi,  c_phi},
    };

    const Eigen::Matrix3d dRz {
        {-s_psi*d_psi, -c_psi*d_psi, 0},
        { c_psi*d_psi,  s_psi*d_psi, 0},
        { 0,            0,           0},
    };

    const Eigen::Matrix3d dRy {
        {-s_theta*d_theta, 0,  c_theta*d_theta},
        { 0,               0,  0              },
        {-c_theta*d_theta, 0, -s_theta*d_theta},
    };

    const Eigen::Matrix3d dRx {
        {0, 0,      0    },
        {0, -s_phi*d_phi, -c_phi*d_phi},
        {0,  c_phi*d_phi, -s_phi*d_phi},
    };

    const Eigen::Matrix3d dR = dRz*Ry*Rx + Rz*dRy*Rx + Rz*Ry*dRx;

    return dR;
}

Eigen::Matrix3d Plant::dW(const Q &q, const Q &dq) {
    const double phi = q(3);
    const double theta = q(4);

    const double d_phi = dq(3);
    const double d_theta = dq(4);

    const double sin_phi = std::sin(phi);
    const double cos_phi = std::cos(phi);
    const double tan_theta = std::tan(theta);
    const double sec_theta = 1./std::cos(theta);

    const Eigen::Matrix3d dW {
        {0,  sec_theta*sec_theta*sin_phi*d_theta + cos_phi*tan_theta*d_phi,  cos_phi*sec_theta*sec_theta*d_theta - sin_phi*tan_theta*d_phi},
        {0, -sin_phi*d_phi,                                                 -cos_phi*d_phi                                                },
        {0,  sec_theta*(sin_phi*tan_theta*d_theta + cos_phi*d_phi),          sec_theta*(cos_phi*tan_theta*d_theta - sin_phi*d_phi)        },
    };

    return dW;
}

Eigen::Vector<double, 5> Plant::decodeControls(const Eigen::Vector<double, 4> &generalized) {
    const double Ft = generalized(0);
    const double F31 = generalized(1);
    const double F42 = generalized(2);
    const double Fs = generalized(3);

    const double C31 = F31/(Params::K_l*Ft);
    const double C42 = F42/(Params::K_l*Ft);
    const double Cs = Fs/(Params::K_l*Ft);
    const double C = (Cs - C31 - C42)/2;

    const double w = std::sqrt(Ft/Params::K_w);
    const double a1 = 0.25*(2*C - C31 + C42);
    const double a2 = C - a1;
    const double a3 = C31 + a1;
    const double a4 = C42 + C - a1;

    return {w, a1, a2, a3, a4};
}

Eigen::Vector<double, 4> Plant::encodeControls(const Eigen::Vector<double, 5> &real) {
    const double w = real(0);
    const double a1 = real(1);
    const double a2 = real(2);
    const double a3 = real(3);
    const double a4 = real(4);

    const double F_t = Params::K_w*w*w;
    const double F_1 = Params::K_l*F_t*a1;
    const double F_2 = Params::K_l*F_t*a2;
    const double F_3 = Params::K_l*F_t*a3;
    const double F_4 = Params::K_l*F_t*a4;

    return {
        F_t,
        F_3 - F_1,
        F_4 - F_2,
        F_1 + F_2 + F_3 + F_4
    };
}
