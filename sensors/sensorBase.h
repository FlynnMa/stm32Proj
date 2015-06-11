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

#ifndef sensorbase_h
#define sensorbase_h

#include "mbed.h"
#include "util.h"

typedef void (*sensorCallbackFunction)(AxisInt *axisRaw, int interval);

/**
 * @brief base class of sensors
 */
class SensorBase {
public:
    SensorBase(PinName sda, PinName scl, sensorCallbackFunction func, int interval = 0, int range = 0);
    SensorBase(I2C& i2c, sensorCallbackFunction func, int interval = 0, int range = 0);

    /* get filtered axis data */
    virtual AxisFloat getAxis(void);

    /* get original raw data */
    virtual AxisInt getAxisRaw(void);

    /* set 0 to disable sensor, or else non-zero to enable sampling */
    virtual bool setInterval(uint32_t);

    /* get current interval */
    virtual uint32_t getInterval(void);

    /* set device sample range */
    virtual bool setRange(int range);

protected:
    virtual void update(void);
    virtual void init(uint32_t);

private:
//    virtual int16_t readAxisReg(uint8_t reg, uint8_t bitsNum);

    I2C                      m_i2c;
    AxisFloat                m_axisFloat;
    AxisInt                  m_axisRaw;
    int                      m_interval; /* duration between samples */
    int                      m_range;
    uint8_t                  m_resolution;
    uint16_t                 m_valueMsk;
    uint8_t                  m_addr;
    uint8_t                  m_chipID;
    const char              *m_name;
    sensorCallbackFunction   m_callback;

};

#endif
