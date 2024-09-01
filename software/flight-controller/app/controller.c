#include "arm_math.h"

#include "controller_params.h"
#include "msg.h"

void controller_calculate(msg_frame_controller_t *controller) {
    float x_data[] = {
        controller->process.rpy[0],
        controller->process.rpy[1],
        controller->process.omega[0],
        controller->process.omega[1],
        controller->process.omega[2],
        controller->process.pos[2],
        controller->process.vel[2],
    };
    const arm_matrix_instance_f32 x = {
        .numRows = 7,
        .numCols = 1,
        .pData = x_data,
    };

    float xd_data[] = {
        controller->setpoint.rpy[0],
        controller->setpoint.rpy[1],
        controller->setpoint.omega[0],
        controller->setpoint.omega[1],
        controller->setpoint.omega[2],
        controller->setpoint.pos[2],
        controller->setpoint.vel[2],
    };
    const arm_matrix_instance_f32 xd = {
        .numRows = 7,
        .numCols = 1,
        .pData = xd_data,
    };

    float e_data[7];
    arm_matrix_instance_f32 e = {
        .numRows = 7,
        .numCols = 1,
        .pData = e_data,
    };

    float tmp_data[4];
    arm_matrix_instance_f32 tmp = {
        .numRows = 4,
        .numCols = 1,
        .pData = tmp_data,
    };

    float u_data[4];
    arm_matrix_instance_f32 u = {
        .numRows = 4,
        .numCols = 1,
        .pData = u_data,
    };

    arm_mat_sub_f32(&xd, &x, &e);
    arm_mat_mult_f32(&K, &e, &tmp);
    arm_mat_add_f32(&tmp, &u0, &u);

    controller->controls.throttle = u.pData[0];
    controller->controls.angles[0] = u.pData[1];
    controller->controls.angles[1] = u.pData[2];
    controller->controls.angles[2] = u.pData[3];
}
