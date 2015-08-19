#include <string.h>
#include "protocal.h"
#include "electronicVehicle.h"
#include "protocalApi.h"
#ifdef _WINDOWS
#include "simulator.h"
#endif
#include "electronicVehicleCustom.h"
#include "Trace.h"

#define ELECTRONIC_VEHICLE_DEVICE_ID_TAOTAO    0x10001000 /* device id for
TAOTAO */

#define ELECTRONIC_VEHICLE_DEVICE ELECTRONIC_VEHICLE_DEVICE_ID_TAOTAO

const char deviceName[] = "Yeth Intelligent";

extern void electronicVehicleCustomWait(void);
/*
 * @brief initialize for electronic Vehicle devices
 *
 * @param none
 *
 * @return @ERROR_TYPE
 *
 */
int32_t electronicVehicleInitialize(void)
{
    int32_t ret = EV_SUCCESS;

    ret = electronicVehicleCustomInit();
    if (EV_SUCCESS != ret)
    {
      return ret;
    }

    ret = protocalApiInit(electronicVehicleCustomUartSendData,
            electronicVehicleOnCmd, NULL, electronicVehicleCustomWait, NULL);

    return ret;
}

/*! Response on the query commands
 *
 * @param[in] cmd     command id
 * @param[in] pData   data according with command id
 * @param[in] len     length of pData
 *
 * @return @ERROR_TYPE
 */
int32_t onCmdQuery(uint8_t cmd, uint8_t *pData, uint8_t len)
{
    uint32_t  deviceID = ELECTRONIC_VEHICLE_DEVICE, u32Data;
    GeneralInfoL infoL;
    GeneralInfoS infoS;
    float   fData;
    uint8_t byte;

//    Trace("onCmdQuery: %d", cmd);
    switch(cmd)
    {

        case CMD_ID_DEVICE_ID:
            protocalApiSetU32(cmd, deviceID);
            break;

        case CMD_ID_DEVICE_NAME:
            protocalApiSetStr(cmd, deviceName);
            break;

        case CMD_ID_FIRMWARE_VERSION:
            electronicVehicleCustomGetFirmwareVersion(&u32Data);
            protocalApiSetU32(cmd, u32Data);
            break;

        case CMD_ID_MAINBOARD_TEMPERITURE:
            electronicVehicleCustomGetTemperature(&fData);
            protocalApiSetFloat(cmd, fData);
            break;

        case CMD_ID_SPEED:
            electronicVehicleCustomGetSpeed(&fData);
            protocalApiSetFloat(cmd, fData);
            break;

        case CMD_ID_BATTERY_VOLTAGE:
            electronicVehicleCustomGetBatteryVoltage(&fData);
            protocalApiSetFloat(cmd, fData);
            break;

        case CMD_ID_CHARGE_STATUS:
            electronicVehicleCustomGetChargeStatus(&byte);
            protocalApiSetU8(cmd, byte);
            break;

        case CMD_ID_FULL_BATTERY:
            electronicVeichleCustomGetBatteryRange(NULL,&fData,NULL);
            protocalApiSetFloat(cmd, fData);
            break;

        case CMD_ID_MILE:
            electronicVehicleCustomGetMile(&u32Data);
            protocalApiSetU32(cmd, u32Data);
            break;

        case CMD_ID_POWER_ONOFF:
            protocalApiSetU8(cmd, 1);
            break;

        case CMD_ID_GENERAL_LONG:
            electronicVehicleCustomGetBatteryVoltage(&infoL.battery);
            electronicVehicleCustomGetTemperature(&infoL.temperature);
            infoL.incharge = 0;
            protocalApiSetBuff(cmd, &infoL, sizeof(infoL));
            break;

        case CMD_ID_GENERAL_SHORT:
            electronicVehicleCustomGetSpeed(&infoS.speed);
            electronicVehicleCustomGetMile(&infoS.mile);
            elecronicVehicleCustomGetCurrent(&infoS.current);
            protocalApiSetBuff(cmd, &infoS, sizeof(infoS));
            break;
        default:
            break;
    }

    return EV_SUCCESS;
}

int32_t onCmdSet(uint8_t cmd, uint8_t *pData, uint8_t len)
{
//    Trace("onCmdSet: %d", cmd);
    switch(cmd)
    {
        case CMD_ID_MILE:
            electronicVehicleCustomSetMile(*(uint32_t*)pData);
            break;

        case CMD_ID_RESET_TARGET:
            break;

        default:
            break;
    }

    return 0;
}

/*!
 * Response on the remote commands
 *
 * @param pSpeed[out] get current speed
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleOnCmd(uint8_t cmdType,
            uint8_t cmd, uint8_t *pData, uint8_t len)
{

    switch(cmdType)
    {
        case CMD_TYPE_QUERY:
            onCmdQuery(cmd, pData, len);
            break;

        case CMD_TYPE_SET:
            onCmdSet(cmd, pData, len);
            break;

        case CMD_TYPE_ACK:
            break;

        default:
            break;
    }

    return EV_SUCCESS;
}

/*
 * on the response of acknowledgement
 *
 * @param[in] cmd     command id
 * @param[in] result  result of the command processing
 *
 * @return none
 */
void electronicVehicleOnAck(uint8_t cmd, uint8_t result)
{
    if(result == EV_SUCCESS)
    {
        return;
    }
}


