#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    uint16_t counter;
} protocol_decoder_t;

typedef struct {
    void *payload;
    uint16_t size;
    uint8_t id;
} protocol_message_t;

uint16_t protocol_encode(void *dest, const protocol_message_t *message);
bool protocol_decode(protocol_decoder_t *decoder, const uint8_t byte, protocol_message_t *message);

#ifdef __cplusplus
}
#endif

#endif
