#include "printer.h"
#include <iostream>
#include <iomanip>
#include "escape_codes.h"

Printer::Printer(Comm &commuication) : commuication{commuication},
	thread_kill{false}, logger{&Printer::readLog, this}, viewer{&Printer::readTelemetry, this} {
}

Printer::~Printer() {
	thread_kill = true;

	logger.join();
	viewer.join();
}

void Printer::readLog() {

	while(!thread_kill) {
		Comm::Log log;
		if(!commuication.logs.pop(log))
			continue;

		mutex.lock();
		switch(log.first) {
			case Transfer::LOG_DEBUG:	std::cout << EscapeCode::GRAY;		break;
			case Transfer::LOG_INFO:	std::cout << EscapeCode::CYAN;		break;
			case Transfer::LOG_WARNING:	std::cout << EscapeCode::YELLOW;	break;
			case Transfer::LOG_ERROR:	std::cout << EscapeCode::RED;		break;
			default: break;
		}

		std::cout << log.first << "\t" << log.second << std::endl;
		mutex.unlock();
	}
}

void Printer::readTelemetry() {

	while(!thread_kill) {
		Comm::Telemetry data;
		if(!commuication.telemetry.pop(data))
			continue;

		if(data.id==Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE) {
			struct Quaternion {float w, i, j, k;} q;
			data.getPayload(q);

			mutex.lock();
			const std::ios_base::fmtflags default_flags = std::cout.flags();
			std::cout << EscapeCode::MAGENTA << std::showpos << std::fixed << std::setprecision(5);
			std::cout << q.w << " " << q.i << " " << q.j << " " << q.k << std::endl;
			std::cout.flags(default_flags);
			mutex.unlock();

			continue;
		}

		mutex.lock();
		std::cout << EscapeCode::MAGENTA << data.id << "\t";
		for(size_t i=0; i<data.length; i++)
			std::cout << static_cast<int>(data.payload[i]) << "\t";
		std::cout << std::endl;
		mutex.unlock();
	}
}
