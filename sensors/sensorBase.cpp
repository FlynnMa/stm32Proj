
/** @file sensorBase.cpp
 * @brief created for sensor base class
 * interface: I2C digital
 */

#include "mbed.h"
#include "sensorBase.h"
#include "rtos.h"

/**
 * @brief Initializes interface (private I2C)
 * @param sda port of I2C SDA
 * @param scl port of I2C SCL
 * @param bmaSamplingSpeed parameter of OSS
 */
SensorBase::SensorBase(PinName sda, PinName scl, sensorCallbackFunction func, int interval, int range)
: m_i2c(sda, scl)
{
    m_range = range;
    m_interval = interval;
    init(interval);
}

/**
 * @brief Initializes interface (public I2C)
 * @param p_i2c instance of I2C class
 * @param
 */
SensorBase::SensorBase(I2C& i2c, sensorCallbackFunction func, int interval, int range)
	:m_i2c(i2c)
{
    m_range = range;
    m_interval = interval;
    init(interval);
}

/**
 * @brief  get filtered axis data
 * @return filtered axis data
 */
AxisFloat SensorBase:: getAxis(void)
{
    return m_axisFloat;
}

/**
 * @brief Get pressure
 * @return pressure (hPa)
 */
AxisInt SensorBase::getAxisRaw()
{
    return m_axisRaw;
}


/**
 * @brief Update results
 */
void SensorBase::update(void)
{

}

bool SensorBase::setRange(int range)
{

    return true;
}

bool SensorBase::setInterval(uint32_t interval)
{
    return true;
}

uint32_t SensorBase::getInterval(void)
{
    return m_interval;
}


void SensorBase::init (uint32_t interval)
{

}

