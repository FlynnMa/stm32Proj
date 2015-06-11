/*
 * mbed library to use a Bosch Sensortec BMP085/BMP180 sensor
 * Copyright (c) 2010 Hiroshi Suga
 * Released under the MIT License: http://mbed.org/license/mit
 */

/** @file BMP085.cpp
 * @brief mbed library to use a Bosch Sensortec BMP085/BMP180 sensor
 * barometric pressure sensor BMP085/BMP180 (Bosch Sensortec)
 * interface: I2C digital
 */

#include "mbed.h"
#include "bma.h"
#include "rtos.h"

#define BMA_CHIPID          0x00

#define BMA_X_AXIS          0x02
#define BMA_Y_AXIS          0x04
#define BMA_Z_AXIS          0x06

#define BMA_RANGE           0x0f
#define BMA_RAGE_MSK        0x0f

#define BMA_BANDWIDTH       0x10
#define BMA_BANDWIDTH_MSK   0x1f


#define BMA_MODE_CTRL       0x11
#define BMA_MODE_MSK        (Bit(7) | Bit(6) | Bit(5))

#define BMA_LOW_NOISE_CTRL  0x12
#define BMA_LOW_NOISE_MSK   (Bit(6))

#define BMA_SOFTRESET       0x14
#define BMA_SOFTRESET_VALUE 0xB6

struct     BmaInfo
{
    uint8_t  addr;
    uint8_t  chipID;
    uint8_t       resolution; // resolution of sampling data in bits(8bits, 10bits, 12bits)
    const char    *name;
};

struct BmaSpeedConvert
{
    uint32_t interval;   // interval in milli-seconds
    uint8_t  bandwidth;  // bandwidth for bma settings
};

struct BmaRangeConvert
{
    BmaRange range;    // range value
    uint8_t  regValue; // register value for bma
};
/*
 * convert interval to bma bandwidth
*/
static const BmaSpeedConvert intervalTbl[] =
{
    {64,   0x08}, //bandwidth 8
    {32,   0x09}, //bandwidth 16
    {16,   0x0a}, //bandwidth 31
    {8,    0x0b}, //bandwidth 63
    {4,    0x0c}, //bandwidth 125
    {2,    0x0d}, //bandwidth 250
};

static const BmaRangeConvert rangeTbl[] =
{
    {BMA_RANGE_2G,  3},
    {BMA_RANGE_4G,  5},
    {BMA_RANGE_8G,  8},
    {BMA_RANGE_16G, 12}
};

static const BmaInfo bmaInfoTbl[] =
{
    {0x18, 0xff, 10, "bma250"},
    {0x19, 0xfa, 12, "bma255(bmi055)"},
    {0x10, 0xfa, 12, "bma255"},
    {0x28, 0xfb, 14, "bma280"}
};

/**
 * @brief Initializes interface (private I2C)
 * @param sda port of I2C SDA
 * @param scl port of I2C SCL
 * @param bmaSamplingSpeed parameter of OSS
 */
BMA::BMA (PinName sda, PinName scl, uint16_t interval, BmaRange range) : m_i2c(sda, scl)
{
    m_range = range;
    init(interval);
}

/**
 * @brief Initializes interface (public I2C)
 * @param p_i2c instance of I2C class
 * @param p_oss parameter of OSS
 */
BMA::BMA (I2C& p_i2c, uint16_t interval, BmaRange range) : m_i2c(p_i2c)
{
    m_range = range;
    init(interval);
}

/**
 * @brief Get temperature
 * @return temperature (`C)
 */
AxisFloat BMA::getAcc()
{
    return m_acc;
}

/**
 * @brief Get pressure
 * @return pressure (hPa)
 */
AxisInt BMA::getAccRaw()
{
    return m_accRaw;
}

int16_t BMA::readAxisReg(uint8_t reg, uint8_t bitsNum)
{
    uint8_t data[2];
    int16_t val;
    uint8_t shiftNum;

   utilI2cReadReg(m_i2c, m_addr, reg, (uint8_t*)data, 2);
   if ((Bit(0) & data[0]) == 0)
   {
        Thread::wait(1);
        utilI2cReadReg(m_i2c, m_addr, reg, (uint8_t*)data, 2);
   }
   shiftNum = 16 - bitsNum;
   val = (data[0] >> shiftNum)| (data[1] << (8 - shiftNum));
   val = val << shiftNum;
   val = val >> shiftNum;
   return val;
 }

/**
 * @brief Update results
 */
void BMA::update()
{
    memset(&m_accRaw, 0, sizeof(m_accRaw));
    m_accRaw.x = (int)readAxisReg(BMA_X_AXIS, m_resolution);
    m_accRaw.y = (int)readAxisReg(BMA_Y_AXIS, m_resolution);
    m_accRaw.z = (int)readAxisReg(BMA_Z_AXIS, m_resolution);

    //    m_accRaw.x = val & m_valueMsk;
//    utilI2cReadReg(m_i2c, m_addr, BMA_X_AXIS, (uint8_t*)data, 2);
//    utilI2cReadReg(m_i2c, m_addr, BMA_Y_AXIS, (uint8_t*)data, 2);
//    m_accRaw.y = (int)val;
//    m_accRaw.y = val & m_valueMsk;
//    utilI2cReadReg(m_i2c, m_addr, BMA_Z_AXIS, (uint8_t*)data, 2);
//    m_accRaw.z = (int)val;
//    m_accRaw.z = val & m_valueMsk;
}

bool BMA::setRange(BmaRange range)
{
    int i, num;
    uint8_t regVal;

    num = sizeof(rangeTbl) / sizeof(BmaRangeConvert);
    for( i = 0; i < num; i++)
    {
        if(range == rangeTbl[i].range)
        {
            regVal = rangeTbl[i].regValue;
        }
    }

    if (i >= num)
    {
        return false;
    }
    utilI2cWriteReg(m_i2c, m_addr, BMA_RANGE, &regVal, 1);
    return true;
}

bool BMA::setInterval(uint32_t interval)
{
    uint8_t mode, lowNoise;
    int i, num;
    uint8_t bandwidth;

    num = sizeof(intervalTbl) / sizeof(BmaSpeedConvert);

    for(i = 0; i < num; i++)
    {
        if (interval == intervalTbl[i].interval)
        {
            bandwidth = intervalTbl[i].bandwidth;
            break;
        }
    }
    if (i >= num)
    {
        return false;
    }

    utilI2cReadReg(m_i2c, m_addr, BMA_MODE_CTRL, &mode, 1);
    utilI2cReadReg(m_i2c, m_addr, BMA_LOW_NOISE_CTRL, &lowNoise, 1);
    if (interval)
    {
        SetBits(mode, BMA_MODE_MSK, 0);
        SetBits(lowNoise, BMA_LOW_NOISE_MSK, 0);
    }
    else
    {
        SetBits(mode, BMA_MODE_MSK, Bit(5));
        SetBits(lowNoise, BMA_LOW_NOISE_MSK, Bit(6));
    }
    utilI2cWriteReg(m_i2c, m_addr, BMA_MODE_CTRL, &mode, 1);
    Thread::wait(1);
    utilI2cWriteReg(m_i2c, m_addr, BMA_LOW_NOISE_CTRL, &lowNoise, 1);

    if (interval)
    {
        Thread::wait(5);
        utilI2cWriteReg(m_i2c, m_addr, BMA_BANDWIDTH, &bandwidth, 1);
        setRange(m_range);
    }

    return true;
}

uint32_t BMA::getInterval(void)
{
    return m_interval;
}


void BMA::init (uint32_t interval) {
    uint8_t addr = 0;
    char data = 0;
    int i, num;

    m_interval = interval;
    num = sizeof(bmaInfoTbl) / sizeof(BmaInfo);
    for (i = 0; i < num; i++)
    {
        addr = bmaInfoTbl[i].addr << 1;
        data = BMA_CHIPID; // set register addr as 0
        m_i2c.write((int)addr, &data, 1, true);
        data = 0; // read chip id
        m_i2c.read((int)addr, &data, 1);
        utilI2cReadReg(m_i2c, addr, BMA_CHIPID, &data, 1);
        if (data == bmaInfoTbl[i].chipID)
            break;
    }
    if (i >= num)
    {
        printf("detect bma failed!\r\n");
        return;
    }

    data = BMA_SOFTRESET_VALUE;
    utilI2cWriteReg(m_i2c,(int)addr, BMA_SOFTRESET, &data, 1);
    Thread::wait(10);
    m_addr = addr;
    m_chipID = bmaInfoTbl[i].chipID;
    m_resolution = bmaInfoTbl[i].resolution;
    strncpy(m_name, bmaInfoTbl[i].name, sizeof(m_name));
    printf("detect %s, chipid=%x, addr=%x\r\n", m_name, m_chipID, m_addr/2);

    setInterval(interval);
    setRange(m_range);

    m_valueMsk = 0;
    for(i = 0; i < m_resolution; i++)
    {
        m_valueMsk |= Bit(i);
    }
}

