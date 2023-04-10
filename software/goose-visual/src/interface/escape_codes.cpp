#include "escape_codes.h"

std::ostream & operator<<(std::ostream &stream, const EscapeCode &code) {
	constexpr char escape = 27;

	stream << escape << "[38;5;" << static_cast<int>(code) << "m";

	return stream;
}
