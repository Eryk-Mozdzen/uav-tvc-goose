#include "arm_math.h"

#include "msg.h"
#include "nvm.h"

static float e_i_data[4] = {0};
static arm_matrix_instance_f32 e_i = {
    .numRows = 4,
    .numCols = 1,
    .pData = e_i_data,
};

static float i_prev_data[4] = {0};
static arm_matrix_instance_f32 i_prev = {
    .numRows = 4,
    .numCols = 1,
    .pData = i_prev_data,
};

static float G_data[] = {
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
};
static const arm_matrix_instance_f32 G = {
    .numRows = 4,
    .numCols = 8,
    .pData = G_data,
};

void controller_reset() {
    memset(e_i.pData,    0, e_i.numRows*sizeof(float));
    memset(i_prev.pData, 0, i_prev.numRows*sizeof(float));
}

void controller_calculate(msg_frame_controller_t *controller) {
    float x_data[] = {
        controller->process.rpy[0],
        controller->process.rpy[1],
        controller->process.rpy[2],
        controller->process.omega[0],
        controller->process.omega[1],
        controller->process.omega[2],
        controller->process.pos[2],
        controller->process.vel[2],
    };
    const arm_matrix_instance_f32 x = {
        .numRows = 8,
        .numCols = 1,
        .pData = x_data,
    };

    float xd_data[] = {
        controller->setpoint.rpy[0],
        controller->setpoint.rpy[1],
        controller->setpoint.rpy[2],
        controller->setpoint.omega[0],
        controller->setpoint.omega[1],
        controller->setpoint.omega[2],
        controller->setpoint.pos[2],
        controller->setpoint.vel[2],
    };
    const arm_matrix_instance_f32 xd = {
        .numRows = 8,
        .numCols = 1,
        .pData = xd_data,
    };

    float e_p_data[8];
    arm_matrix_instance_f32 e_p = {
        .numRows = 8,
        .numCols = 1,
        .pData = e_p_data,
    };

    float e_pi_data[12];
    arm_matrix_instance_f32 e_pi = {
        .numRows = 12,
        .numCols = 1,
        .pData = e_pi_data,
    };

    msg_frame_gains_t gains;
    nvm_read(0xFF, &gains, sizeof(gains));

    const arm_matrix_instance_f32 K = {
        .numRows = 4,
        .numCols = 12,
        .pData = gains.K,
    };

    const arm_matrix_instance_f32 u0 = {
        .numRows = 4,
        .numCols = 1,
        .pData = gains.u0,
    };

    float u_data[4];
    arm_matrix_instance_f32 u = {
        .numRows = 4,
        .numCols = 1,
        .pData = u_data,
    };

    float i_data[4];
    arm_matrix_instance_f32 i = {
        .numRows = 4,
        .numCols = 1,
        .pData = i_data,
    };

    float i_sum_data[4];
    arm_matrix_instance_f32 i_sum = {
        .numRows = 4,
        .numCols = 1,
        .pData = i_sum_data,
    };

    float i_trap_data[4];
    arm_matrix_instance_f32 i_trap = {
        .numRows = 4,
        .numCols = 1,
        .pData = i_trap_data,
    };

    float e_i_new_data[4];
    arm_matrix_instance_f32 e_i_new = {
        .numRows = 4,
        .numCols = 1,
        .pData = e_i_new_data,
    };

    float Ke_data[4];
    arm_matrix_instance_f32 Ke = {
        .numRows = 4,
        .numCols = 1,
        .pData = Ke_data,
    };

    float mKe_data[4];
    arm_matrix_instance_f32 mKe = {
        .numRows = 4,
        .numCols = 1,
        .pData = mKe_data,
    };

    arm_mat_sub_f32(&x, &xd, &e_p);

    while(e_p.pData[2]>=PI) {
        e_p.pData[2] -=2*PI;
    }

    while(e_p.pData[2]<=-PI) {
        e_p.pData[2] +=2*PI;
    }

    const float dt = 0.001f;

    arm_mat_mult_f32(&G, &e_p, &i);
    arm_mat_add_f32(&i, &i_prev, &i_sum);
    arm_mat_scale_f32(&i_sum, dt*0.5f, &i_trap);
    arm_mat_add_f32(&e_i, &i_trap, &e_i_new);
    memcpy(e_i.pData, e_i_new.pData, e_i.numRows*sizeof(float));
    memcpy(i_prev.pData, i.pData, i.numRows*sizeof(float));

    memcpy(&e_pi.pData[0],           e_p.pData, e_p.numRows*sizeof(float));
    memcpy(&e_pi.pData[e_p.numRows], e_i.pData, e_i.numRows*sizeof(float));

    arm_mat_mult_f32(&K, &e_pi, &Ke);
    arm_mat_scale_f32(&Ke, -1.f, &mKe);
    arm_mat_add_f32(&mKe, &u0, &u);

    controller->controls.throttle = u.pData[0];
    controller->controls.angles[0] = u.pData[1];
    controller->controls.angles[1] = u.pData[2];
    controller->controls.angles[2] = u.pData[3];
}
