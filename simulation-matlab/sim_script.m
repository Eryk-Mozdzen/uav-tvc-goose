clear all;

%% phisical parameters
g = 9.81;           % m/s^2
m = 1;              % kg

l = 0.1;            % m
r = 0.05;           % m

I_r = 0.0000001;    % kg*m^2
I_xx = 0.0001;      % kg*m^2
I_yy = 0.0001;      % kg*m^2
I_zz = 0.0001;      % kg*m^2

K_w = 1400;         % rad/s
K_f = 0.00001;      % N*(s/rad)^2
C_f = 0.01;         %
C_d = 0.1;          % N
C_t = 0.0001;       % m
C_ad = 0.01;        % kg/s

%% state estimation parameters

%mag_distortion = [1.5, -1, 0.5; 0.2, 0.5, 0.3; -0.3, 0.1, 1.1];
%mag_bias = [1.3, -0.2, 0.7];

mag_distortion = eye(3);
mag_bias = [0, 0, 0];

attitude_filter_alpha = 0.997;
attitude_filter_beta = 0.001;

%% equilibrium point

actuators_0 = [
    asin(C_t/(4*C_f*r))
    asin(C_t/(4*C_f*r))
    asin(C_t/(4*C_f*r))
    asin(C_t/(4*C_f*r))
    sqrt((m*g/(1 - C_d))/K_f)/K_w;
];

%% linear model hover

w_t0 = actuators_0(5)*K_w;
F_t0 = K_f*w_t0^2;

A = [
    0,  0, 0, 1,              0,             0, 0, 0
    0,  0, 0, 0,              1,             0, 0, 0
    0,  0, 0, 0,              0,             1, 0, 0
    0,  0, 0, 0,              I_r*w_t0/I_xx, 0, 0, 0
    0,  0, 0, -I_r*w_t0/I_yy, 0,             0, 0, 0
    0,  0, 0, 0,              0,             0, 0, 0
    0,  0, 0, 0,              0,             0, 0, 1
    0,  0, 0, 0,              0,             0, 0, 0
];

B = [
    0,                  0,                  0,                  0,                0
    0,                  0,                  0,                  0,                0
    0,                  0,                  0,                  0,                0
    0,                  -F_t0*l*C_f/I_xx,   0,                  F_t0*l*C_f/I_xx,  0
    F_t0*l*C_f/I_yy,    0,                  -F_t0*l*C_f/I_yy,   0,                0
    -F_t0*r*C_f/I_zz,   -F_t0*r*C_f/I_zz,   -F_t0*r*C_f/I_zz,   -F_t0*r*C_f/I_zz, 0
    0,                  0,                  0,                  0,                0
    0,                  0,                  0,                  0,                2*(1-C_d)*K_f*w_t0*K_w/m
];

%% regulator

roll_pitch_max = 0.1;   % rad
w_x_y_max = 1;          % rad/s

yaw_max = 0.5;          % rad
w_z_max = 2;            % rad/s

z_max = 0.5;            % m
v_z_max = 1;            % m/s

Q = diag([ ...
    1/(roll_pitch_max^2) ...
    1/(roll_pitch_max^2) ...
    1/(yaw_max^2) ...
    1/(w_x_y_max^2) ...
    1/(w_x_y_max^2) ...
    1/(w_z_max^2) ...
    1/(z_max^2) ...
    1/(v_z_max^2) ...
]);

alpha_max = deg2rad(10);    % rad
throttle_max = 0.01;        % 

R = diag([ ...
    1/(alpha_max^2) ...
    1/(alpha_max^2) ...
    1/(alpha_max^2) ...
    1/(alpha_max^2) ...
    1/(throttle_max^2) ...
 ]);

K = lqr(A, B, Q, R, 0);
