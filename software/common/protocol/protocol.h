#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PROTOCOL_INIT {NULL, NULL, NULL, NULL, NULL, {NULL, 0, 0, 0}, {NULL, 0, 0, 0}, true, 0, NULL, NULL, 0, 0, 0, 0, 0}

typedef enum {
    PROTOCOL_ERROR_DOUBLE_ZERO,
    PROTOCOL_ERROR_CRC_MISMATCH,
    PROTOCOL_ERROR_DECODER_OVERFLOW,
} protocol_error_t;

typedef void (*protocol_tx_cb_t)(void *, const void *, const uint32_t);
typedef void (*protocol_rx_cb_t)(void *, const uint8_t, const uint32_t, const void *, const uint32_t);
typedef void (*protocol_err_cb_t)(void *, const protocol_error_t);
typedef uint32_t (*protocol_time_cb_t)(void *);

typedef struct {
    uint8_t *buffer;
    uint32_t size;
    uint32_t read;
    uint32_t write;
} protocol_fifo_t;

typedef struct {
    void *user;
    protocol_tx_cb_t callback_tx;
    protocol_rx_cb_t callback_rx;
    protocol_err_cb_t callback_err;
    protocol_time_cb_t callback_time;
    protocol_fifo_t fifo_tx;
    protocol_fifo_t fifo_rx;
    bool available;

    uint32_t time_last;
    uint8_t *decoded;
    uint8_t *cursor;
    uint32_t max;
    uint8_t cobs;
    uint8_t counter;
    uint8_t state;
    uint32_t crc;
} protocol_t;

void protocol_enqueue(protocol_t *instance, const uint8_t id, const void *payload, const uint32_t size);
void protocol_process(protocol_t *instance);

#ifdef __cplusplus
}
#endif

#endif
