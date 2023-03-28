#pragma once

#include <ostream>

enum EscapeCode {
	CYAN = 14,
	GRAY = 7,
	YELLOW = 11,
	RED = 9
};

std::ostream & operator<<(std::ostream &stream, const EscapeCode &code) {
	stream << static_cast<char>(27) << "[38;5;" << static_cast<int>(code) << "m";
	return stream;
}
