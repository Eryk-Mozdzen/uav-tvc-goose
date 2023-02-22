#include "matrix.h"
#include "lqr.h"

int main() {

	constexpr float M = 0.5; 	// kg     	mass of the cart
	constexpr float m = 0.2; 	// kg     	mass of the pendulum
	constexpr float L = 100; 	// m      	length to pendulum center of mass
	constexpr float I = 0;   	// kg*m^2 	inertia of the pendulum
	constexpr float g = 9.81;	// m/s^2  	gravity acceleration
	constexpr float d = 0.1; 	// N/(m*s)	friction of the cart

	constexpr float p = I*(M+m)+M*m*L*L;

	constexpr Matrix<4, 4> A = {
		0,	1,				0, 				0,
		0,	-(I+m*L*L)*d/p,	m*m*g*L*L/p,	0,
		0,	0,				0,				1,
		0,	-m*L*d/p,		m*g*L*(M+m)/p,	0
	};

	constexpr Matrix<4, 1> B = {
		0,
		(I+m*L*L)/p,
		0,
		m*L/p
	};

	constexpr Matrix<4, 4> Q = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	constexpr Matrix<1, 1> R = {
		1000
	};

	constexpr Matrix<1, 4> K = LQR(A, B, Q, R);

	std::cout << K << std::endl;
}
