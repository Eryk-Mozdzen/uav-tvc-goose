#pragma once

#include "TimerCPP.h"
#include "logger.h"

template<typename T>
class IntervalLogger {
    const Transfer::ID id;
	TimerMember<IntervalLogger<T>> timer;
    T data;
    bool data_ready;

	void send();

public:
    IntervalLogger(const char *name, const Transfer::ID id);

    void feed(const T &data);
};

template<typename T>
IntervalLogger<T>::IntervalLogger(const char *name, const Transfer::ID id) :
        id{id},
        timer{name, this, &IntervalLogger<T>::send, 50, pdTRUE},
        data_ready{false} {

    timer.start();
}

template<typename T>
void IntervalLogger<T>::send() {
    if(data_ready) {
        Logger::getInstance().send(id, data);
        data_ready = false;
    }
}

template<typename T>
void IntervalLogger<T>::feed(const T &data) {
    this->data = data;
    data_ready = true;
}
