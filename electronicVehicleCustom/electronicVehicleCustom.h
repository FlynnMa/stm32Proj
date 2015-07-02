#ifndef protocalCustomWin32_h
#define protocalCustomWin32_h
#include <stdint.h>

/*! default protocal schedual duration */
#define PROTOCAL_CUSTOM_SCHEDULE_DURATION_DEFAULT    100


typedef struct ProtocalCustomDataType{
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
} ProtocalCustomDataType;

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
int32_t protocalCustomStartSchedule(void);

/*!
* Stop schedule task for protocal
*/
int32_t protocalCustomStopSchedule(void);

/*!
 * Send buffer data via uart, to be implemented by customer
 *
 * @param[out] pBuf   buffer address contains formated data to be sent
 * @param[in] len    length of data to be sent
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleCustomUartSendData(uint8_t* pBuf, uint32_t len);


#endif
