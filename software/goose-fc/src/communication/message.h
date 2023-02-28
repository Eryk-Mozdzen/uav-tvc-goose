#pragma once

#include <cstddef>

class Message {
	static constexpr size_t max_len = 64;

	char buffer[max_len + 1];

public:

	Message();
	Message(const char *str);
	Message(const Message &other);

	void operator=(const Message &other);
	
	size_t size() const;

	char * c_str();
};
