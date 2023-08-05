#pragma once

struct Sample {
	float x, y, z;
};

struct Params {
	float offset[3];
	float scale[3];

	Params() {
		offset[0] = 0.f;
		offset[1] = 0.f;
		offset[2] = 0.f;
		scale[0] = 1.f;
		scale[1] = 1.f;
		scale[2] = 1.f;
	}
};
