#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nmea.h"

#define START               '$'
#define FIELD_DELIMITER     ','
#define CHECKSUM_DELIMITER  '*'
#define CR                  '\r'
#define LF                  '\n'

enum state {
    STATE_START,
    STATE_FIELDS,
    STATE_TAIL,
};

bool nmea_consume(nmea_messaage_t *message, const char byte) {
    if(byte==START) {
        memset(message, 0, sizeof(*message));
        message->context.state = STATE_FIELDS;
        return false;
    }

    if(message->context.state==STATE_FIELDS) {
        if(byte==FIELD_DELIMITER) {
            message->context.checksum ^=byte;
            message->context.field_counter++;
            message->context.field_counter %=NMEA_MAX_FIELD_NUM;
            message->context.char_counter = 0;
        } else if(byte==CHECKSUM_DELIMITER) {
            message->context.field_counter++;
            message->argc = message->context.field_counter;
            message->context.state = STATE_TAIL;
            message->context.field_counter = 0;
            message->context.char_counter = 0;
        } else {
            message->context.checksum ^=byte;
            message->argv[message->context.field_counter][message->context.char_counter] = byte;
            message->context.char_counter++;
            message->context.char_counter %=NMEA_MAX_FIELD_SIZE;
        }

        return false;
    }

    if(message->context.state==STATE_TAIL) {
        message->context.tail[message->context.char_counter] = byte;
        message->context.char_counter++;

        if(message->context.char_counter==4) {
            const uint8_t nibble1 = (message->context.checksum & 0xF0)>>4;
            const uint8_t nibble2 = (message->context.checksum & 0x0F)>>0;
            const char cs1 = (nibble1<=9) ? ('0' + nibble1) : ('A' + nibble1 - 10);
            const char cs2 = (nibble2<=9) ? ('0' + nibble2) : ('A' + nibble2 - 10);

            if(message->context.tail[0]==cs1 && message->context.tail[1]==cs2 && message->context.tail[2]==CR && message->context.tail[3]==LF) {
                message->context.state = STATE_START;
                message->context.char_counter = 0;
                return true;
            }
        }
    }

    return false;
}
