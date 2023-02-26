#pragma once

#include "TaskCPP.h"
#include "QueueCPP.h"

#include "queue_element.h"

class Transmitter : public TaskClassS<512> {
		
		struct TX {
			enum {
				LOG,
				TELEMETRY
			};
		};
		
	public:
		enum LOG {
			DEBUG = 7,
			INFO = 14,
			WARNING = 11,
			ERROR = 9
		};
		
		Transmitter();

		void init();

		void task();
		
		static void log(const LOG type, const char *format, ...);
};
