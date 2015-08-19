#ifndef protocalCustomWin32_h
#define protocalCustomWin32_h
#include <stdint.h>

#define SOFTWARE_VERSION_MAJOR                       0

#define SOFTWARE_VERSION_SUB                         1

#define SOFTWARE_VERSION_MODIFY                      0

#define SOFTWARE_VERSION_YEAR                        15

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
    float current;

    uint32_t scheduleDuration;  /**< schedule duration for the protocal in milli-seconds, default 100ms */
    uint32_t batteryInterval;
    uint8_t  isCharging;

    uint32_t mile;

    int      isPowerOn;  /* is device power on or off */
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
        uint32_t *pVer);

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
 * Get electronic current
 *
 * @param pCurrent[out]    battery realtime current
 *
 * @return @ERROR_TYPE
 */
int32_t elecronicVehicleCustomGetCurrent(float *pCurrent);

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

/*!
 * Get total mile, this function is invoked on the request of mile data by
 * mobile phone
 *
 * @param[i] pMile  total mile counted by embeded end
 */
void electronicVehicleCustomGetMile(uint32_t *pMile);

#ifdef __cplusplus
}
#endif


#endif
