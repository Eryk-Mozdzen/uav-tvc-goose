#ifndef RTOS_STREAM_HPP
#define RTOS_STREAM_HPP

#include <cmath>
#include <cstdint>
#include <cstring>

namespace rtos {

namespace fmt {

struct Hex {};
struct Dec {};
struct ShowSign {};
struct NoSign {};
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

    struct Format {
        Base base = Base::Dec;
        uint32_t width = 0;
        uint32_t precision = 3;
        bool sign = false;
    };

    char *const buffer;
    const uint32_t bufferMaxSize;

    uint32_t index = 0;
    Format format;

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
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const fmt::Dec &modifier) {
        format.base = Stream::Base::Dec;
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const fmt::ShowSign &modifier) {
        format.sign = true;
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const fmt::NoSign &modifier) {
        format.sign = false;
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const fmt::SetW &modifier) {
        format.width = modifier.value;
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const fmt::SetPrecision &modifier) {
        format.precision = modifier.value;
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const char variable) {
        const uint32_t available = bufferMaxSize - index;

        if(available > 0) {
            buffer[index] = variable;
            index++;
        }

        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const char *variable) {
        const uint32_t len = strlen(variable);
        const uint32_t available = bufferMaxSize - index;
        const uint32_t write = (available > len) ? len : available;

        memcpy(&buffer[index], variable, write);

        index += write;

        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const bool variable) {
        *this << (variable ? "true" : "false");
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const float variable) {
        if(std::isnan(variable)) {
            *this << "nan";
            return static_cast<DERIVED &>(*this);
        }

        float value = variable;

        if(variable < 0.f) {
            *this << '-';
            value = -value;
        } else if(format.sign) {
            *this << '+';
        }

        if(std::isinf(value)) {
            *this << "inf";
            return static_cast<DERIVED &>(*this);
        };

        int multiplier = 1;
        for(uint32_t i = 0; i < format.precision; i++) {
            multiplier *= 10;
        }

        int integer = static_cast<int>(value);
        float frac = value - static_cast<float>(integer);
        int decimals = static_cast<int>((frac * static_cast<float>(multiplier)) + 0.5f);

        if(decimals >= multiplier) {
            integer += (decimals / multiplier);
            decimals -= (decimals / multiplier) * multiplier;
        }

        index += writeInt(&buffer[index], integer);

        *this << '.';

        for(uint32_t i = 0; i < format.precision; i++) {
            multiplier /= 10;
            if((multiplier >= 10) && (decimals < multiplier)) {
                *this << '0';
            }
        }

        index += writeInt(&buffer[index], decimals);

        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const uint8_t variable) {
        switch(format.base) {
            case Base::Dec: {
                index += writeInt(&buffer[index], variable);
            } break;
            case Base::Hex: {
                *this << "0x";

                const uint32_t nibble1 = (variable >> 4) & 0x0F;
                const uint32_t nibble0 = (variable >> 0) & 0x0F;

                if(nibble1 < 10) {
                    *this << static_cast<char>('0' + nibble1);
                } else {
                    *this << static_cast<char>('A' + nibble1 - 10);
                }

                if(nibble0 < 10) {
                    *this << static_cast<char>('0' + nibble0);
                } else {
                    *this << static_cast<char>('A' + nibble0 - 10);
                }
            } break;
        }

        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const uint32_t variable) {
        switch(format.base) {
            case Base::Dec: {
                index += writeInt(&buffer[index], variable);
            } break;
            case Base::Hex: {
                *this << "0x";

                for(uint32_t i = 0; i < 8; i++) {
                    const uint32_t nibble = (variable >> (4 * (7 - i))) & 0x0F;

                    if(nibble < 10) {
                        *this << static_cast<char>('0' + nibble);
                    } else {
                        *this << static_cast<char>('A' + nibble - 10);
                    }
                }

            } break;
        }

        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const int32_t variable) {
        switch(format.base) {
            case Base::Dec: {
                if(format.sign) {
                    if(variable > 0) {
                        *this << '+';
                        index += writeInt(&buffer[index], variable);
                    } else {
                        *this << '-';
                        index += writeInt(&buffer[index], -variable);
                    }
                }
            } break;
            case Base::Hex: {
                *this << "0x";

                for(uint32_t i = 0; i < 8; i++) {
                    const uint32_t nibble = (variable >> (4 * (7 - i))) & 0x0F;

                    if(nibble < 10) {
                        *this << static_cast<char>('0' + nibble);
                    } else {
                        *this << static_cast<char>('A' + nibble - 10);
                    }
                }

            } break;
        }

        return static_cast<DERIVED &>(*this);
    }
};

}

#endif
