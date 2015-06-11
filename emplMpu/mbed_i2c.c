
#include "i2c_api.h"

static i2c_t mbed_i2c_object;

void mbed_i2c_init(PinName sda, PinName scl)
{
    i2c_init(&mbed_i2c_object, sda, scl);
    i2c_frequency(&mbed_i2c_object, 100000);
}

int mbed_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data)
{
    int i;
    slave_addr = slave_addr << 1;
    i2c_start(&mbed_i2c_object);
    i2c_byte_write(&mbed_i2c_object, slave_addr);
    i2c_byte_write(&mbed_i2c_object, reg_addr);
    for (i = 0; i < length; i++) {
        i2c_byte_write(&mbed_i2c_object, data[i]);
    }
    i2c_stop(&mbed_i2c_object);
    return 0;
}

int mbed_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data)
{
    slave_addr = slave_addr << 1;
    i2c_write(&mbed_i2c_object, slave_addr, &reg_addr, 1, 0);
    return (i2c_read(&mbed_i2c_object, slave_addr, data, length, 1) == length) ? 0 : 1;
}
