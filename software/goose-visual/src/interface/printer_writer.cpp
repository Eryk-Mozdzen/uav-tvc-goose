#include "printer_writer.h"
#include <iostream>
#include <iomanip>
#include "escape_codes.h"

PrinterWriter::PrinterWriter(QObject *parent) : QObject{parent} {

	connect(&timer, &QTimer::timeout, this, &PrinterWriter::sendSomething);

	timer.setInterval(1000);
	timer.start();
}

void PrinterWriter::sendSomething() {
	onTransmit(Transfer::encode(21.37f, Transfer::ID::CONTROL_MOTOR_THROTTLE));
}

void PrinterWriter::onReceive(const Transfer::FrameRX &frame) {

	if(frame.id<=Transfer::ID::LOG_ERROR) {
		switch(frame.id) {
			case Transfer::ID::LOG_DEBUG:	std::cout << EscapeCode::GRAY;		break;
			case Transfer::ID::LOG_INFO:	std::cout << EscapeCode::CYAN;		break;
			case Transfer::ID::LOG_WARNING:	std::cout << EscapeCode::YELLOW;	break;
			case Transfer::ID::LOG_ERROR:	std::cout << EscapeCode::RED;		break;
			default: break;
		}

		std::cout << std::string(reinterpret_cast<const char *>(frame.payload), frame.length) << std::endl;

		return;
	}

	/*if(frame.id==Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE) {
		struct Quaternion {float w, i, j, k;} q;
		frame.getPayload(q);

		const std::ios_base::fmtflags default_flags = std::cout.flags();
		std::cout << EscapeCode::MAGENTA << std::showpos << std::fixed << std::setprecision(5);
		std::cout << q.w << " " << q.i << " " << q.j << " " << q.k << std::endl;
		std::cout.flags(default_flags);

		return;
	}*/

	if(frame.id==Transfer::ID::TELEMETRY_SENSOR_DISTANCE) {
		float dist;
		frame.getPayload(dist);

		const std::ios_base::fmtflags default_flags = std::cout.flags();
		std::cout << EscapeCode::GREEN << std::fixed << std::setprecision(3);
		std::cout << dist << std::endl;
		std::cout.flags(default_flags);

		return;
	}

	/*std::cout << EscapeCode::MAGENTA << frame.id << "\t";
	for(size_t i=0; i<frame.length; i++)
		std::cout << static_cast<int>(frame.payload[i]) << "\t";
	std::cout << std::endl;*/
}
