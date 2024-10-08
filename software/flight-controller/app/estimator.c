// auto-generated file
// 2024-09-01 11:04:31

#include <math.h>

#include "ekf.h"

#define g 9.80665f
#define T 0.001f

static float x_data[15] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102400,
};

static float P_data[15*15] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

ekf_t ekf = {
	.x.numRows = 15,
	.x.numCols = 1,
	.x.pData = x_data,
	.P.numRows = 15,
	.P.numCols = 15,
	.P.pData = P_data,
};

static void system_f(const float *x, const float *u, float *x_next) {
	const float w_x = u[0];
	const float w_y = u[1];
	const float w_z = u[2];
	const float a_x = u[3];
	const float a_y = u[4];
	const float a_z = u[5];

	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];
	const float p_x = x[7];
	const float p_y = x[8];
	const float p_z = x[9];
	const float v_x = x[10];
	const float v_y = x[11];
	const float v_z = x[12];
	const float theta_d = x[13];
	const float p_0 = x[14];

	x_next[0] = -0.5F*T*q_x*w_x - 0.5F*T*q_y*w_y - 0.5F*T*q_z*w_z + q_w;
	x_next[1] = 0.5F*T*q_w*w_x + 0.5F*T*q_y*w_z - 0.5F*T*q_z*w_y + q_x;
	x_next[2] = 0.5F*T*q_w*w_y - 0.5F*T*q_x*w_z + 0.5F*T*q_z*w_x + q_y;
	x_next[3] = 0.5F*T*q_w*w_z + 0.5F*T*q_x*w_y - 0.5F*T*q_y*w_x + q_z;
	x_next[4] = w_x;
	x_next[5] = w_y;
	x_next[6] = w_z;
	x_next[7] = -0.5F*(T)*(T)*(a_x*((q_w)*(q_w) + (q_x)*(q_x) - (q_y)*(q_y) - (q_z)*(q_z)) + a_y*(-2*q_w*q_z + 2*q_x*q_y) + a_z*(2*q_w*q_y + 2*q_x*q_z)) + T*v_x + p_x;
	x_next[8] = -0.5F*(T)*(T)*(a_x*(2*q_w*q_z + 2*q_x*q_y) + a_y*((q_w)*(q_w) - (q_x)*(q_x) + (q_y)*(q_y) - (q_z)*(q_z)) + a_z*(-2*q_w*q_x + 2*q_y*q_z)) + T*v_y + p_y;
	x_next[9] = -0.5F*(T)*(T)*(a_x*(-2*q_w*q_y + 2*q_x*q_z) + a_y*(2*q_w*q_x + 2*q_y*q_z) + a_z*((q_w)*(q_w) - (q_x)*(q_x) - (q_y)*(q_y) + (q_z)*(q_z)) + g) + T*v_z + p_z;
	x_next[10] = -T*(a_x*((q_w)*(q_w) + (q_x)*(q_x) - (q_y)*(q_y) - (q_z)*(q_z)) + a_y*(-2*q_w*q_z + 2*q_x*q_y) + a_z*(2*q_w*q_y + 2*q_x*q_z)) + v_x;
	x_next[11] = -T*(a_x*(2*q_w*q_z + 2*q_x*q_y) + a_y*((q_w)*(q_w) - (q_x)*(q_x) + (q_y)*(q_y) - (q_z)*(q_z)) + a_z*(-2*q_w*q_x + 2*q_y*q_z)) + v_y;
	x_next[12] = -T*(a_x*(-2*q_w*q_y + 2*q_x*q_z) + a_y*(2*q_w*q_x + 2*q_y*q_z) + a_z*((q_w)*(q_w) - (q_x)*(q_x) - (q_y)*(q_y) + (q_z)*(q_z)) + g) + v_z;
	x_next[13] = theta_d;
	x_next[14] = p_0;
}

static void system_df(const float *x, const float *u, float *x_next) {
	const float w_x = u[0];
	const float w_y = u[1];
	const float w_z = u[2];
	const float a_x = u[3];
	const float a_y = u[4];
	const float a_z = u[5];

	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];

	x_next[0] = 1;
	x_next[1] = -0.5F*T*w_x;
	x_next[2] = -0.5F*T*w_y;
	x_next[3] = -0.5F*T*w_z;
	x_next[4] = 0;
	x_next[5] = 0;
	x_next[6] = 0;
	x_next[7] = 0;
	x_next[8] = 0;
	x_next[9] = 0;
	x_next[10] = 0;
	x_next[11] = 0;
	x_next[12] = 0;
	x_next[13] = 0;
	x_next[14] = 0;

	x_next[15] = 0.5F*T*w_x;
	x_next[16] = 1;
	x_next[17] = 0.5F*T*w_z;
	x_next[18] = -0.5F*T*w_y;
	x_next[19] = 0;
	x_next[20] = 0;
	x_next[21] = 0;
	x_next[22] = 0;
	x_next[23] = 0;
	x_next[24] = 0;
	x_next[25] = 0;
	x_next[26] = 0;
	x_next[27] = 0;
	x_next[28] = 0;
	x_next[29] = 0;

	x_next[30] = 0.5F*T*w_y;
	x_next[31] = -0.5F*T*w_z;
	x_next[32] = 1;
	x_next[33] = 0.5F*T*w_x;
	x_next[34] = 0;
	x_next[35] = 0;
	x_next[36] = 0;
	x_next[37] = 0;
	x_next[38] = 0;
	x_next[39] = 0;
	x_next[40] = 0;
	x_next[41] = 0;
	x_next[42] = 0;
	x_next[43] = 0;
	x_next[44] = 0;

	x_next[45] = 0.5F*T*w_z;
	x_next[46] = 0.5F*T*w_y;
	x_next[47] = -0.5F*T*w_x;
	x_next[48] = 1;
	x_next[49] = 0;
	x_next[50] = 0;
	x_next[51] = 0;
	x_next[52] = 0;
	x_next[53] = 0;
	x_next[54] = 0;
	x_next[55] = 0;
	x_next[56] = 0;
	x_next[57] = 0;
	x_next[58] = 0;
	x_next[59] = 0;

	x_next[60] = 0;
	x_next[61] = 0;
	x_next[62] = 0;
	x_next[63] = 0;
	x_next[64] = 0;
	x_next[65] = 0;
	x_next[66] = 0;
	x_next[67] = 0;
	x_next[68] = 0;
	x_next[69] = 0;
	x_next[70] = 0;
	x_next[71] = 0;
	x_next[72] = 0;
	x_next[73] = 0;
	x_next[74] = 0;

	x_next[75] = 0;
	x_next[76] = 0;
	x_next[77] = 0;
	x_next[78] = 0;
	x_next[79] = 0;
	x_next[80] = 0;
	x_next[81] = 0;
	x_next[82] = 0;
	x_next[83] = 0;
	x_next[84] = 0;
	x_next[85] = 0;
	x_next[86] = 0;
	x_next[87] = 0;
	x_next[88] = 0;
	x_next[89] = 0;

	x_next[90] = 0;
	x_next[91] = 0;
	x_next[92] = 0;
	x_next[93] = 0;
	x_next[94] = 0;
	x_next[95] = 0;
	x_next[96] = 0;
	x_next[97] = 0;
	x_next[98] = 0;
	x_next[99] = 0;
	x_next[100] = 0;
	x_next[101] = 0;
	x_next[102] = 0;
	x_next[103] = 0;
	x_next[104] = 0;

	x_next[105] = -0.5F*(T)*(T)*(2*a_x*q_w - 2*a_y*q_z + 2*a_z*q_y);
	x_next[106] = -0.5F*(T)*(T)*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[107] = -0.5F*(T)*(T)*(-2*a_x*q_y + 2*a_y*q_x + 2*a_z*q_w);
	x_next[108] = -0.5F*(T)*(T)*(-2*a_x*q_z - 2*a_y*q_w + 2*a_z*q_x);
	x_next[109] = 0;
	x_next[110] = 0;
	x_next[111] = 0;
	x_next[112] = 1;
	x_next[113] = 0;
	x_next[114] = 0;
	x_next[115] = T;
	x_next[116] = 0;
	x_next[117] = 0;
	x_next[118] = 0;
	x_next[119] = 0;

	x_next[120] = -0.5F*(T)*(T)*(2*a_x*q_z + 2*a_y*q_w - 2*a_z*q_x);
	x_next[121] = -0.5F*(T)*(T)*(2*a_x*q_y - 2*a_y*q_x - 2*a_z*q_w);
	x_next[122] = -0.5F*(T)*(T)*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[123] = -0.5F*(T)*(T)*(2*a_x*q_w - 2*a_y*q_z + 2*a_z*q_y);
	x_next[124] = 0;
	x_next[125] = 0;
	x_next[126] = 0;
	x_next[127] = 0;
	x_next[128] = 1;
	x_next[129] = 0;
	x_next[130] = 0;
	x_next[131] = T;
	x_next[132] = 0;
	x_next[133] = 0;
	x_next[134] = 0;

	x_next[135] = -0.5F*(T)*(T)*(-2*a_x*q_y + 2*a_y*q_x + 2*a_z*q_w);
	x_next[136] = -0.5F*(T)*(T)*(2*a_x*q_z + 2*a_y*q_w - 2*a_z*q_x);
	x_next[137] = -0.5F*(T)*(T)*(-2*a_x*q_w + 2*a_y*q_z - 2*a_z*q_y);
	x_next[138] = -0.5F*(T)*(T)*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[139] = 0;
	x_next[140] = 0;
	x_next[141] = 0;
	x_next[142] = 0;
	x_next[143] = 0;
	x_next[144] = 1;
	x_next[145] = 0;
	x_next[146] = 0;
	x_next[147] = T;
	x_next[148] = 0;
	x_next[149] = 0;

	x_next[150] = -T*(2*a_x*q_w - 2*a_y*q_z + 2*a_z*q_y);
	x_next[151] = -T*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[152] = -T*(-2*a_x*q_y + 2*a_y*q_x + 2*a_z*q_w);
	x_next[153] = -T*(-2*a_x*q_z - 2*a_y*q_w + 2*a_z*q_x);
	x_next[154] = 0;
	x_next[155] = 0;
	x_next[156] = 0;
	x_next[157] = 0;
	x_next[158] = 0;
	x_next[159] = 0;
	x_next[160] = 1;
	x_next[161] = 0;
	x_next[162] = 0;
	x_next[163] = 0;
	x_next[164] = 0;

	x_next[165] = -T*(2*a_x*q_z + 2*a_y*q_w - 2*a_z*q_x);
	x_next[166] = -T*(2*a_x*q_y - 2*a_y*q_x - 2*a_z*q_w);
	x_next[167] = -T*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[168] = -T*(2*a_x*q_w - 2*a_y*q_z + 2*a_z*q_y);
	x_next[169] = 0;
	x_next[170] = 0;
	x_next[171] = 0;
	x_next[172] = 0;
	x_next[173] = 0;
	x_next[174] = 0;
	x_next[175] = 0;
	x_next[176] = 1;
	x_next[177] = 0;
	x_next[178] = 0;
	x_next[179] = 0;

	x_next[180] = -T*(-2*a_x*q_y + 2*a_y*q_x + 2*a_z*q_w);
	x_next[181] = -T*(2*a_x*q_z + 2*a_y*q_w - 2*a_z*q_x);
	x_next[182] = -T*(-2*a_x*q_w + 2*a_y*q_z - 2*a_z*q_y);
	x_next[183] = -T*(2*a_x*q_x + 2*a_y*q_y + 2*a_z*q_z);
	x_next[184] = 0;
	x_next[185] = 0;
	x_next[186] = 0;
	x_next[187] = 0;
	x_next[188] = 0;
	x_next[189] = 0;
	x_next[190] = 0;
	x_next[191] = 0;
	x_next[192] = 1;
	x_next[193] = 0;
	x_next[194] = 0;

	x_next[195] = 0;
	x_next[196] = 0;
	x_next[197] = 0;
	x_next[198] = 0;
	x_next[199] = 0;
	x_next[200] = 0;
	x_next[201] = 0;
	x_next[202] = 0;
	x_next[203] = 0;
	x_next[204] = 0;
	x_next[205] = 0;
	x_next[206] = 0;
	x_next[207] = 0;
	x_next[208] = 1;
	x_next[209] = 0;

	x_next[210] = 0;
	x_next[211] = 0;
	x_next[212] = 0;
	x_next[213] = 0;
	x_next[214] = 0;
	x_next[215] = 0;
	x_next[216] = 0;
	x_next[217] = 0;
	x_next[218] = 0;
	x_next[219] = 0;
	x_next[220] = 0;
	x_next[221] = 0;
	x_next[222] = 0;
	x_next[223] = 0;
	x_next[224] = 1;
}

static float system_Q_data[15*15] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

ekf_system_model_t system_model = {
	.Q.numRows = 15,
	.Q.numCols = 15,
	.Q.pData = system_Q_data,
	.f = system_f,
	.df = system_df,
};

static void magnetometer_h(const float *x, float *z) {
	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];
	const float theta_d = x[13];

	z[0] = (-2*q_w*q_y + 2*q_x*q_z)*sinf(theta_d) + (2*q_w*q_z + 2*q_x*q_y)*cosf(theta_d);
	z[1] = (2*q_w*q_x + 2*q_y*q_z)*sinf(theta_d) + ((q_w)*(q_w) - (q_x)*(q_x) + (q_y)*(q_y) - (q_z)*(q_z))*cosf(theta_d);
	z[2] = (-2*q_w*q_x + 2*q_y*q_z)*cosf(theta_d) + ((q_w)*(q_w) - (q_x)*(q_x) - (q_y)*(q_y) + (q_z)*(q_z))*sinf(theta_d);
}

static void magnetometer_dh(const float *x, float *z) {
	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];
	const float theta_d = x[13];

	z[0] = -2*q_y*sinf(theta_d) + 2*q_z*cosf(theta_d);
	z[1] = 2*q_y*cosf(theta_d) + 2*q_z*sinf(theta_d);
	z[2] = -2*q_w*sinf(theta_d) + 2*q_x*cosf(theta_d);
	z[3] = 2*q_w*cosf(theta_d) + 2*q_x*sinf(theta_d);
	z[4] = 0;
	z[5] = 0;
	z[6] = 0;
	z[7] = 0;
	z[8] = 0;
	z[9] = 0;
	z[10] = 0;
	z[11] = 0;
	z[12] = 0;
	z[13] = (-2*q_w*q_y + 2*q_x*q_z)*cosf(theta_d) - (2*q_w*q_z + 2*q_x*q_y)*sinf(theta_d);
	z[14] = 0;

	z[15] = 2*q_w*cosf(theta_d) + 2*q_x*sinf(theta_d);
	z[16] = 2*q_w*sinf(theta_d) - 2*q_x*cosf(theta_d);
	z[17] = 2*q_y*cosf(theta_d) + 2*q_z*sinf(theta_d);
	z[18] = 2*q_y*sinf(theta_d) - 2*q_z*cosf(theta_d);
	z[19] = 0;
	z[20] = 0;
	z[21] = 0;
	z[22] = 0;
	z[23] = 0;
	z[24] = 0;
	z[25] = 0;
	z[26] = 0;
	z[27] = 0;
	z[28] = (2*q_w*q_x + 2*q_y*q_z)*cosf(theta_d) - ((q_w)*(q_w) - (q_x)*(q_x) + (q_y)*(q_y) - (q_z)*(q_z))*sinf(theta_d);
	z[29] = 0;

	z[30] = 2*q_w*sinf(theta_d) - 2*q_x*cosf(theta_d);
	z[31] = -2*q_w*cosf(theta_d) - 2*q_x*sinf(theta_d);
	z[32] = -2*q_y*sinf(theta_d) + 2*q_z*cosf(theta_d);
	z[33] = 2*q_y*cosf(theta_d) + 2*q_z*sinf(theta_d);
	z[34] = 0;
	z[35] = 0;
	z[36] = 0;
	z[37] = 0;
	z[38] = 0;
	z[39] = 0;
	z[40] = 0;
	z[41] = 0;
	z[42] = 0;
	z[43] = -(-2*q_w*q_x + 2*q_y*q_z)*sinf(theta_d) + ((q_w)*(q_w) - (q_x)*(q_x) - (q_y)*(q_y) + (q_z)*(q_z))*cosf(theta_d);
	z[44] = 0;
}

static float magnetometer_R_data[3*3] = {
	100, 0, 0,
	0, 100, 0,
	0, 0, 100,
};

ekf_measurement_model_t magnetometer_model = {
	.R.numRows = 3,
	.R.numCols = 3,
	.R.pData = magnetometer_R_data,
	.h = magnetometer_h,
	.dh = magnetometer_dh,
};

static void rangefinder_h(const float *x, float *z) {
	const float p_z = x[9];

	z[0] = p_z;
}

static void rangefinder_dh(const float *x, float *z) {
	z[0] = 0;
	z[1] = 0;
	z[2] = 0;
	z[3] = 0;
	z[4] = 0;
	z[5] = 0;
	z[6] = 0;
	z[7] = 0;
	z[8] = 0;
	z[9] = 1;
	z[10] = 0;
	z[11] = 0;
	z[12] = 0;
	z[13] = 0;
	z[14] = 0;
}

static float rangefinder_R_data[1*1] = {
	10,
};

ekf_measurement_model_t rangefinder_model = {
	.R.numRows = 1,
	.R.numCols = 1,
	.R.pData = rangefinder_R_data,
	.h = rangefinder_h,
	.dh = rangefinder_dh,
};

static void barometer_h(const float *x, float *z) {
	const float p_z = x[9];
	const float p_0 = x[14];

	z[0] = p_0*powf(1 - 1.0F/44330.0F*p_z, 5.255F);
}

static void barometer_dh(const float *x, float *z) {
	const float p_z = x[9];
	const float p_0 = x[14];

	z[0] = 0;
	z[1] = 0;
	z[2] = 0;
	z[3] = 0;
	z[4] = 0;
	z[5] = 0;
	z[6] = 0;
	z[7] = 0;
	z[8] = 0;
	z[9] = -0.000118542748F*p_0*powf(1 - 1.0F/44330.0F*p_z, 4.255F);
	z[10] = 0;
	z[11] = 0;
	z[12] = 0;
	z[13] = 0;
	z[14] = powf(1 - 1.0F/44330.0F*p_z, 5.255F);
}

static float barometer_R_data[1*1] = {
	100,
};

ekf_measurement_model_t barometer_model = {
	.R.numRows = 1,
	.R.numCols = 1,
	.R.pData = barometer_R_data,
	.h = barometer_h,
	.dh = barometer_dh,
};

static void gps_h(const float *x, float *z) {
	const float p_x = x[7];
	const float p_y = x[8];

	z[0] = p_x;
	z[1] = p_y;
}

static void gps_dh(const float *x, float *z) {
	z[0] = 0;
	z[1] = 0;
	z[2] = 0;
	z[3] = 0;
	z[4] = 0;
	z[5] = 0;
	z[6] = 0;
	z[7] = 1;
	z[8] = 0;
	z[9] = 0;
	z[10] = 0;
	z[11] = 0;
	z[12] = 0;
	z[13] = 0;
	z[14] = 0;

	z[15] = 0;
	z[16] = 0;
	z[17] = 0;
	z[18] = 0;
	z[19] = 0;
	z[20] = 0;
	z[21] = 0;
	z[22] = 0;
	z[23] = 1;
	z[24] = 0;
	z[25] = 0;
	z[26] = 0;
	z[27] = 0;
	z[28] = 0;
	z[29] = 0;
}

static float gps_R_data[2*2] = {
	100000, 0,
	0, 100000,
};

ekf_measurement_model_t gps_model = {
	.R.numRows = 2,
	.R.numCols = 2,
	.R.pData = gps_R_data,
	.h = gps_h,
	.dh = gps_dh,
};

static void flow_h(const float *x, float *z) {
	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];
	const float omega_x = x[4];
	const float omega_y = x[5];
	const float p_z = x[9];
	const float v_x = x[10];
	const float v_y = x[11];

	z[0] = -omega_y + v_x*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) + v_y*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[1] = omega_x - v_x*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) + v_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
}

static void flow_dh(const float *x, float *z) {
	const float q_w = x[0];
	const float q_x = x[1];
	const float q_y = x[2];
	const float q_z = x[3];
	const float p_z = x[9];
	const float v_x = x[10];
	const float v_y = x[11];

	z[0] = -2*q_z*v_x*(2*q_w*q_z + 2*q_x*q_y)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) - 2*q_z*v_y*(2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + 2*q_z*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[1] = -2*q_y*v_x*(2*q_w*q_z + 2*q_x*q_y)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) - 2*q_y*v_y*(2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + 2*q_y*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[2] = 2*q_x*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 4*q_y*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) + v_x*(-2*q_x*(2*q_w*q_z + 2*q_x*q_y) + 4*q_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + v_y*(2*q_w*q_z + 2*q_x*q_y)*(-2*q_x*(2*q_w*q_z + 2*q_x*q_y) + 4*q_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[3] = 2*q_w*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 4*q_z*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) + v_x*(-2*q_w*(2*q_w*q_z + 2*q_x*q_y) + 4*q_z*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + v_y*(2*q_w*q_z + 2*q_x*q_y)*(-2*q_w*(2*q_w*q_z + 2*q_x*q_y) + 4*q_z*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[4] = 0;
	z[5] = -1;
	z[6] = 0;
	z[7] = 0;
	z[8] = 0;
	z[9] = -v_x*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/((p_z)*(p_z)*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - v_y*(2*q_w*q_z + 2*q_x*q_y)/((p_z)*(p_z)*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[10] = (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[11] = (2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[12] = 0;
	z[13] = 0;
	z[14] = 0;

	z[15] = 2*q_z*v_x*(2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) - 2*q_z*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 2*q_z*v_y*(2*q_w*q_z + 2*q_x*q_y)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[16] = 2*q_y*v_x*(2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) - 2*q_y*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 2*q_y*v_y*(2*q_w*q_z + 2*q_x*q_y)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[17] = -2*q_x*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 4*q_y*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - v_x*(2*q_w*q_z + 2*q_x*q_y)*(-2*q_x*(2*q_w*q_z + 2*q_x*q_y) + 4*q_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + v_y*(-2*q_x*(2*q_w*q_z + 2*q_x*q_y) + 4*q_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[18] = -2*q_w*v_x/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - 4*q_z*v_y/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - v_x*(2*q_w*q_z + 2*q_x*q_y)*(-2*q_w*(2*q_w*q_z + 2*q_x*q_y) + 4*q_z*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F)) + v_y*(-2*q_w*(2*q_w*q_z + 2*q_x*q_y) + 4*q_z*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1))*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 3.0F/2.0F));
	z[19] = 1;
	z[20] = 0;
	z[21] = 0;
	z[22] = 0;
	z[23] = 0;
	z[24] = v_x*(2*q_w*q_z + 2*q_x*q_y)/((p_z)*(p_z)*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F)) - v_y*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/((p_z)*(p_z)*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[25] = -(2*q_w*q_z + 2*q_x*q_y)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[26] = (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)/(p_z*powf((2*q_w*q_z + 2*q_x*q_y)*(2*q_w*q_z + 2*q_x*q_y) + (-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1)*(-2*(q_y)*(q_y) - 2*(q_z)*(q_z) + 1), 1.0F/2.0F));
	z[27] = 0;
	z[28] = 0;
	z[29] = 0;
}

static float flow_R_data[2*2] = {
	100000, 0,
	0, 100000,
};

ekf_measurement_model_t flow_model = {
	.R.numRows = 2,
	.R.numCols = 2,
	.R.pData = flow_R_data,
	.h = flow_h,
	.dh = flow_dh,
};

EKF_PREDICT(15, 6)
EKF_CORRECT(15, 1)
EKF_CORRECT(15, 2)
EKF_CORRECT(15, 3)
