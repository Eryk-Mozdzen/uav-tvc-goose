#pragma once

#include <ostream>

enum EscapeCode {
	CYAN = 14,
	GRAY = 7,
	GREEN = 10,
	YELLOW = 11,
	RED = 9,
	MAGENTA = 13
};

std::ostream & operator<<(std::ostream &stream, const EscapeCode &code);
