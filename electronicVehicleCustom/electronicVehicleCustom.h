#ifndef protocalCustomWin32_h
#define protocalCustomWin32_h
#include <stdint.h>

/*! default protocal schedual duration */
#define PROTOCAL_CUSTOM_SCHEDULE_DURATION_DEFAULT    100

/*!
 * Electronic Vehicle events supported
 */
typedef enum  {
    EV_EVENT_BLUETOOTH_CONNECTION, /**< bluetooth connection status changed */
    EV_EVENT_RESET,
    EV_EVENT_REPLY,
    EV_EVENT_SETTINGS,
    EV_EVENT_MAX
} EV_EVENT_TYPE;


typedef struct electronicVehicleCustomData{
    float voltage;
    float fullVoltage;
    float outVoltage;
    float lowVoltage;
    float speed;
    float mileage;
    float maxSpeed;
    float temperature;

    uint32_t scheduleDuration;  /**< schedule duration for the protocal in milli-seconds, default 100ms */
    uint32_t batteryInterval;
    uint8_t  isCharging;

    uint32_t mile;
} electronicVehicleCustomDataType;

#ifdef __cplusplus
extern "C" {
#endif

/*!
* Initialize customer enviroment , to be implemented by customer
*
* @param none
* @return @ERROR_TYPE
*/
int32_t electronicVehicleCustomInit(void);

/*!
* Start schedule task for protocal
*/
int32_t electronicVehicleCustomStartSchedule(void);

/*!
* Stop schedule task for protocal
*/
int32_t electronicVehicleCustomStopSchedule(void);

/*!
 * Send buffer data via uart, to be implemented by customer
 *
 * @param[out] pBuf   buffer address contains formated data to be sent
 * @param[in] len    length of data to be sent
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomUartSendData(uint8_t* pBuf, uint32_t len);

/*!
 * Get firmware version
 *
 * @param[out] ppBuf  address to be set of firmware version,
 *                        which is ASCII string formate, be sure that the
 *                        address of *ppBuf is a const char type which
 *                        will not be released
 * @param[out] pLen   length of ppBuf contents
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetFirmwareVersion(
                                        const char **ppBuf, uint8_t *pLen);

/*!
 * Get mainboard temperature
 *
 * @param[out] pTemperature   temperature value in float, unit centigrade
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetTemperature(float *pTemperature);

/*!
 * Get battery voltage
 *
 * @param[out] pBattery      battery voltage value in float, unit volt
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetBatteryVoltage(float *pBattery);

/*!
 * Get battery range
 *
 * @param pBatteryOut[out]    battery voltage which indicates used out
 * @param pBatteryFull[out]   battery voltage which indicates battery is full
 * @param pBatteryLow[out]    battery voltage low which will generate warning on mobile phone
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetBatteryRange(
                float *pBatteryOut,
                float *pBatteryFull,
                float *pBatteryLow);

/*!
 * Get max speed
 *
 * @param pMaxSpeed[out]    max speed of this electronic vehicle
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetMaxSpeed(float *pMaxSpeed);

/*!
 * Get current speed
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomGetSpeed(float *pSpeed);

/*!
 * Send buffer data via uart, to be implemented by customer
 *
 * @param[out] pBuf   buffer address contains formated data to be sent
 * @param[in] len    length of data to be sent
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomUartSendData(uint8_t* pBuf,
                uint32_t len);


/*!
 * set total mile, this function is received after target request mile data from
 * mobile phone, normally it is invoked after power up, the target has loss its
 * data, while it contains a backup on mobile phone
 *
 * @param[in] mile total mile counted
 *
 */
void electronicVehicleCustomSetMile(uint32_t mile);


#ifdef __cplusplus
}
#endif


#endif
