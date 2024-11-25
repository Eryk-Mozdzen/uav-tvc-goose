#ifndef EKF_H
#define EKF_H

#include "arm_math.h"

typedef struct {
    arm_matrix_instance_f32 P;
    arm_matrix_instance_f32 x;
} ekf_t;

typedef struct {
    arm_matrix_instance_f32 Q;
    void (*f)(const float *x, const float *u, float *x_next);
	void (*df)(const float *x, const float *u, float *x_next);
} ekf_system_model_t;

typedef struct {
    arm_matrix_instance_f32 R;
    void (*h)(const float *x, float *z);
	void (*dh)(const float *x, float *z);
} ekf_measurement_model_t;

#define EKF_PREDICT_DEF(X, U) \
    void ekf_predict_##X##_##U(ekf_t *ekf, const ekf_system_model_t *system, const float *u_data);

#define EKF_CORRECT_DEF(X, Z) \
    void ekf_correct_##X##_##Z(ekf_t *ekf, const ekf_measurement_model_t *measurement, const float *z_data);

#define EKF_PREDICT(X, U) \
    void ekf_predict_##X##_##U(ekf_t *ekf, const ekf_system_model_t *system, const float *u_data) { \
        arm_matrix_instance_f32 u = { \
            .numRows = U, \
            .numCols = 1, \
            .pData = (float *)u_data \
        }; \
 \
        float F_data[X*X]; \
        arm_matrix_instance_f32 F = { \
            .numRows = X, \
            .numCols = X, \
            .pData = F_data \
        }; \
 \
        system->df(ekf->x.pData, u.pData, F.pData); \
 \
        float x_next_data[X]; \
        arm_matrix_instance_f32 x_next = { \
            .numRows = X, \
            .numCols = 1, \
            .pData = x_next_data \
        }; \
        system->f(ekf->x.pData, u.pData, x_next.pData); \
        memcpy(ekf->x.pData, x_next.pData, X*sizeof(float)); \
 \
        float FT_data[X*X]; \
        arm_matrix_instance_f32 FT = { \
            .numRows = X, \
            .numCols = X, \
            .pData = FT_data \
        }; \
 \
        arm_mat_trans_f32(&F, &FT); \
 \
        float FP_data[X*X]; \
        arm_matrix_instance_f32 FP = { \
            .numRows = X, \
            .numCols = X, \
            .pData = FP_data \
        }; \
 \
        float FPFT_data[X*X]; \
        arm_matrix_instance_f32 FPFT = { \
            .numRows = X, \
            .numCols = X, \
            .pData = FPFT_data \
        }; \
 \
        arm_mat_mult_f32(&F, &ekf->P, &FP); \
        arm_mat_mult_f32(&FP, &FT, &FPFT); \
        arm_mat_add_f32(&FPFT, &system->Q, &ekf->P); \
    }

#define EKF_CORRECT(X, Z) \
    void ekf_correct_##X##_##Z(ekf_t *ekf, const ekf_measurement_model_t *measurement, const float *z_data) { \
        float H_data[Z*X]; \
        arm_matrix_instance_f32 H = { \
            .numRows = Z, \
            .numCols = X, \
            .pData = H_data \
        }; \
 \
        measurement->dh(ekf->x.pData, H.pData); \
 \
        float HT_data[X*Z]; \
        arm_matrix_instance_f32 HT = { \
            .numRows = X, \
            .numCols = Z, \
            .pData = HT_data \
        }; \
 \
        arm_mat_trans_f32(&H, &HT); \
 \
        float y_data[Z]; \
        arm_matrix_instance_f32 y = { \
            .numRows = Z, \
            .numCols = 1, \
            .pData = y_data \
        }; \
 \
        arm_matrix_instance_f32 z = { \
            .numRows = Z, \
            .numCols = 1, \
            .pData = (float *)z_data \
        }; \
 \
        float h_data[Z]; \
        arm_matrix_instance_f32 h = { \
            .numRows = Z, \
            .numCols = 1, \
            .pData = h_data \
        }; \
        measurement->h(ekf->x.pData, h.pData); \
        arm_mat_sub_f32(&z, &h, &y); \
 \
        float PHT_data[X*Z]; \
        arm_matrix_instance_f32 PHT = { \
            .numRows = X, \
            .numCols = Z, \
            .pData = PHT_data \
        }; \
 \
        float HPHT_data[Z*Z]; \
        arm_matrix_instance_f32 HPHT = { \
            .numRows = Z, \
            .numCols = Z, \
            .pData = HPHT_data \
        }; \
 \
        float S_data[Z*Z]; \
        arm_matrix_instance_f32 S = { \
            .numRows = Z, \
            .numCols = Z, \
            .pData = S_data \
        }; \
 \
        arm_mat_mult_f32(&ekf->P, &HT, &PHT); \
        arm_mat_mult_f32(&H, &PHT, &HPHT); \
        arm_mat_add_f32(&HPHT, &measurement->R, &S); \
 \
        float K_data[X*Z]; \
        arm_matrix_instance_f32 K = { \
            .numRows = X, \
            .numCols = Z, \
            .pData = K_data \
        }; \
 \
        float Sinv_data[Z*Z]; \
        arm_matrix_instance_f32 Sinv = { \
            .numRows = Z, \
            .numCols = Z, \
            .pData = Sinv_data \
        }; \
 \
        arm_mat_inverse_f32(&S, &Sinv); \
        arm_mat_mult_f32(&PHT, &Sinv, &K); \
 \
        float Ky_data[X]; \
        arm_matrix_instance_f32 Ky = { \
            .numRows = X, \
            .numCols = 1, \
            .pData = Ky_data \
        }; \
 \
        float x_Ky_data[X]; \
        arm_matrix_instance_f32 x_Ky = { \
            .numRows = X, \
            .numCols = 1, \
            .pData = x_Ky_data \
        }; \
 \
        arm_mat_mult_f32(&K, &y, &Ky); \
        arm_mat_add_f32(&ekf->x, &Ky, &x_Ky); \
        memcpy(ekf->x.pData, x_Ky.pData, X*sizeof(float)); \
 \
        float I_data[X*X]; \
        arm_matrix_instance_f32 I = { \
            .numRows = X, \
            .numCols = X, \
            .pData = I_data \
        }; \
        for(uint16_t i=0; i<X*X; i++) { \
            I.pData[i] = 0.f; \
        } \
        for(uint16_t i=0; i<X; i++) { \
            I.pData[i*X + i] = 1.f; \
        } \
 \
        float KH_data[X*X]; \
        arm_matrix_instance_f32 KH = { \
            .numRows = X, \
            .numCols = X, \
            .pData = KH_data \
        }; \
 \
        float I_KH_data[X*X]; \
        arm_matrix_instance_f32 I_KH = { \
            .numRows = X, \
            .numCols = X, \
            .pData = I_KH_data \
        }; \
 \
        float P_next_data[X*X]; \
        arm_matrix_instance_f32 P_next = { \
            .numRows = X, \
            .numCols = X, \
            .pData = P_next_data \
        }; \
 \
        arm_mat_mult_f32(&K, &H, &KH); \
        arm_mat_sub_f32(&I, &KH, &I_KH); \
        arm_mat_mult_f32(&I_KH, &ekf->P, &P_next); \
        memcpy(ekf->P.pData, P_next.pData, X*X*sizeof(float)); \
    }

#endif
