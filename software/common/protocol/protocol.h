#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PROTOCOL_INIT {NULL, NULL, NULL, {NULL, 0, 0, 0}, {NULL, 0, 0, 0}, true, 0, 0, NULL, NULL, 0, 0, 0, 0}

typedef void (*protocol_callback_rx_t)(void *, const uint8_t, const void *, const uint32_t);
typedef void (*protocol_callback_tx_t)(void *, const void *, const uint32_t);

typedef struct {
    uint8_t *buffer;
    uint32_t size;
    uint32_t read;
    uint32_t write;
} protocol_fifo_t;

typedef struct {
    void *ctx;
    protocol_callback_tx_t callback_tx;
    protocol_callback_rx_t callback_rx;
    protocol_fifo_t fifo_tx;
    protocol_fifo_t fifo_rx;
    bool available;
    uint32_t time;

    uint32_t time_last;
    uint8_t *decoded;
    uint8_t *cursor;
    uint32_t max;
    uint8_t cobs;
    uint8_t counter;
    uint8_t state;
} protocol_t;

void protocol_enqueue(protocol_t *obj, const uint8_t id, const void *payload, const uint32_t payload_size);
void protocol_process(protocol_t *obj);

#ifdef __cplusplus
}
#endif

#endif
