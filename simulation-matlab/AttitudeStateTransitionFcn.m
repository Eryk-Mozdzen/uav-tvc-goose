
function x = AttitudeStateTransitionFcn(x, u)

    dt = 0.01;

    q_w = x(1);
    q_i = x(2);
    q_j = x(3);
    q_k = x(4);

    A = [
        1, 0, 0, 0,  0.5*dt*q_i,  0.5*dt*q_j,  0.5*dt*q_k;
        0, 1, 0, 0, -0.5*dt*q_w,  0.5*dt*q_k, -0.5*dt*q_j;
        0, 0, 1, 0, -0.5*dt*q_k, -0.5*dt*q_w,  0.5*dt*q_i;
        0, 0, 0, 1,  0.5*dt*q_j, -0.5*dt*q_i, -0.5*dt*q_w;

        0, 0, 0, 0, 1, 0, 0;
        0, 0, 0, 0, 0, 1, 0;
        0, 0, 0, 0, 0, 0, 1;
    ];

    B = [
        -q_i, -q_j, -q_k;
         q_w, -q_k,  q_j;
         q_k,  q_w, -q_i;
        -q_j,  q_i,  q_w;
        0,    0,    0;
        0,    0,    0;
        0,    0,    0;
    ];

    x = A*x + 0.5*dt*B*u;
end
