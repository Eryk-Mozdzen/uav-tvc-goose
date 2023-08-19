#pragma once

#include <cstdint>

template <uint32_t N>
class MovingAverage {
	float values[N] = {0};
	float sum = 0;
	uint32_t counter = 0;

public:
	MovingAverage();

	void add(const float sample);
	float get() const;
};

template<uint32_t N>
MovingAverage<N>::MovingAverage() {

}

template<uint32_t N>
void MovingAverage<N>::add(const float sample) {
	sum -=values[counter];
	values[counter] = sample;
	sum +=values[counter];

	counter++;
	counter %=N;
}

template<uint32_t N>
float MovingAverage<N>::get() const {
	return sum/N;
}
