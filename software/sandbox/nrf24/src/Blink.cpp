#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

class Blink : Thread<512> {
    Publisher<messages::Led> publisher;

    void thread() {
        messages::Led message;

        while(true) {
            message.state = true;
            publisher.publish(message);
            delay(50);

            message.state = false;
            publisher.publish(message);
            delay(1950);
        }
    }

public:
    Blink() : Thread{"blink", Thread::Priority::Idle}, publisher{topics::LedControl} {
    }
};

static Blink blink;
