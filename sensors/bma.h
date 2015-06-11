/*
 * mbed library to use a Bosch Sensortec BMP085/BMP180 sensor
 * Copyright (c) 2010 Hiroshi Suga
 * Released under the MIT License: http://mbed.org/license/mit
 */

/** @file BMP085.h
 * @brief mbed library to use a Bosch Sensortec BMP085/BMP180 sensor
 * barometric pressure sensor BMP085/BMP180 (Bosch Sensortec)
 * interface: I2C digital
 */

#ifndef BMP085_H
#define BMP085_H

#include "mbed.h"
#include "util.h"

enum BmaRange{
    BMA_RANGE_2G,
    BMA_RANGE_4G,
    BMA_RANGE_8G,
    BMA_RANGE_16G
};
/**
 * @brief BMA class
 */
class BMA {
public:
    BMA(PinName sda, PinName scl, uint16_t interval = 0, BmaRange range = BMA_RANGE_2G);
    BMA(I2C& p_i2c, uint16_t interval = 0, BmaRange range = BMA_RANGE_2G);

    AxisFloat getAcc(void);

    AxisInt getAccRaw(void);

    void update(void);

    bool setInterval(uint32_t);

    uint32_t getInterval(void);

    bool setRange(BmaRange);

protected:
    void init(uint32_t);

private:
    int16_t readAxisReg(uint8_t reg, uint8_t bitsNum);

    I2C                m_i2c;
    AxisFloat          m_acc;
    AxisInt            m_accRaw;
    int                m_interval; /* duration between samples */
    BmaRange           m_range;
    uint8_t            m_resolution;
    uint16_t           m_valueMsk;
    uint8_t            m_addr;
    uint8_t            m_chipID;
    char               m_name[10];
};

#endif
