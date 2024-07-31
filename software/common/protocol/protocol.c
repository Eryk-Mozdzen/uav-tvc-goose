#include <stddef.h>
#include <stdint.h>

#include "protocol.h"

#define DECODE_BUFFER_SIZE  1024

#define RX_LIMIT            16
#define TX_SIZE_THRESHOLD   256
#define TX_TIME_THRESHOLD   100

#define MIN(a, b)           ((a)<(b) ? (a) : (b))

typedef struct {
    uint8_t buffer[DECODE_BUFFER_SIZE];
    uint32_t cobs;
    uint32_t counter;
    uint32_t cursor;
} decoder_t;

static decoder_t decoder = {0};

static inline void fifo_write(fifo_t *fifo, const uint8_t byte) {
    fifo->buffer[fifo->write] = byte;
    fifo->write++;
    fifo->write %=fifo->size;
}

static inline uint8_t fifo_read(fifo_t *fifo) {
    const uint8_t byte = fifo->buffer[fifo->read];
    fifo->read++;
    fifo->read %=fifo->size;
    return byte;
}

static inline uint32_t fifo_pending(const fifo_t *fifo) {
    return ((fifo->read>fifo->write ? fifo->size : 0) + fifo->write) - fifo->read;
}

static uint32_t crc32(uint32_t crc, const void *buffer, const uint32_t size) {
    crc ^=0xFFFFFFFF;

    for(uint32_t i=0; i<size; i++) {
        crc ^=((uint8_t *)buffer)[i];
        for(uint8_t j=0; j<8; j++) {
            if(crc & 0x00000001) {
                crc = (crc>>1)^0xEDB88320;
            } else {
                crc >>=1;
            }
        }
    }

    crc ^=0xFFFFFFFF;

    return crc;
}

static uint8_t * cobs_encode(fifo_t *fifo, uint8_t *cobs, const void *buffer, const uint32_t size) {
    for(uint32_t i=0; i<size; i++) {
        const uint8_t byte = ((uint8_t *)buffer)[i];

        if(*cobs==0xFF) {
            cobs = &fifo->buffer[fifo->write];
            fifo_write(fifo, 1);
        }

        if(byte) {
            (*cobs)++;
            fifo_write(fifo, byte);
        } else {
            cobs = &fifo->buffer[fifo->write];
            fifo_write(fifo, 1);
        }
    }

    return cobs;
}

void protocol_enqueue(protocol_t *obj, const uint8_t id, const void *payload, const uint32_t size) {
    uint32_t crc = 0;
    crc = crc32(crc, &size, sizeof(size));
    crc = crc32(crc, &id, sizeof(id));
    crc = crc32(crc, payload, size);

    uint8_t *cobs = &obj->fifo_tx.buffer[obj->fifo_tx.write];
    fifo_write(&obj->fifo_tx, 1);

    cobs = cobs_encode(&obj->fifo_tx, cobs, &crc, sizeof(crc));
    cobs = cobs_encode(&obj->fifo_tx, cobs, &size, sizeof(size));
    cobs = cobs_encode(&obj->fifo_tx, cobs, &id, sizeof(id));
    cobs = cobs_encode(&obj->fifo_tx, cobs, payload, size);

    fifo_write(&obj->fifo_tx, 0);
}

void protocol_process(protocol_t *obj, const protocol_info_t *info) {
    const uint32_t rx_pending = fifo_pending(&obj->fifo_rx);

    for(uint32_t i=0; i<RX_LIMIT && i<rx_pending; i++) {
        const uint8_t byte = fifo_read(&obj->fifo_rx);

        /*if(!decoder.cursor) {
            decoder.cobs = byte;
            decoder.counter = 0;
            continue;
        }

        if(decoder.counter==decoder.cobs) {
            if(decoder.cobs==0xFF && byte) {
                decoder.cobs = byte;
                continue;
            }
        }

        if(decoder.cobs==)
        decoder.buffer[decoder.counter] = byte;
        decoder.counter++;
        decoder.cobs++;

        if(byte || (decode_counter>=DECODE_BUFFER_SIZE)) {
            decode_counter = 0;
            continue;
        }

        uint32_t index = decode_buffer[0] - 1;

        while(index<decode_counter - 1) {
            const uint8_t next = decode_buffer[index];
            decode_buffer[index] = 0;
            index +=next;
        }

        if(decode_buffer[2]!=(decoder->counter - 5)) {
            decoder->counter = 0;
            continue;
        }

        if(decoder->buffer[1]!=crc8(&decoder->buffer[2], decoder->counter - 2)) {
            decoder->counter = 0;
            continue;
        }

        decoder->counter = 0;

        const void *payload = &decode_buffer[10];
        const uint32_t size = (((uint32_t)decode_buffer[4])<<24) | (((uint32_t)decode_buffer[3])<<16) | (((uint32_t)decode_buffer[2])<<8) | decode_buffer[1];
        const uint8_t id = decode_buffer[9];

        obj->callback_rx(id, payload, size);*/
    }

    const uint32_t delta_time = info->time - obj->last_time;
    const uint32_t tx_pending = fifo_pending(&obj->fifo_tx);

    if(info->available && tx_pending && (tx_pending>TX_SIZE_THRESHOLD || delta_time>TX_TIME_THRESHOLD)) {
        const uint32_t len = MIN(tx_pending, obj->fifo_tx.size - obj->fifo_tx.read);

        obj->callback_tx(&obj->fifo_tx.buffer[obj->fifo_tx.read], len);
        obj->fifo_tx.read +=len;
        obj->fifo_tx.read %=obj->fifo_tx.size;

        obj->last_time = info->time;
    }
}
