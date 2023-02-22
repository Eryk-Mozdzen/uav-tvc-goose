#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
	float roll, pitch, yaw, throttle;
} controls_t;

typedef enum {
	TX_LOG,
	TX_TELEMETRY
} tx_msg_t;

extern QueueHandle_t tx_msg_queue;
extern QueueHandle_t rx_msg_queue;

void Communication_Init();

#endif
