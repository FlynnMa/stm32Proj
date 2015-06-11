#include "mbed.h"
#include "rtos.h"
#include "util.h"
#ifdef __BOSCH_SENSORS__
#include "bma.h"
#endif
#include "sensors.h"
#include "trace.h"

#ifdef __MPU6050__
#include "MPU6050_6Axis_MotionApps20.h"
#endif

struct SensorInfo {
    bool log;
    bool on;
    bool interval;
    bool exit;
    void *pStack;
};

struct SensorControl{
    SensorInfo  acc;

    SensorInfo  gyro;

    SensorInfo  mag;

    SensorInfo  orientation;

    bool        binary;  /**< log as binary */
};

SensorControl sensorsCtrl;
unsigned char sensorsStack[1024];
Thread *pSensorThread;

#ifdef __BOSCH_SENSORS__
bool sensorsTraceBinary(uint8_t type, void *data, uint8_t len)
{
    uint8_t buf[BINARY_FRAME_MAX_LEN];

    if (len > BINARY_FRAME_MAX_LEN)
    {
        return false;
    }
    buf[0] = BINARY_HEAD_MASK | type;
    buf[1] = len;
    memcpy(&buf[2], data, len);
    buf[2+len] = BINARY_TAIL_MAGIC;

    printf((const char*)buf);
    return true;
}

static void boschSensorProcess(void const *argument)
{
    AxisInt axisRaw;
    SensorInfo *pInfo = &sensorsCtrl.acc;
    I2C i2c(I2C_SDA, I2C_SCL);
    BMA acc(i2c);

    pInfo->interval = 16;
    acc.setInterval(pInfo->interval);
    Thread::wait(10);
    pInfo->exit = false;
    while(pInfo->exit == false)
    {
        acc.update();
        axisRaw = acc.getAccRaw();
        if (pInfo->log == true)
        {
            if (sensorsCtrl.binary == true)
            {
//                axisRaw.x = 0x12345678;
//                axisRaw.y = 0xfedcba09;
//                axisRaw.z = 0x5aa59966;
                sensorsTraceBinary(BINARY_TYPE_ACC, (void*)&axisRaw, sizeof(axisRaw));
            }
            else
            {
                printf("#ACC=%d,%d,%d,\r\n", axisRaw.x, axisRaw.y, axisRaw.z);
            }
        }
        Thread::wait(pInfo->interval);
    }
    acc.setInterval(0);
    pInfo->pThread = NULL;
}
#endif

#ifdef __MPU6050__
const float PI = 3.14159265;
void mpuDataReady(void)
{
    pSensorThread->signal_set(0x1);
}

void mpuProcess(void const *argument)
{
    I2C i2c(I2C_SDA, I2C_SCL);
    MPU6050     mpu(0x68);
    InterruptIn mpuInt(PB_4);
    SensorInfo *pInfo = &sensorsCtrl.orientation;
    uint8_t     intStatus;
    uint8_t     ret;
    uint16_t    fifoCount;
    uint16_t    packetSize;
    Quaternion  q;
    float       ypr[3];
    VectorFloat gravity;    // [x, y, z]            gravity vector
    uint8_t     fifoBuffer[64];

    ret = mpu.getDeviceID();
    if (ret == 0)
    {
        Trace("failed on connecting to mpu:%x", ret);
    }


    mpu.initialize();
    if (mpu.testConnection() == false)
    {
        Trace("MPU6050 connection failed");
        return;
    }

    ret =  mpu.dmpInitialize();
    if (ret != 0)
    {
        Trace("DMP initialize failed:%d", ret);
        return;
    }

    mpu.setDMPEnabled(true);
    mpuInt.rise(&mpuDataReady);

    packetSize = mpu.dmpGetFIFOPacketSize();

    while(pInfo->exit == false)
    {
        Thread::signal_wait(0x1);

        intStatus = mpu.getIntStatus();
        fifoCount = mpu.getFIFOCount();
        if (intStatus & 0x10)
        {
            TraceError("FIFO overflow");
            mpu.resetFIFO();
        }
        if ((intStatus & 0x02) || (intStatus & 0x01))
        {
            while (fifoCount < packetSize)
            {
                fifoCount = mpu.getFIFOCount();
            }
            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);

            fifoCount -= packetSize;

            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            printf("#YPR=%f,%f,%f,\r\n", ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
        }
    }
}

#endif

void sensorsAccEnable(int en)
{
    SensorInfo *pInfo = &sensorsCtrl.acc;

    if (TOGGLE == en)
    {
        pInfo->on = !pInfo->on;
    }
    else
    {
        pInfo->on = en;
    }

    if ((pSensorThread == NULL) && (pInfo->on == true))
    {
#ifdef __BOSCH_SENSORS__
        pSensorThread = new Thread(boschSensorProcess, NULL, osPriorityRealtime, sizeof(sensorsStack), sensorsStack);
#endif
    }
    else if ((pSensorThread) && (pInfo->on == false))
    {
        pInfo->exit = true;
    }
}


void sensorsAccLogEnable(int en)
{
    SensorInfo *pInfo = &sensorsCtrl.acc;

    if (en == TOGGLE)
    {
        pInfo->log =! pInfo->log;
    }
    else
    {
        pInfo->log = en;
    }
}

void sensorsLogBinary(int en)
{
    if (en == TOGGLE)
    {
        sensorsCtrl.binary =! sensorsCtrl.binary;
    }
    else
    {
        sensorsCtrl.binary = en;
    }
}

void sensorsOrientationEnable(int en)
{
    SensorInfo *pInfo = &sensorsCtrl.orientation;
/*
    if (pInfo->pStack == NULL)
    {
        pInfo->pStack = malloc(1024);
    }
*/
    if (TOGGLE == en)
    {
        pInfo->on = !pInfo->on;
    }
    else
    {
        pInfo->on = en;
    }

    if ((pSensorThread == NULL) && (pInfo->on == true))
    {
#ifdef __MPU6050__
        pSensorThread = new Thread(mpuProcess, NULL, osPriorityRealtime, sizeof(sensorsStack), sensorsStack);//(unsigned char*)pInfo->pStack);
#endif
    }
    else if ((pSensorThread) && (pInfo->on == false))
    {
        pInfo->exit = true;
    }
}

void sensorsOrientationLogEnable(int en)
{
    SensorInfo *pInfo = &sensorsCtrl.orientation;

    if (en == TOGGLE)
    {
        pInfo->log =! pInfo->log;
    }
    else
    {
        pInfo->log = en;
    }
}

