#include "mbed.h"
#include "rtos.h"
#include "util.h"
#ifdef __BOSCH_SENSORS__
#include "bma.h"
#endif
#include "sensors.h"
#include "trace.h"

#if __MPU6050__
#include "MPU6050_6Axis_MotionApps20.h"
#endif

#ifdef __EMPLMPU__
#include "mbed_i2c.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
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


    acc.update();
    axis = acc.getAccRaw();
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
    uint16_t    packetNum;
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
//            Trace("int : %d", intStatus);
            while (fifoCount < packetSize)
            {
                fifoCount = mpu.getFIFOCount();
            }

            packetNum = fifoCount / packetSize;
//            TraceLine();
            while(packetNum --)
            {
                // read a packet from FIFO
                mpu.getFIFOBytes(fifoBuffer, packetSize);

                fifoCount -= packetSize;

                // display Euler angles in degrees
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                printf("#YPR=%f,%f,%f\r\n", ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
//                printf("FIFO:%d,packet:%d\r\n", (int)fifoCount, (int)packetSize);
            }
            if(fifoCount != 0)
            {
                printf("FIFO:%d,packet:%d\r\n", (int)fifoCount, (int)packetSize);
                mpu.resetFIFO();
            }
        }
    }
}

#endif

#ifdef __EMPLMPU__
/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (100)

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static inline unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static signed char board_orientation[9] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
};
Thread *pMpuThread;

void tap_cb(unsigned char direction, unsigned char count)
{
    Trace("Tap motion detected\n");
}

void android_orient_cb(unsigned char orientation)
{
    Trace("Oriention changed\n");
}

void mpuDataReady(void)
{
    pSensorThread->signal_set(0x1);
//    Trace("data ready");
}

uint8_t MPU6050::dmpGetYawPitchRoll(float *data, long *q, VectorFloat *gravity) {
    long w,x,y,z;

    w = q[0], x = q[1], y = q[2], z = q[3];
    // yaw: (about Z axis)
    data[0] = atan2(2* x * y - 2 * w * z, 2 * w * w + 2* x* x - 1);
    // pitch: (nose up/down, about Y axis)
    data[1] = atan(gravity -> x / sqrt(gravity -> y*gravity -> y + gravity -> z*gravity -> z));
    // roll: (tilt left/right, about X axis)
    data[2] = atan(gravity -> y / sqrt(gravity -> x*gravity -> x + gravity -> z*gravity -> z));
    return 0;
}

void mpuProcess(void const *argument)
{
    SensorInfo *pInfo = &sensorsCtrl.acc;
    uint8_t accel_fsr;
    uint16_t gyro_rate, gyro_fsr;
    InterruptIn mpuInt(PB_4);
    uint8_t ret;

    Trace("mpu process");
    mbed_i2c_init(I2C_SDA, I2C_SCL);
    ret = mpu_get_device_id();
    Trace("Get device id:%x", ret);
    mpu_init(0);

    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);

    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);

    dmp_load_motion_driver_firmware();
    dmp_set_orientation(inv_orientation_matrix_to_scalar(board_orientation));
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(android_orient_cb);

    uint16_t dmp_features = DMP_FEATURE_6X_LP_QUAT |
                       DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL |
                       DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    mpu_set_dmp_state(true);

    dmp_set_tap_thresh(TAP_XYZ, 50);

    dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);

    mpuInt.rise(mpuDataReady);

    while(pInfo->exit == false)
    {
        unsigned long sensor_timestamp;
        short gyro[3], accel[3], sensors;
        long quat[4];
        unsigned char more = 1;

        Thread::signal_wait(0x1);

        dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,
                              &more);

        if (sensors & INV_XYZ_GYRO)
        {
            // LOG("GYRO: %d, %d, %d\n", gyro[0], gyro[1], gyro[2]);
        }
        if (sensors & INV_XYZ_ACCEL)
        {
            // LOG("ACC: %d, %d, %d\n", accel[0], accel[1], accel[2]);
        }

        /* Unlike gyro and accel, quaternions are written to the FIFO in
         * the body frame, q30. The orientation is set by the scalar passed
         * to dmp_set_orientation during initialization.
         */
         if (sensors & INV_WXYZ_QUAT)
         {
             // LOG("QUAT: %ld, %ld, %ld, %ld\n", quat[0], quat[1], quat[2], quat[3]);
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
#else
		pSensorThread = new Thread(mpuProcess, NULL, osPriorityRealtime, sizeof(sensorsStack), sensorsStack);
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
