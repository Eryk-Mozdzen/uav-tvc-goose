#ifndef RTOS_STREAM_HPP
#define RTOS_STREAM_HPP

#include <cstdint>
#include <cstring>

namespace rtos {

namespace fmt {

struct Hex {};
struct Dec {};
struct ShowSign {};
struct NoSign {};
struct Left {};
struct Right {};
struct SetW {
    uint32_t value;
};
struct SetPrecision {
    uint32_t value;
};

}

constexpr fmt::Hex hex{};
constexpr fmt::Dec dec{};
constexpr fmt::ShowSign showpos{};
constexpr fmt::NoSign noshowpos{};
constexpr fmt::Left left{};
constexpr fmt::Right right{};

inline fmt::SetW setw(const uint32_t w) {
    return {w};
}

inline fmt::SetPrecision setprecision(const uint32_t p) {
    return {p};
}

template <typename DERIVED>
class Stream {
    enum class Base : uint32_t {
        Dec = 10,
        Hex = 16,
    };

    enum class Align : uint32_t {
        Left,
        Right,
    };

    struct Format {
        Base base = Base::Dec;
        Align align = Align::Right;
        uint32_t width = 0;
        uint32_t precision = 3;
        bool sign = false;
    };

    char *const buffer;
    const uint32_t bufferMaxSize;

    uint32_t index = 0;
    Format format;

    DERIVED &self() {
        return static_cast<DERIVED &>(*this);
    }

    static int writeInt(char *str, const int32_t variable) {
        if(variable == 0) {
            *str = '0';
            return 1;
        }

        char buffer[16];
        int val = variable;
        int i = 0;

        while(val > 0) {
            buffer[i++] = '0' + (val % 10);
            val /= 10;
        }

        const int len = i;

        while(i--) {
            *str = buffer[i];
            str++;
        }

        return len;
    }

public:
    Stream(char *buffer, const uint32_t bufferMaxSize)
        : buffer{buffer}, bufferMaxSize{bufferMaxSize} {
    }

    uint32_t length() const {
        return index;
    }

    void clear() {
        index = 0;
    }

    DERIVED &operator<<(const fmt::Hex &modifier) {
        format.base = Stream::Base::Hex;
        return self();
    }

    DERIVED &operator<<(const fmt::Dec &modifier) {
        format.base = Stream::Base::Dec;
        return self();
    }

    DERIVED &operator<<(const fmt::ShowSign &modifier) {
        format.sign = true;
        return self();
    }

    DERIVED &operator<<(const fmt::NoSign &modifier) {
        format.sign = false;
        return self();
    }

    DERIVED &operator<<(const fmt::Left &modifier) {
        format.align = Stream::Align::Left;
        return self();
    }

    DERIVED &operator<<(const fmt::Right &modifier) {
        format.align = Stream::Align::Right;
        return self();
    }

    DERIVED &operator<<(const fmt::SetW &modifier) {
        format.width = modifier.value;
        return self();
    }

    DERIVED &operator<<(const fmt::SetPrecision &modifier) {
        format.precision = modifier.value;
        return self();
    }

    DERIVED &operator<<(const char variable) {
        const uint32_t available = bufferMaxSize - index;

        if(available > 0) {
            buffer[index] = variable;
            index++;
        }

        return self();
    }

    DERIVED &operator<<(const char *variable) {
        const uint32_t len = strlen(variable);
        const uint32_t available = bufferMaxSize - index;
        const uint32_t write = (available > len) ? len : available;

        memcpy(&buffer[index], variable, write);

        index += write;

        return self();
    }

    DERIVED &operator<<(const bool variable) {
        *this << (variable ? "true" : "false");
        return self();
    }

    DERIVED &operator<<(const int variable) {
        if(format.sign) {
            if(variable >= 0) {
                *this << '+';
            } else {
                *this << '-';
            }
        }

        if(variable > 0) {
            index += writeInt(&buffer[index], variable);
        } else {
            index += writeInt(&buffer[index], -variable);
        }

        return self();
    }

    DERIVED &operator<<(const float variable) {
        if(format.sign) {
            if(variable >= 0.f) {
                *this << '+';
            } else {
                *this << '-';
            }
        }

        float value = variable;

        if(value < 0.f) {
            value = -value;
        }

        const int integer = (int)value;
        const float frac = value - (float)integer;
        const int decimals = (int)(frac * 1000.f + 0.5f);

        index += writeInt(&buffer[index], integer);

        *this << '.';

        if(decimals < 100) {
            *this << '0';
        }

        if(decimals < 10) {
            *this << '0';
        }

        index += writeInt(&buffer[index], decimals);

        return self();
    }

    DERIVED &operator<<(const uint8_t variable) {
        index += writeInt(&buffer[index], variable);
        return self();
    }

    DERIVED &operator<<(const uint32_t variable) {
        index += writeInt(&buffer[index], variable);
        return self();
    }

    DERIVED &operator<<(const int32_t variable) {
        if(variable > 0) {
            *this << '+';
            index += writeInt(&buffer[index], variable);
        } else if(variable < 0) {
            *this << '-';
            index += writeInt(&buffer[index], -variable);
        } else {
            *this << "0";
        }

        return self();
    }
};

}

#endif
