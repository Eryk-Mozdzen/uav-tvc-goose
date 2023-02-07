#pragma once

template<class T, unsigned int START, unsigned int LEN, typename S>
class BitFieldWriteOnly {
		static constexpr T mask = (((1<<LEN)-1)<<START);
		volatile T bits;

	public:
		BitFieldWriteOnly& operator=(S value) {
			bits &=~mask;
			bits |=(value<<START);
			return *this;
		}
};