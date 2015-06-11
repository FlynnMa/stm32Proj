#include "mbed.h"
#include "util.h"

void utilI2cWriteReg(I2C &i2c, uint8_t addr, uint8_t reg, void *buf, int length)
{
    char *p = (char*)malloc(length +1);

    p[0] = reg;
    memcpy(&p[1], buf, length);
    i2c.write((int)addr, (char*)p, 1 + length);
    free((void*)p);
//    i2c.write((int)addr, (char*)buf, length);
}

void utilI2cReadReg(I2C &i2c, uint8_t addr, uint8_t reg, void *buf, int length)
{
    i2c.write((int)addr, (char*)&reg, 1, true);
    i2c.read((int)addr, (char*)buf, length);
}

void testBitsUtil(void)
{
    uint32_t val = 0xffffffff;
    uint32_t msk = Bit(10) | Bit(11);
    uint32_t setVal = Bit(10);

    ClrBits(val,msk);
    SetBits(val,msk,setVal);
}

