#ifndef sensors_h
#define sensors_h

#define TOGGLE    2

#define BINARY_HEAD_MASK                0xA0
#define BINARY_TAIL_MAGIC               0xed

#define BINARY_TYPE_ACC                 0x01 /**< x,y,z 4bytes each */
#define BINARY_TYPE_GYRO                0x02
#define BINARY_TYPE_MAG                 0x03
#define BINARY_TYPE_ORIENTATION         0x04
#define BINARY_TYPE_LINEAR              0x05
#define BINARY_TYPE_GRAVITY             0x06
#define BINARY_TYPE_EULA                0x07
#define BINARY_TYPE_PRESSURE            0x08
#define BINARY_TYPE_PROXIMITY           0x09

#define BINARY_FRAME_MAX_LEN            34

struct SensorDataBin {
    uint8_t head; /**< head mask | binaryType */
    uint8_t length;
    void    *data;  /* dynamic length */
} __attribute__((__packed__));

void sensorsAccEnable(int en);

void sensorsAccLogEnable(int en);

void sensorsLogBinary(int en);

void sensorsOrientationEnable(int en);

void sensorsOrientationLogEnable(int en);


#endif
