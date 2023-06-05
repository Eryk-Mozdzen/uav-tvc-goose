#pragma once

#include <ostream>

enum EscapeCode {
	GRAY = 7,
	RED = 9,
	GREEN = 10,
	YELLOW = 11,
	BLUE = 12,
	MAGENTA = 13,
	CYAN = 14
};

std::ostream & operator<<(std::ostream &stream, const EscapeCode &code);
