#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#define COM		Communication::getInstance()

class Communication {
		QueueHandle_t tx_queue;
		QueueHandle_t rx_queue;
		
		struct TX {
			enum {
				LOG,
				TELEMETRY
			};
		};
		
		Communication();

	public:
		enum LOG {
			DEBUG = 7,
			INFO = 14,
			WARNING = 11,
			ERROR = 9
		};

		Communication(Communication &) = delete;
		void operator=(const Communication &) = delete;

		void run();
		void log(const LOG type, const char *format, ...);

		static Communication& getInstance();
};

void transmitterTaskFcn(void *param);
