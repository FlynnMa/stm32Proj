#ifndef protocalApi_h
#define protocalApi_h
#include <stdint.h>

#define CMD_ID_QUERY_DEVICE_ID                0

#define CMD_ID_QUERY_DEVICE_NAME              1

#define CMD_ID_QUERY_FIRMWARE_VERSION         2

#define CMD_ID_QUERY_MAINBOARD_TEMPERITURE    3

#define CMD_ID_QUERY_BATTERY_VOLTAGE          4

#define CMD_ID_QUERY_CHARGE_STATUS            5

#define CMD_ID_QUERY_SPEED                    6

#define CMD_ID_REPORT_DEVICE_ID               20

#define CMD_ID_REPORT_DEVICE_NAME             21

#define CMD_ID_REPORT_FIRMWARE_VERSION        22

#define CMD_ID_REPORT_MAINBOARD_TEMPERITURE   23

#define CMD_ID_REPORT_BATTERY_VOLTAGE         24

#define CMD_ID_REPORT_CHARGE_STATUS           25

#define CMD_ID_REPORT_CURRENT_SPEED           26

#define CMD_ID_REPORT_BATTERY_RANGE           27

#define CMD_ID_REPORT_MAX_SPEED               28

#define CMD_ID_SET_REPORT_DURATION            40

#define CMD_ID_SET_UART_HARDWARE_FLOW         41 /* 201 */

#define CMD_ID_SET_BLUETOOTH_CONNECTION       42 /* 202, 2, enable/disable, checkSum */

#define CMD_ID_RESET_STATUS                   43 /* 203, 1, checksum*/

#define CMD_ID_SET_BATTERY_INTERVAL           44

#define CMD_ID_SET_SPEED_INTERVAL             45

#define ACK                                   101 /**< cmd + result,
                                                        OK  : result == 0
                                                        NOK : result > 0 */

/*!
 * Electronic Vehicle events supported
 */
typedef enum  {
    EV_EVENT_BLUETOOTH_CONNECTION, /**< bluetooth connection status changed */
    EV_EVENT_QUERY,
    EV_EVENT_REPLY,
    EV_EVENT_SETTINGS,
    EV_EVENT_MAX
} EV_EVENT_TYPE;

/*!
 * error status
 */
enum ERROR_TYPE{
    SUCCESS,
    ERROR_HW_FAILURE,
    ERROR_UNSUPPORTED,
    ERROR_INVALID_PARAM,
    ERROR_BAD_STATUS,
    ERROR_CHECKSUM,
    ERROR_OUT_RANGE,
    ERROR_MAX
} ;

/*!
 * Protocal schedule for doing regular analysis and reporting
 *
 * @param[in] duration duration in ms since last report
 *
 * @return none
 */
typedef void (*pfnProtocalSchedule)(uint32_t duration, void *pData);

/*!
 * protocal events response, to be implemented by customer
 *
 * @param[in] event event to be responsed, relate to @EV_EVENT_TYPE
 * @param[in] data  data corresponding with event
 *
 * @return none
 */
void protocalCustomEvents(EV_EVENT_TYPE event, uint32_t data);

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
 *                        which is ASCII string formate, be sure that the address of *ppBuf
 *                        is a const char type which will not be released
 * @param[out] pLen   length of ppBuf contents
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetFirmwareVersion(const char **ppBuf, uint32_t *pLen);

/*!
 * Get mainboard temperature
 *
 * @param[out] pTemperature   temperature value in float, unit centigrade
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetTemperature(float *pTemperature);

/*!
 * Get battery voltage
 *
 * @param[out] pBattery      battery voltage value in float, unit volt
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetBatteryVoltage(float *pBattery);

/*!
 * Get battery range
 *
 * @param pBatteryOut[out]    battery voltage which indicates used out
 * @param pBatteryFull[out]   battery voltage which indicates battery is full
 * @param pBatteryLow[out]    battery voltage low which will generate warning on mobile phone
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetBatteryRange(float *pBatteryOut, float *pBatteryFull, float *pBatteryLow);

/*!
 * Get max speed
 *
 * @param pMaxSpeed[out]    max speed of this electronic vehicle
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetMaxSpeed(float *pMaxSpeed);

/*!
 * Get current speed
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 */
int32_t protocalCustomGetSpeed(float *pSpeed);

/*!
 * Set a buffer to the protocal,
 *¡¡which will be used for running algorithm and bufferring area
 *
 *
 * @param ppBuffer[out]   set a buffer for protocal use, minimum 128 bytes
 *
 * @return none
 */
void protocalCustomGetBuffer(char **ppBuffer, uint32_t *pLen);

/*!
 * Schedule the protocal in sepcified duration in ms, protocal will use it
 * for regular data analysis and reporting

*

 * @param[in] duration  duration duration in milli-seconds
 * @param[in] cpfnScheduleCB allback function of scheduler,
 *           if pfnScheduleCB == NULL, meanwhile duration is 0, means cancel the schedule
 *
 * @return @ERROR_TYPE
 */
int protocalCustomSchedule(uint32_t duration, pfnProtocalSchedule pfnScheduleCB);

/*!
 * Initialize protocal

*
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t protocalInit(void);

/*!
 * Response for receiving character, and input to protocal analysis
 *
 * @param[in] ch char input into protocal
 *
 * @return @ERROR_TYPE
 */
int32_t protocalUartReceiveChar(uint8_t ch);

#endif
