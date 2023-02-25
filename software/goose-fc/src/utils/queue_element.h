#ifndef QUEUE_ELEMENT_H
#define QUEUE_ELEMENT_H

#include <stdint.h>
#include <string.h>

typedef struct {
	uint8_t type;
	uint8_t data[128];
} queue_element_t;

#endif