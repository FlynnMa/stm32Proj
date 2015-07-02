
#include "mttty.h"
#include "protocal.h"
#include "protocalApi.h"
#include "protocalCustomWin32.h"

ProtocalCustomDataType protocalData;


/*!
 * Initialize customer enviroment , to be implemented by customer
 *
 * @param none
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomInit(void)
{
    // init uart device

    // init default settings
    protocalData.scheduleDuration = PROTOCAL_CUSTOM_SCHEDULE_DURATION_DEFAULT;
}

void electronicVehicleCustomeOnSchedule(uint32_t duration)
{
    electronicVehicleSchedule(duration, NULL);
}

#ifdef _WINDOWS
void electronicVehicleCustomScheduleTimer(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
    static DWORD lastTime = 0;

    electronicVehicleCustomeOnSchedule((uint32_t)(dwTime - lastTime));
    lastTime = dwTime;
}
#endif


#ifdef _WINDOWS
UINT_PTR timerID;
#endif
/*!
 * Start schedule task for protocal
 */
int32_t protocalCustomStartSchedule(void)
{
#ifdef _WINDOWS
    timerID = SetTimer(NULL, NULL, protocalData.scheduleDuration, electronicVehicleCustomScheduleTimer);
#endif
}

/*!
 * Stop schedule task for protocal
 */
int32_t protocalCustomStopSchedule(void)
{
#ifdef _WINDOWS
    KillTimer(NULL, timerID);
#endif
}

#ifdef _WINDOWS
extern int mtttySendUart(char *pData, uint32_t length);
#endif

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
#endif
}

/*!
 * protocal events response, to be implemented by customer
 *
 * @param[in] event event to be responsed, relate to @EV_EVENT_TYPE
 * @param[in] data  data corresponding with event
 *
 * @return none
 */
void protocalCustomEvents(EV_EVENT_TYPE event, uint32_t data)
{

}

/*!
 * Get firmware version
 *
 * @param[out] ppBuf  address to be set of firmware version,
 *                        which is ASCII string formate, be sure that the address of *ppBuf
 *                        is a const char type which will not be released
 * @param[out] pLen   length of ppBuf contents
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetFirmwareVersion(const char **ppBuf, uint32_t *pLen)
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
int32_t protocalCustomGetTemperature(float *pTemperature)
{
    *pTemperature = protocalData.temperature;
}

/*!
 * Get battery voltage
 *
 * @param[out] pBattery      battery voltage value in float, unit volt
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetBatteryVoltage(float *pBattery)
{
    *pBattery = protocalData.voltage;
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
int32_t protocalCustomGetBatteryRange(float *pBatteryOut, float *pBatteryFull, float *pBatteryLow)
{
    *pBatteryOut = protocalData.outVoltage;
    *pBatteryFull = protocalData.fullVoltage;
    *pBatteryLow = protocalData.lowVoltage;
    return 0;
}

/*!
 * Get max speed
 *
 * @param pMaxSpeed[out]    max speed of this electronic vehicle
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetMaxSpeed(float *pMaxSpeed)
{
    *pMaxSpeed = protocalData.maxSpeed;
    return 0;
}

/*!
 * Get current speed
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetSpeed(float *pSpeed)
{
    *pSpeed = protocalData.speed;
    return 0;
}

