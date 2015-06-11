#ifndef UTIL_H
#define UTIL_H

#define Bit(n)                                (1 << (n))
#define ClrBits(val, msk)                     ((val) = (val) & (~(msk)))
#define SetBits(val, msk, setVal)             ((val) = (ClrBits(val,msk) | setVal))

/**
 * @brief sampling speed
 */
enum SensorSamplingSpeed {
	SENSOR_SAMPLE_STOP       = 0, /**< sample stopped */
    SENSOR_SAMPLE_SLOW       = 1, ///< ultra low power (1 time)
    SENSOR_SAMPLE_STANDARD   = 2, ///< standard (2 times)
    SENSOR_SAMPLE_HIGH       = 3, ///< high resolution (4 times)
    SENSOR_SAMPLE_ULTRA_HIGH = 4  ///< ultra high resolution (8 times)
};

typedef struct AxisFloatType{
	float x;
	float y;
	float z;
} AxisFloat;

typedef struct AxisIntType {
	int x;
	int y;
	int z;
}AxisInt;

void utilI2cWriteReg(I2C &i2c, uint8_t addr, uint8_t reg, void *buf, int length);
void utilI2cReadReg(I2C &i2c, uint8_t addr, uint8_t reg, void *buf, int length);

#endif
