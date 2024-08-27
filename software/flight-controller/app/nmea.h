#ifndef NMEA_H
#define NMEA_H

#include <stdint.h>
#include <stdbool.h>

#define NMEA_MAX_FIELD_NUM     24
#define NMEA_MAX_FIELD_SIZE    16

struct nmea_context {
    char tail[4];
    uint8_t state;
    uint8_t checksum;
    uint8_t field_counter;
    uint8_t char_counter;
};

typedef struct {
    uint8_t argc;
    char argv[NMEA_MAX_FIELD_NUM][NMEA_MAX_FIELD_SIZE];
    struct nmea_context context;
} nmea_messaage_t;

bool nmea_consume(nmea_messaage_t *message, const char byte);

#endif
