#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "protocol.h"

#define RX_LIMIT            16
#define TX_SIZE_THRESHOLD   256
#define TX_TIME_THRESHOLD   100

#define MIN(a, b)           ((a)<(b) ? (a) : (b))

enum state {
    STATE_BEGIN,
    STATE_DATA,
};

static inline void fifo_write(protocol_fifo_t *fifo, const uint8_t byte) {
    fifo->buffer[fifo->write] = byte;
    fifo->write++;
    fifo->write %=fifo->size;
}

static inline uint8_t fifo_read(protocol_fifo_t *fifo) {
    const uint8_t byte = fifo->buffer[fifo->read];
    fifo->read++;
    fifo->read %=fifo->size;
    return byte;
}

static inline uint32_t fifo_pending(const protocol_fifo_t *fifo) {
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

static uint8_t * cobs_encode(protocol_fifo_t *fifo, uint8_t *cobs, const void *buffer, const uint32_t size) {
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

void protocol_process(protocol_t *obj) {
    const uint32_t rx_pending = fifo_pending(&obj->fifo_rx);

    for(uint32_t i=0; i<RX_LIMIT && i<rx_pending; i++) {
        const uint8_t byte = fifo_read(&obj->fifo_rx);

        switch(obj->state) {
            case STATE_BEGIN: {
                obj->cursor = obj->decoded;
                obj->cobs = byte;
                obj->counter = 0;
                obj->state = STATE_DATA;
            } break;
            case STATE_DATA: {
                obj->counter++;

                if(!byte) {
                    uint32_t frame_size;
                    uint32_t frame_crc;
                    memcpy(&frame_size, &obj->decoded[4], 4);
                    memcpy(&frame_crc, &obj->decoded[0], 4);

                    const uint32_t num = obj->cursor - obj->decoded;

                    if((num-9)==frame_size) {
                        const uint8_t id = obj->decoded[8];
                        const uint8_t *payload = &obj->decoded[9];
                        const uint32_t size = num - 9;

                        uint32_t crc = 0;
                        crc = crc32(crc, &size, sizeof(size));
                        crc = crc32(crc, &id, sizeof(id));
                        crc = crc32(crc, payload, size);

                        if(frame_crc==crc) {
                            obj->callback_rx(obj->ctx, id, payload, size);
                        }
                    }

                    obj->state = STATE_BEGIN;
                } else if(obj->cobs==obj->counter) {
                    if(obj->cobs!=0xFF) {
                        *obj->cursor = 0;
                        obj->cursor++;
                    }
                    obj->cobs = byte;
                    obj->counter = 0;
                } else {
                    *obj->cursor = byte;
                    obj->cursor++;
                }

            } break;
        }
    }

    const uint32_t delta_time = obj->time - obj->time_last;
    const uint32_t tx_pending = fifo_pending(&obj->fifo_tx);

    if(obj->available && tx_pending && (tx_pending>TX_SIZE_THRESHOLD || delta_time>TX_TIME_THRESHOLD)) {
        const uint32_t len = MIN(tx_pending, obj->fifo_tx.size - obj->fifo_tx.read);

        obj->callback_tx(obj->ctx, &obj->fifo_tx.buffer[obj->fifo_tx.read], len);
        obj->fifo_tx.read +=len;
        obj->fifo_tx.read %=obj->fifo_tx.size;

        obj->time_last = obj->time;
    }
}
