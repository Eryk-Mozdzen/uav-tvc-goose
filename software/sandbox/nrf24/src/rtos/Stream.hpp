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
    enum class Base {
        Dec,
        Hex,
    };

    struct Format {
        Base base = Base::Dec;
        uint32_t width = 0;
        uint32_t precision = 3;
        bool sign = false;
    };

    char *const buffer;
    const uint32_t bufferMaxSize;

    uint32_t index;
    Format format;

    void writeUnsignedInteger(uint32_t variable) {
        const uint32_t available = bufferMaxSize - index;

        if(variable == 0) {
            if(available > 0) {
                buffer[index] = '0';
                index++;
            }
            return;
        }

        char tmp[10];
        uint32_t num = 0;

        while(variable > 0) {
            tmp[num] = '0' + (variable % 10);
            num++;
            variable /= 10;
        }

        const uint32_t write = (available > num) ? num : available;

        for(uint32_t i = 0; i < write; i++) {
            buffer[index + i] = tmp[num - i - 1];
        }

        index += write;
    }

    void align(const uint32_t start) {
        const uint32_t taken = index - start;

        if(taken < format.width) {
            for(uint32_t i = 0; i < taken; i++) {
                if((start + format.width - i - 1) < bufferMaxSize) {
                    buffer[start + format.width - i - 1] = buffer[start + taken - i - 1];
                }
            }

            for(uint32_t i = start; i < (start + format.width - taken); i++) {
                buffer[i] = ' ';
            }

            index = ((start + format.width) < bufferMaxSize) ? (start + format.width)
                                                             : bufferMaxSize - 1;
        }
    }

    void terminate() {
        if(index < bufferMaxSize) {
            buffer[index] = '\0';
        } else {
            buffer[bufferMaxSize - 1] = '\0';
        }
    }

public:
    Stream(char *buffer, const uint32_t bufferMaxSize)
        : buffer{buffer}, bufferMaxSize{bufferMaxSize}, index{0} {
        terminate();
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

        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const char *variable) {
        const uint32_t len = strlen(variable);
        const uint32_t available = bufferMaxSize - index;
        const uint32_t write = (available > len) ? len : available;

        memcpy(&buffer[index], variable, write);

        index += write;

        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const bool variable) {
        const uint32_t start = index;
        *this << (variable ? "true" : "false");
        align(start);
        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const float variable) {
        const uint32_t start = index;

        if(std::isnan(variable)) {
            *this << "nan";
            align(start);
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
            align(start);
            return static_cast<DERIVED &>(*this);
        };

        const uint32_t precision = (format.precision > 8) ? 8 : format.precision;

        uint32_t multiplier = 1;
        for(uint32_t i = 0; i < precision; i++) {
            multiplier *= 10;
        }

        uint32_t integer = static_cast<uint32_t>(value);
        const float frac = value - static_cast<float>(integer);
        uint32_t decimals = static_cast<uint32_t>((frac * static_cast<float>(multiplier)) + 0.5f);

        if(decimals >= multiplier) {
            integer += (decimals / multiplier);
            decimals -= (decimals / multiplier) * multiplier;
        }

        writeUnsignedInteger(integer);

        if(precision > 0) {
            *this << '.';

            for(uint32_t i = 0; i < precision; i++) {
                multiplier /= 10;
                if((multiplier >= 10) && (decimals < multiplier)) {
                    *this << '0';
                }
            }

            writeUnsignedInteger(decimals);

            for(uint32_t i = 0; i < (format.precision - precision); i++) {
                *this << '0';
            }
        }

        align(start);
        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const uint8_t variable) {
        const uint32_t start = index;

        switch(format.base) {
            case Base::Dec: {
                writeUnsignedInteger(variable);
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

        align(start);
        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const uint32_t variable) {
        const uint32_t start = index;

        switch(format.base) {
            case Base::Dec: {
                writeUnsignedInteger(variable);
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

        align(start);
        terminate();
        return static_cast<DERIVED &>(*this);
    }

    DERIVED &operator<<(const int32_t variable) {
        const uint32_t start = index;

        switch(format.base) {
            case Base::Dec: {
                if(variable < 0) {
                    *this << "-";
                    writeUnsignedInteger(-variable);
                } else {
                    if(format.sign) {
                        *this << "+";
                    }
                    writeUnsignedInteger(variable);
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

        align(start);
        terminate();
        return static_cast<DERIVED &>(*this);
    }
};

}

#endif
