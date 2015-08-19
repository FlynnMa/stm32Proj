#include <string.h>
#ifdef _WINDOWS
#include "mttty.h"
#endif
#include "cmsis.h"
#include "protocal.h"
#include "protocalApi.h"
#include "electronicVehicleCustom.h"
#include "electronicVehicle.h"
#include "Trace.h"

electronicVehicleCustomDataType evCustomData;
const char version[4] = {SOFTWARE_VERSION_MAJOR,
                        SOFTWARE_VERSION_SUB,
                        SOFTWARE_VERSION_MODIFY,
                        SOFTWARE_VERSION_YEAR};

/*!
 * Initialize customer enviroment , to be implemented by customer
 * invoked by electronicVehicleInitialize
 *
 * @param none
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomInit(void)
{
    // init uart device

    // init default settings

    evCustomData.voltage = 48.2;
    evCustomData.fullVoltage = 50.1;
    evCustomData.lowVoltage = 37.43;
    evCustomData.speed = 12.12;
    evCustomData.current = 1.55;
    evCustomData.temperature = 39;

    return 0;
}


extern void electronicVehicleSendData(uint8_t* pBuf, uint32_t len);

/*!
 * Send buffer data via uart, to be implemented by customer
 *
 * @param[out] pBuf   buffer address contains formated data to be sent
 * @param[in] len    length of data to be sent
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomUartSendData(uint8_t* pBuf, uint32_t len)
{
    electronicVehicleSendData(pBuf, len);
    return 0;
}

/*!
 * Get firmware version, this is used on the mobile app start up, it may require
 * the firmware version for checking software compatibility and detect new
 * firmware availability
 *
 * @param[out] pVer  address to be set of firmware version,
 *                        4bytes version in binary formate
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetFirmwareVersion(
    uint32_t *pVer)
{
    memcpy(pVer, version, 4);

    return 0;
}

/*!
 * Get mainboard temperature
 *
 * @param[out] pTemperature   temperature value in float, unit centigrade
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetTemperature(float *pTemperature)
{
    *pTemperature = evCustomData.temperature;

    return 0;
}

/*!
 * Get battery voltage
 *
 * @param[out] pBattery      battery voltage value in float, unit volt
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetBatteryVoltage(float *pBattery)
{
    *pBattery = evCustomData.voltage;
    return 0;
}

/*!
 * Get battery range
 *
 * @param pBatteryOut[out]    battery voltage which indicates used out
 * @param pBatteryFull[out]   battery voltage which indicates battery is full
 * @param pBatteryLow[out]    battery voltage low which will generate warning on mobile phone
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVeichleCustomGetBatteryRange(
        float *pBatteryOut,
        float *pBatteryFull, float *pBatteryLow)
{
    if (NULL != pBatteryOut)
    {
        *pBatteryOut = evCustomData.outVoltage;
    }

    if (NULL != pBatteryFull)
    {
        *pBatteryFull = evCustomData.fullVoltage;
    }

    if (NULL != pBatteryLow)
    {
        *pBatteryLow = evCustomData.lowVoltage;
    }
    return 0;
}

/*!
 * Get electronic current
 *
 * @param pCurrent[out]    battery realtime current
 *
 * @return @ERROR_TYPE
 */
int32_t elecronicVehicleCustomGetCurrent(float *pCurrent)
{
    if (NULL != pCurrent)
    {
        *pCurrent = evCustomData.current;
    }
}

/*!
 * Get max speed
 *
 * @param pMaxSpeed[out]    max speed of this electronic vehicle
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetMaxSpeed(float *pMaxSpeed)
{
    *pMaxSpeed = evCustomData.maxSpeed;
    return 0;
}

/*!
 * Get current speed
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetSpeed(float *pSpeed)
{
    *pSpeed = evCustomData.speed;
    return 0;
}


int32_t electronicVehicleCustomGetChargeStatus(uint8_t *pCharge)
{
    *pCharge = evCustomData.isCharging;
    return 0;
}


void electronicVehicleCustomOnAckFailed(uint8_t cmdID,
    uint8_t err)
{
    TraceDebug("ack failed on cmd:%d, err:%d", cmdID, err);
}

/*!
 * set total mile, this function is invoked on the remote request mile data from
 * mobile phone, normally it is invoked after power up, the target has loss its
 * data, while it contains a backup on mobile phone
 *
 * @param[in] mile total mile counted by mobile phone
 *
 */
void electronicVehicleCustomSetMile(uint32_t mile)
{
    evCustomData.mile = mile;
}

/*!
 * Get total mile, this function is invoked on the request of mile data by
 * mobile phone
 *
 * @param[i] pMile  total mile counted by embeded end
 */
void electronicVehicleCustomGetMile(uint32_t *pMile)
{
    *pMile = evCustomData.mile++;
}

