#include <stm32u0xx_hal.h>

#include "rtos/Thread.hpp"

using namespace rtos;

extern SPI_HandleTypeDef hspi2;

class nRF24L01p : Thread<1024> {
    enum Register {
        CONFIG = 0x00,
        EN_AA = 0x01,
        EN_RXADDR = 0x02,
        SETUP_AW = 0x03,
        SETUP_RETR = 0x04,
        RF_CH = 0x05,
        RF_SETUP = 0x06,
        STATUS = 0x07,
        OBSERVE_TX = 0x08,
        CD = 0x09,
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

    enum PaDbm {
        MIN,
        LOW,
        HIGH,
        MAX,
        ERROR,
    };

    enum DataRate {
        _1MBPS,
        _2MBPS,
        _250KBPS,
    };

    enum CRCMode {
        DISABLED,
        _8,
        _16,
    };

    static constexpr uint8_t MASK_RX_DR = 6;
    static constexpr uint8_t MASK_TX_DS = 5;
    static constexpr uint8_t MASK_MAX_RT = 4;
    static constexpr uint8_t EN_CRC = 3;
    static constexpr uint8_t CRCO = 2;
    static constexpr uint8_t PWR_UP = 1;
    static constexpr uint8_t PRIM_RX = 0;
    static constexpr uint8_t ENAA_P5 = 5;
    static constexpr uint8_t ENAA_P4 = 4;
    static constexpr uint8_t ENAA_P3 = 3;
    static constexpr uint8_t ENAA_P2 = 2;
    static constexpr uint8_t ENAA_P1 = 1;
    static constexpr uint8_t ENAA_P0 = 0;
    static constexpr uint8_t ERX_P5 = 5;
    static constexpr uint8_t ERX_P4 = 4;
    static constexpr uint8_t ERX_P3 = 3;
    static constexpr uint8_t ERX_P2 = 2;
    static constexpr uint8_t ERX_P1 = 1;
    static constexpr uint8_t ERX_P0 = 0;
    static constexpr uint8_t AW = 0;
    static constexpr uint8_t ARD = 4;
    static constexpr uint8_t ARC = 0;
    static constexpr uint8_t PLL_LOCK = 4;
    static constexpr uint8_t RF_DR = 3;
    static constexpr uint8_t RF_PWR = 6;
    static constexpr uint8_t RX_DR = 6;
    static constexpr uint8_t TX_DS = 5;
    static constexpr uint8_t MAX_RT = 4;
    static constexpr uint8_t RX_P_NO = 1;
    static constexpr uint8_t TX_FULL = 0;
    static constexpr uint8_t PLOS_CNT = 4;
    static constexpr uint8_t ARC_CNT = 0;
    static constexpr uint8_t TX_REUSE = 6;
    static constexpr uint8_t FIFO_FULL = 5;
    static constexpr uint8_t TX_EMPTY = 4;
    static constexpr uint8_t RX_FULL = 1;
    static constexpr uint8_t RX_EMPTY = 0;
    static constexpr uint8_t DPL_P5 = 5;
    static constexpr uint8_t DPL_P4 = 4;
    static constexpr uint8_t DPL_P3 = 3;
    static constexpr uint8_t DPL_P2 = 2;
    static constexpr uint8_t DPL_P1 = 1;
    static constexpr uint8_t DPL_P0 = 0;
    static constexpr uint8_t EN_DPL = 2;
    static constexpr uint8_t EN_ACK_PAY = 1;
    static constexpr uint8_t EN_DYN_ACK = 0;

    static constexpr uint8_t R_REGISTER = 0x00;
    static constexpr uint8_t W_REGISTER = 0x20;
    static constexpr uint8_t REGISTER_MASK = 0x1F;
    static constexpr uint8_t ACTIVATE = 0x50;
    static constexpr uint8_t R_RX_PL_WID = 0x60;
    static constexpr uint8_t R_RX_PAYLOAD = 0x61;
    static constexpr uint8_t W_TX_PAYLOAD = 0xA0;
    static constexpr uint8_t W_ACK_PAYLOAD = 0xA8;
    static constexpr uint8_t FLUSH_TX = 0xE1;
    static constexpr uint8_t FLUSH_RX = 0xE2;
    static constexpr uint8_t REUSE_TX_PL = 0xE3;
    static constexpr uint8_t NOP = 0xFF;

    static constexpr uint8_t LNA_HCURR = 0;

    static constexpr uint8_t RPD = 0x09;

    static constexpr uint8_t RF_DR_LOW = 5;
    static constexpr uint8_t RF_DR_HIGH = 3;
    static constexpr uint8_t RF_PWR_LOW = 1;
    static constexpr uint8_t RF_PWR_HIGH = 2;

    inline void ce(const bool state) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    inline void csn(const bool state) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    uint8_t readReg(const Register reg) {
        const uint8_t txData[2] = {
            static_cast<uint8_t>(R_REGISTER | (REGISTER_MASK & reg)),
            0,
        };
        uint8_t rxData[2] = {0};

        csn(false);
        HAL_SPI_TransmitReceive(&hspi2, txData, rxData, 2, HAL_MAX_DELAY);
        csn(true);

        return rxData[1];
    }

    void writeReg(const Register reg, const uint8_t value) {
        const uint8_t txData[2] = {
            static_cast<uint8_t>(W_REGISTER | (REGISTER_MASK & reg)),
            value,
        };

        csn(false);
        HAL_SPI_Transmit(&hspi2, txData, 2, HAL_MAX_DELAY);
        csn(true);
    }

    void flushRx() {
        const uint8_t txData[1] = {FLUSH_RX};

        csn(false);
        HAL_SPI_Transmit(&hspi2, txData, 1, HAL_MAX_DELAY);
        csn(true);
    }

    void flushTx() {
        const uint8_t txData[1] = {FLUSH_TX};

        csn(false);
        HAL_SPI_Transmit(&hspi2, txData, 1, HAL_MAX_DELAY);
        csn(true);
    }

    void setChannel(uint8_t channel) {
        if(channel > 127) {
            channel = 127;
        }
        writeReg(RF_CH, channel);
    }

    void setPALevel(const PaDbm level) {
        uint8_t setup = readReg(RF_SETUP);

        setup &= ~((1 << RF_PWR_LOW) | (1 << RF_PWR_HIGH));

        switch(level) {
            case MIN: {

            } break;
            case LOW: {
                setup |= (1 << RF_PWR_LOW);
            } break;
            case HIGH: {
                setup |= (1 << RF_PWR_HIGH);
            } break;
            case MAX: {
                setup |= ((1 << RF_PWR_LOW) | (1 << RF_PWR_HIGH));
            } break;
            case ERROR: {
                setup |= ((1 << RF_PWR_LOW) | (1 << RF_PWR_HIGH));
            } break;
        }

        writeReg(RF_SETUP, setup);
    }

    void setDataRate(const DataRate datarate) {
        uint8_t setup = readReg(RF_SETUP);

        setup &= ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));

        switch(datarate) {
            case _1MBPS: {

            } break;
            case _2MBPS: {
                setup |= (1 << RF_DR_HIGH);
            } break;
            case _250KBPS: {
                setup |= (1 << RF_DR_LOW);
            } break;
        }

        writeReg(RF_SETUP, setup);
    }

    void setCRCLength(const CRCMode length) {
        uint8_t config = readReg(CONFIG);

        config &= ~((1 << CRCO) | (1 << EN_CRC));

        switch(length) {
            case DISABLED: {

            } break;
            case _8: {
                config |= (1 << EN_CRC);
            } break;
            case _16: {
                config |= (1 << EN_CRC);
                config |= (1 << CRCO);
            } break;
        }

        writeReg(CONFIG, config);
    }

    void thread() {
        ce(false);
        csn(true);

        delay(5);

        writeReg(SETUP_RETR, (0b0100 << ARD) | (0b1111 << ARC));

        setPALevel(PaDbm::MAX);
        setDataRate(DataRate::_250KBPS);
        setDataRate(DataRate::_1MBPS);
        setCRCLength(CRCMode::_16);

        writeReg(DYNPD, 0);
        writeReg(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

        setChannel(76);

        flushRx();
        flushTx();

        while(true) {
            delay(100);
        }
    }

public:
    nRF24L01p() : Thread{"nRF24L01+ driver", Thread::Priority::Mid} {
    }
};

static nRF24L01p nrf24l01p;
