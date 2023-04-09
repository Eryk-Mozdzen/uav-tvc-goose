#include "writer.h"
#include <chrono>
#include <iostream>

Writer::Writer(Comm &communication) : communication{communication}, thread_kill{false}, thread{&Writer::write, this} {

}

Writer::~Writer() {
	thread_kill = true;
	thread.join();
}

void Writer::write() {

	while(!thread_kill) {

		const Transfer::FrameTX frame = Transfer::encode(21.37f, Transfer::ID::TELEMETRY_INPUTS_MOTOR);

		communication.controls.push(frame);
		//std::cout << "send" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}