#ifndef RTOS_STRING_STREAM_HPP
#define RTOS_STRING_STREAM_HPP

#include "rtos/Stream.hpp"

namespace rtos {

class StringStream : public Stream<StringStream> {
public:
    StringStream(char *str, const uint32_t strMaxLen) : Stream{str, strMaxLen} {
    }
};

}

#endif
