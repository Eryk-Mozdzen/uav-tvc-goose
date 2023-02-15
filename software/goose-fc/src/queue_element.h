#ifndef QUEUE_ELEMENT_H
#define QUEUE_ELEMENT_H

#include <stdint.h>

#define QUEUE_ELEMENT_SIZE		3*sizeof(float)

typedef struct {
	uint8_t type;
	uint8_t data[QUEUE_ELEMENT_SIZE];
} queue_element_t;

#endif
