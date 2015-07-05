#ifndef electronicVehicle_h
#define electronicVehicle_h
#include <stdint.h>

#define CMD_ID_DEVICE_ID                      0

#define CMD_ID_DEVICE_NAME                    1

#define CMD_ID_FIRMWARE_VERSION               2

#define CMD_ID_MAINBOARD_TEMPERITURE          3

#define CMD_ID_BATTERY_VOLTAGE                4

#define CMD_ID_CHARGE_STATUS                  5

#define CMD_ID_SPEED                          6

#define CMD_ID_MILE                           7

#define CMD_ID_MAX_SPEED                      8

#define CMD_ID_LOW_BATTERY                    9

#define CMD_ID_SHUTDOWN_BATTERY               10

#define CMD_ID_FULL_BATTERY                   11


typedef struct ElectronicVehicleDataType {

    uint32_t     timerBattery;   /* calculate timer duration
                                  for reporting battery */
    uint32_t     timerSpeed;     /* calculate timer duration for report speed */

    uint32_t     intervalBattery;

    uint32_t     intervalSpeed;

} ElectronicVehicleDataType;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief initialize for electronic Vehicle devices
 *
 * @param none
 *
 * @return @ERROR_TYPE
 *
 */
int32_t electronicVehicleInitialize(void);

/*!
 * Protocal schedule for doing regular analysis and reporting
 *
 * @param[in] duration duration in ms since last report
 *
 * @return none
 */
void electronicVehicleSchedule(uint32_t duration);

/*!
 * Response on the remote commands
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 * @todo customer decide the ack result according with the process, protocal
 *          will judge the return value to do the ack
 */
int32_t electronicVehicleOnCmd(uint8_t cmdType, uint8_t cmd,
                    uint8_t *pData, uint8_t len);

/*!
 * This function response characters when data is received, data will be
 * transferred into low level protocal for further analysis.
 * Notice: this function must be invoked in an individual thread.
 * Make sure it will not be blocked by other tasks
 *
 * @param[in] ch
 *
 * @return none
 */
void electronicVehicleOnChar(uint8_t ch);

/*
 * @brief This function dispatch received events to customer applications
 * Notice, this function must be invoked in an individual thread, make sure it
 * wont be blocked by anything!
 *
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleDispatchEvents(void);

/*
 * This function returns the device connection status
 * On the startup, if bluetooth connection has been established, a handshake
 * between BT and mobile phone will be executed, if success, the connection
 * is established
 *
 * @param none
 *
 * @return
 * 0: disconnected
 * 1: connected
 */
uint8_t electronicVehicleIsConnected(void);

#ifdef __cplusplus
}
#endif

#endif

