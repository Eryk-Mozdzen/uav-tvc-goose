#pragma once

template<class T, unsigned int START, unsigned int LEN, typename S>
class BitFieldReadOnly {
		static constexpr T mask = (((1<<LEN)-1)<<START);
		volatile T bits;

	public:
		operator S() const {
			return S((bits & mask)>>START);
		}
};