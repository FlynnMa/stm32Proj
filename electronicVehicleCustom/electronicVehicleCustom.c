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
    evCustomData.speed = 12.12;

    return 0;
}


void electronicVehicleSendData(uint8_t* pBuf, uint32_t len);

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
#ifdef _WINDOWS
    if (!mtttySendUart((char*)pBuf, len))
        return ERROR_HW_FAILURE;
#else
    electronicVehicleSendData(pBuf, len);
#endif
    return 0;
}

/*!
 * protocal events response, to be implemented by customer
 *
 * @param[in] event event to be responsed, relate to @EV_EVENT_TYPE
 * @param[in] data  data corresponding with event
 *
 * @return none
 */
void electronicVehicleCustomEvents(EV_EVENT_TYPE event, uint32_t data)
{
    switch(event)
    {
            break;

        default:
            break;
    }
}

/*!
 * Get firmware version, this is used on the mobile app start up, it may require
 * the firmware version for checking software compatibility and detect new
 * firmware availability
 *
 * @param[out] ppBuf  address to be set of firmware version,
 *                        which is ASCII string formate, be sure that the address of *ppBuf
 *                        is a const char type which will not be released
 * @param[out] pLen   length of ppBuf contents
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetFirmwareVersion(
    const char **ppBuf, uint8_t *pLen)
{
    static const char *pVersion = "v0.1";
    *ppBuf = pVersion;
    *pLen = strlen(pVersion) + 1;

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
int32_t electronicVeichleCustomGetBatteryRange(float *pBatteryOut, float *pBatteryFull, float *pBatteryLow)
{
    *pBatteryOut = evCustomData.outVoltage;
    *pBatteryFull = evCustomData.fullVoltage;
    *pBatteryLow = evCustomData.lowVoltage;
    return 0;
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
}


void electronicVehicleCustomOnAckFailed(uint8_t cmdID,
    uint8_t err)
{
    TraceDebug("ack failed on cmd:%d, err:%d", cmdID, err);
}

/*!
 * set total mile, this function is received after target request mile data from
 * mobile phone, normally it is invoked after power up, the target has loss its
 * data, while it contains a backup on mobile phone
 *
 * @param[in] mile total mile counted
 *
 */
void electronicVehicleCustomSetMile(uint32_t mile)
{
    evCustomData.mile = mile;
}

