#pragma once

template<class T, unsigned int START, unsigned int LEN, typename S>
class BitField {
		volatile static constexpr T mask = (((1<<LEN)-1)<<START);
		volatile T bits;

	public:
		BitField& operator=(S value) {
			bits &=~mask;
			bits |=(value<<START);
			return *this;
		}

		operator S() const {
			return S((bits & mask)>>START);
		}
};
