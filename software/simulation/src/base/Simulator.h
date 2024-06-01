#pragma once

#include <atomic>
#include <thread>
#include <drake/systems/analysis/simulator.h>

template<typename T>
class Simulator : public drake::systems::Simulator<T> {
    std::atomic<bool> running = true;
	std::thread thread;

	void run();

public:
	Simulator(const drake::systems::System<T> &system);
	~Simulator();

	void StartAdvance();
};

template<typename T>
Simulator<T>::Simulator(const drake::systems::System<T> &system) : drake::systems::Simulator<T>{system} {

}

template<typename T>
Simulator<T>::~Simulator() {
    running = false;

    if(thread.joinable()) {
        thread.join();
    }
}

template<typename T>
void Simulator<T>::StartAdvance() {
    thread = std::thread(&Simulator<T>::run, this);
}

template<typename T>
void Simulator<T>::run() {
    while(running) {
        this->AdvanceTo(this->get_context().get_time() + 1);
    }
}
