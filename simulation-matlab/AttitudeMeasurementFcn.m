
function x = AttitudeMeasurementFcn(x)

    q_w = x(1);
    q_i = x(2);
    q_j = x(3);
    q_k = x(4);

    C = [
         2*q_j, -2*q_k,  2*q_w, -2*q_i, 0, 0, 0;
        -2*q_i, -2*q_w, -2*q_k, -2*q_j, 0, 0, 0;
        -2*q_w,  2*q_i,  2*q_j, -2*q_k, 0, 0, 0;

        -2*q_k, -2*q_j, -2*q_i, -2*q_w, 0, 0, 0;
        -2*q_w,  2*q_i, -2*q_j,  2*q_k, 0, 0, 0;
         2*q_i,  2*q_w, -2*q_k, -2*q_j, 0, 0, 0;
    ];

    x = C*x;
end
