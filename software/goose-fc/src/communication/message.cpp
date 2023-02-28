#include "message.h"
#include <cstring>

Message::Message() {
	memset(buffer, 0, max_len + 1);
}

Message::Message(const char *str) {
	const size_t str_len = strlen(str);
	const size_t len = str_len<max_len ? str_len : max_len;
	
	memcpy(buffer, str, len);
	buffer[len] = '\0';
}

Message::Message(const Message &other) {
	const size_t str_len = strlen(other.buffer);
	const size_t len = str_len<max_len ? str_len : max_len;

	memcpy(buffer, other.buffer, len);
	buffer[len] = '\0';
}

void Message::operator=(const Message &other) {
	const size_t str_len = strlen(other.buffer);
	const size_t len = str_len<max_len ? str_len : max_len;

	memcpy(buffer, other.buffer, len);
	buffer[len] = '\0';
}

size_t Message::size() const {
	return strlen(buffer);
}

char * Message::c_str() {
	return buffer;
}
