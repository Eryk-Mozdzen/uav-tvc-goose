#include <stm32u0xx_hal.h>

#include "rtos/Log.hpp"
#include "rtos/Publisher.hpp"
#include "rtos/Subscriber.hpp"
#include "rtos/Thread.hpp"
#include "rtos/Topics.hpp"

using namespace rtos;

class Output {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;

public:
    enum class State {
        LOW = GPIO_PIN_RESET,
        HIGH = GPIO_PIN_SET,
    };

    Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) : GPIOx{GPIOx}, GPIO_Pin{GPIO_Pin} {
    }

    inline void set(const State state) {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, static_cast<GPIO_PinState>(state));
    }
};

class nRF24L01p : Thread<512> {
    enum Register : uint8_t {
        CONFIG = 0x00,
        EN_AA = 0x01,
        EN_RXADDR = 0x02,
        SETUP_AW = 0x03,
        SETUP_RETR = 0x04,
        RF_CH = 0x05,
        RF_SETUP = 0x06,
        STATUS = 0x07,
        OBSERVE_TX = 0x08,
        RPD = 0x09,
        RX_ADDR_P0 = 0x0A,
        RX_ADDR_P1 = 0x0B,
        RX_ADDR_P2 = 0x0C,
        RX_ADDR_P3 = 0x0D,
        RX_ADDR_P4 = 0x0E,
        RX_ADDR_P5 = 0x0F,
        TX_ADDR = 0x10,
        RX_PW_P0 = 0x11,
        RX_PW_P1 = 0x12,
        RX_PW_P2 = 0x13,
        RX_PW_P3 = 0x14,
        RX_PW_P4 = 0x15,
        RX_PW_P5 = 0x16,
        FIFO_STATUS = 0x17,
        DYNPD = 0x1C,
        FEATURE = 0x1D,
    };

    struct SPICommand {
        enum Word : uint8_t {
            R_RX_PAYLOAD = 0x61,
            W_TX_PAYLOAD = 0xA0,
            FLUSH_TX = 0xE1,
            FLUSH_RX = 0xE2,
            REUSE_TX_PL = 0xE3,
            R_RX_PL_WID = 0x60,
            W_TX_PAYLOAD_NO_ACK = 0xB0,
            NOP = 0xFF,
        };

        static constexpr Word R_REGISTER(const uint8_t reg) {
            return static_cast<Word>(0x00 | (reg & 0x1F));
        }

        static constexpr Word W_REGISTER(const uint8_t reg) {
            return static_cast<Word>(0x20 | (reg & 0x1F));
        }

        static constexpr Word W_ACK_PAYLOAD(const uint8_t pipe) {
            return static_cast<Word>(0xA8 | (pipe & 0x07));
        }
    };

    SPI_HandleTypeDef &hspi;
    Output ce;
    Output csn;
    const bool transmitter;

    uint8_t readReg(const Register reg) {
        const uint8_t txData[2] = {
            SPICommand::R_REGISTER(reg),
            0,
        };
        uint8_t rxData[2] = {0};

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_TransmitReceive(&hspi, txData, rxData, 2, 10), HAL_OK);
        csn.set(Output::State::HIGH);

        return rxData[1];
    }

    void readReg(const Register reg, uint8_t *values, const uint32_t num) {
        const uint32_t len = (num <= 32) ? num : 32;

        const uint8_t txData[33] = {SPICommand::R_REGISTER(reg)};
        uint8_t rxData[33] = {0};

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_TransmitReceive(&hspi, txData, rxData, len + 1, 10), HAL_OK);
        csn.set(Output::State::HIGH);

        memcpy(values, &rxData[1], len);
    }

    void writeReg(const Register reg, const uint8_t value) {
        const uint8_t txData[2] = {
            SPICommand::W_REGISTER(reg),
            value,
        };

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_Transmit(&hspi, txData, 2, 10), HAL_OK);
        csn.set(Output::State::HIGH);
    }

    void writeReg(const Register reg,
                  const uint8_t value1,
                  const uint8_t value2,
                  const uint8_t value3,
                  const uint8_t value4,
                  const uint8_t value5) {
        const uint8_t txData[6] = {
            SPICommand::W_REGISTER(reg), value1, value2, value3, value4, value5,
        };

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_Transmit(&hspi, txData, 6, 10), HAL_OK);
        csn.set(Output::State::HIGH);
    }

    uint8_t readRxPayload(void *data) {
        const uint8_t txData[33] = {SPICommand::R_RX_PAYLOAD};
        uint8_t rxData[33] = {0};

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_TransmitReceive(&hspi, txData, rxData, 33, 10), HAL_OK);
        csn.set(Output::State::HIGH);

        memcpy(data, &rxData[1], 32);
        return rxData[0];
    }

    uint8_t writeTxPayload(const void *data, const uint32_t num) {
        const uint32_t len = (num <= 32) ? num : 32;

        uint8_t txData[33] = {SPICommand::W_TX_PAYLOAD};
        uint8_t rxData[33] = {0};

        memcpy(&txData[1], data, len);

        csn.set(Output::State::LOW);
        RTOS_ASSERT(HAL_SPI_TransmitReceive(&hspi, txData, rxData, len + 1, 10), HAL_OK);
        csn.set(Output::State::HIGH);

        return rxData[0];
    }

    void flushRxFifo() {
        const uint8_t txData[1] = {SPICommand::FLUSH_RX};

        csn.set(Output::State::LOW);
        HAL_SPI_Transmit(&hspi, txData, 1, HAL_MAX_DELAY);
        csn.set(Output::State::HIGH);
    }

    void flushTxFifo() {
        const uint8_t txData[1] = {SPICommand::FLUSH_TX};

        csn.set(Output::State::LOW);
        HAL_SPI_Transmit(&hspi, txData, 1, HAL_MAX_DELAY);
        csn.set(Output::State::HIGH);
    }

    void thread() {
        csn.set(Output::State::HIGH);
        ce.set(Output::State::LOW);

        delay(100);

        // default register values
        writeReg(Register::CONFIG, 0x00);
        writeReg(Register::EN_AA, 0x3F);
        writeReg(Register::EN_RXADDR, 0x03);
        writeReg(Register::SETUP_AW, 0x03);
        writeReg(Register::SETUP_RETR, 0x03);
        writeReg(Register::RF_CH, 0x02);
        writeReg(Register::RF_SETUP, 0x0E);
        writeReg(Register::STATUS, 0x70);
        writeReg(Register::RX_ADDR_P0, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7);
        writeReg(Register::RX_ADDR_P1, 0xC2, 0xC2, 0xC2, 0xC2, 0xC2);
        writeReg(Register::RX_ADDR_P2, 0xC3);
        writeReg(Register::RX_ADDR_P3, 0xC4);
        writeReg(Register::RX_ADDR_P4, 0xC5);
        writeReg(Register::RX_ADDR_P5, 0xC6);
        writeReg(Register::TX_ADDR, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7);
        writeReg(Register::RX_PW_P0, 0x00);
        writeReg(Register::RX_PW_P1, 0x00);
        writeReg(Register::RX_PW_P2, 0x00);
        writeReg(Register::RX_PW_P3, 0x00);
        writeReg(Register::RX_PW_P4, 0x00);
        writeReg(Register::RX_PW_P5, 0x00);
        writeReg(Register::DYNPD, 0x00);

        flushRxFifo();
        flushTxFifo();

        delay(10);

        // MASK_RX_DR = 1
        // MASK_TX_DS = 1
        // MASK_MAX_RT = 1
        // EN_CRC = 1
        // CRCO = 0
        // PWR_UP = 1
        // PRIM_RX = 0
        writeReg(Register::CONFIG, 0x7A);

        delay(10);

        // ERX_P0 = 1
        writeReg(Register::EN_RXADDR, 0x01);

        // AW = 11
        writeReg(Register::SETUP_AW, 0x03);

        // ARD = 0000
        // ARC = 0000
        writeReg(Register::SETUP_RETR, 0x00);

        // RF_CH = 0000010
        writeReg(Register::RF_CH, 0x02);

        // CONT_WAVE = 0
        // RF_DR_LOW = 1
        // PLL_LOCK = 0
        // RF_DR_HIGH = 0
        // RF_PWR = 11
        writeReg(Register::RF_SETUP, 0x26);

        if(transmitter) {
            // TX ADDR = 05 04 03 02 01
            writeReg(Register::TX_ADDR, 0x01, 0x02, 0x03, 0x04, 0x05);
        } else {
            // PIPE0 RX ADDR = 05 04 03 02 01
            writeReg(Register::RX_ADDR_P0, 0x01, 0x02, 0x03, 0x04, 0x05);

            // RX_PW_P0 = 17
            writeReg(Register::RX_PW_P0, 17);
        }

        if(!transmitter) {
            // PRIM_RX = 1
            // CE = 1
            const uint8_t val = readReg(Register::CONFIG);
            writeReg(Register::CONFIG, val | 0x01);
            ce.set(Output::State::HIGH);

            delay(10);
        }

        while(true) {
            if(transmitter) {
                const char *msg = "hello world nRF24";

                const uint8_t status = writeTxPayload(msg, 17);
                if(status & 0x01) {
                    rtos::log << rtos::acquire << "TX_FULL" << rtos::endl << rtos::release;
                }

                {
                    // PRIM_RX = 0
                    // CE = 1
                    const uint8_t val = readReg(Register::CONFIG);
                    writeReg(Register::CONFIG, val & ~0x01);
                    ce.set(Output::State::HIGH);
                }

                delay(100);

                // CE = 0
                ce.set(Output::State::LOW);

                {
                    // MAX_RT cleared
                    const uint8_t val = readReg(Register::STATUS);
                    writeReg(Register::STATUS, val | 0x10);
                }

                flushTxFifo();

                delay(1000);
            } else {
                char msg[32];
                const uint8_t status = readRxPayload(msg);

                if((status & 0x0E) == 0x00) {
                    msg[18] = '\0';
                    rtos::log << rtos::acquire << rtos::dec << msg << rtos::endl << rtos::release;
                }

                delay(100);
            }
        }
    }

public:
    nRF24L01p(SPI_HandleTypeDef &hspi, const Output csn, const Output ce, const bool transmitter)
        : Thread{"nRF24L01+ driver", Thread::Priority::Mid},
          hspi{hspi},
          ce{ce},
          csn{csn},
          transmitter{transmitter} {
    }
};

class Transmitter : Subscriber<messages::Led, 512> {
    nRF24L01p &radio;

    void receive(const messages::Led &message) {
        // serialization
        // radio.transmit();
    }

public:
    Transmitter(nRF24L01p &radio)
        : Subscriber{topics::LedControl, "radio tx", Thread::Priority::Mid}, radio{radio} {
    }
};

class Receiver : Thread<512> {
    Publisher<messages::Led> publisher;
    nRF24L01p &radio;

    void thread() {
        while(true) {
            // radio.receive();
            //  deserialize
            // publisher.publish(topic::message::Led{});
            delay(1);
        }
    }

public:
    Receiver(nRF24L01p &radio)
        : Thread{"radio rx", Thread::Priority::Mid}, publisher{topics::LedControl}, radio{radio} {
    }
};

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

static nRF24L01p radio1(hspi2, Output(GPIOC, GPIO_PIN_1), Output(GPIOC, GPIO_PIN_0), true);
static nRF24L01p radio2(hspi1, Output(GPIOA, GPIO_PIN_10), Output(GPIOA, GPIO_PIN_8), false);

static Transmitter tx(radio1);
static Receiver rx(radio2);
