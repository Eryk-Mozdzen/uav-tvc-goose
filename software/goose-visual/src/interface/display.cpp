#include "display.h"
#include <iostream>
#include <iomanip>
#include "escape_codes.h"

Display::Display(ConcurrentQueue<CommunicationBase::Log> &logs, ConcurrentQueue<CommunicationBase::Telemetry> &telemetry) :
	logs{logs}, telemetry{telemetry}, thread_kill{false}, logger{&Display::readLog, this}, viewer{&Display::readTelemetry, this} {
}

Display::~Display() {
	thread_kill = true;

	logger.join();
	viewer.join();
}

void Display::readLog() {

	while(!thread_kill) {
		CommunicationBase::Log log;
		if(!logs.pop(log))
			continue;

		switch(log.first) {
			case Transfer::LOG_DEBUG:	std::cout << EscapeCode::GRAY;		break;
			case Transfer::LOG_INFO:	std::cout << EscapeCode::CYAN;		break;
			case Transfer::LOG_WARNING:	std::cout << EscapeCode::YELLOW;	break;
			case Transfer::LOG_ERROR:	std::cout << EscapeCode::RED;		break;
			default: break;
		}

		std::cout << log.first << ":\t" << log.second << std::endl;
	}
}

void Display::readTelemetry() {

	while(!thread_kill) {
		CommunicationBase::Telemetry data;
		if(!telemetry.pop(data))
			continue;

		if(data.id==Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE) {
			struct Quaternion {float w, i, j, k;} q;
			data.getPayload(q);

			const std::ios_base::fmtflags default_flags = std::cout.flags();

			std::cout << EscapeCode::MAGENTA << std::showpos << std::fixed << std::setprecision(5);
			std::cout << q.w << " " << q.i << " " << q.j << " " << q.k << std::endl;

			std::cout.flags(default_flags);

			continue;
		}

		std::cout << static_cast<int>(data.id) << ":\t";
		for(size_t i=0; i<data.length; i++)
			std::cout << static_cast<int>(data.payload[i]) << "\t";
		std::cout << std::endl;
	}
}
