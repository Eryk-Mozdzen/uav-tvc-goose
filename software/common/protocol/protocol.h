#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void (*protocol_callback_rx_t)(const uint8_t, const void *, const uint32_t);
typedef void (*protocol_callback_tx_t)(const void *, const uint32_t);

typedef struct {
    uint8_t *buffer;
    uint32_t size;
    uint32_t read;
    uint32_t write;
} fifo_t;

typedef struct {
    protocol_callback_tx_t callback_tx;
    protocol_callback_rx_t callback_rx;
    fifo_t fifo_tx;
    fifo_t fifo_rx;
    uint32_t last_time;
} protocol_t;

typedef struct {
    uint32_t time;
    bool available;
} protocol_info_t;

void protocol_enqueue(protocol_t *obj, const uint8_t id, const void *payload, const uint32_t payload_size);
void protocol_process(protocol_t *obj, const protocol_info_t *info);

#ifdef __cplusplus
}
#endif

#endif
