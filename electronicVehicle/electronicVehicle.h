#ifndef electronicVehicle_h
#define electronicVehicle_h
#include <stdint.h>

#define REPORT_BATTERY_DURATION               2000 // unit ms

#define REPORT_SPEED_DURATION                 200 //unit ms

typedef struct ElectronicVehicleDataType {

    float       maxSpeed;           /**< Max speed set */

    float       maxVoltage;

    float       minVoltage;

    float       lowVoltage;

    const char  *pFirmwareVersion;

    uint32_t     firmwareVersionLen;

    uint32_t     timerBattery;   /* calculate timer duration
                                  for reporting battery */
    uint32_t     timerSpeed;     /* calculate timer duration for report speed */

    uint32_t     intervalBattery;

    uint32_t     intervalSpeed;
} ElectronicVehicleDataType;

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
void electronicVehicleSchedule(uint32_t duration, void *pData);

int32_t electronicVehicleOnCmd(uint8_t cmd, uint8_t *pData, uint32_t len);

void electronicVehicleSetBatteryInterval(uint32_t interval);

void electronicVehicleSetSpeedInterval(uint32_t interval);

/*!
 * This function response characters when data is received
 *
 * @param[in] ch
 *
 * @return none
 */
void electronicVehicleOnChar(uint8_t ch);

#endif

