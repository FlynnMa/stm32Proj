#ifndef electronicVehicle_h
#define electronicVehicle_h
#include <stdint.h>


/*
 * ========================================================
 * custom command ids supported by onelution electronic vehicle protocal
 * each command should follow its formate definition, and the data must
 * strictly limited in the length range
 * =======================================================
*/
#define CMD_ID_DEVICE_ID                      0 /**< 4byte device ID
                                                        in binary formate */

#define CMD_ID_DEVICE_NAME                    1 /**< less than 24bytes
                                                    device name in ASCII formate */

#define CMD_ID_FIRMWARE_VERSION               2 /**< 4byte firmware version
                                                    in binary formate :
                                                    first:  major
                                                    second: min
                                                    third:  modify
                                                    fourth: reserved */

#define CMD_ID_MAINBOARD_TEMPERITURE          3 /**< 4byte temperiture float
                                                    in binary formate */

#define CMD_ID_BATTERY_VOLTAGE                4 /**< 4bytes voltage float
                                                    in binary formate */

#define CMD_ID_CHARGE_STATUS                  5 /**< 1byte charging status
                                                      0 : no charging
                                                      1 : charging */

#define CMD_ID_SPEED                          6 /**< 4bytes speed float in
                                                    binary formate */

#define CMD_ID_MILE                           7 /**< 4bytes unsigned int */

#define CMD_ID_MAX_SPEED                      8 /**< 4bytes unsigned int */

#define CMD_ID_LOW_BATTERY                    9 /**< 4bytes float in binary */

#define CMD_ID_SHUTDOWN_BATTERY               10 /**< 4bytes float in binary */

#define CMD_ID_FULL_BATTERY                   11 /**< 4bytes float in binary */

#define CMD_ID_POWER_ONOFF                    12 /**< 1 byte power onoff
                                                            status */

#define CMD_ID_DRIVE_MODE                     13 /**< 1 byte drive mode */

#define CMD_ID_CURRENT                        14 /**< 4 bytes int32 value in mA*/

#define CMD_ID_PERIOD_LONG                    15 /**< 4 bytes int32 value in ms
                                      this defines update rate of long period
                                      range 1000 ~ 10000 */

#define CMD_ID_PERIOD_SHORT                    16 /**< 4 bytes int32 value in ms
                                      this defines update rate of short period
                                      range 200 ~ 1000 */

#define CMD_ID_GENERAL_LONG            17  /**< a combined command, contains
                                      battery, temperature, charge status */

#define CMD_ID_GENERAL_SHORT           18 /**< a combined command queried in short
                                      duration, contains speed, current, mile */



typedef struct GeneralInfoLongType {
    float    battery;
    float    temperature;
    int32_t  incharge;
} GeneralInfoL;

typedef struct GeneralInfoShortType {
    uint32_t mile;
    float speed;
    float    current;
} GeneralInfoS;

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



#ifdef __cplusplus
}
#endif

#endif

