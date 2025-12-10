#include "rtos/Publisher.hpp"
#include "rtos/Thread.hpp"
#include "topic/Topics.hpp"

using namespace rtos;

class Blink : Thread<1024> {
    Publisher<topic::message::Led> publisher;

    void thread() {
        topic::message ::Led message;

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
    Blink() : Thread{"blink", Thread::Priority::Idle}, publisher{topic::LedControl} {
    }
};

static Blink blink;
