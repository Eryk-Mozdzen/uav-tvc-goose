#ifndef QUEUE_ELEMENT_H
#define QUEUE_ELEMENT_H

#include <stdint.h>

typedef struct {
	uint8_t type;
	void *data;
} queue_element_t;

#endif
