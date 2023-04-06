#include "escape_codes.h"

std::ostream & operator<<(std::ostream &stream, const EscapeCode &code) {
	stream << static_cast<char>(27) << "[38;5;" << static_cast<int>(code) << "m";
	return stream;
}
