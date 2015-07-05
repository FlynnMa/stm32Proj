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

const char deviceName[] = "Taotao Intelligent";

ElectronicVehicleDataType vehicle;
static ProtocalStatusType     communicator;

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

    ret = protocalInit(&communicator);
    if (EV_SUCCESS != ret)
    {
        return ret;
    }

    ret = electronicVehicleCustomInit();
    if (EV_SUCCESS != ret)
    {
      return ret;
    }

    protocalInit(&communicator);
    protocalSetOnCmdCallback(&communicator, electronicVehicleOnCmd);
    protocalSetSendDataFunc(&communicator, electronicVehicleCustomUartSendData);
    protocalSetWaitFunc(&communicator, electronicVehicleCustomWait);
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
    uint32_t  deviceID = ELECTRONIC_VEHICLE_DEVICE;
    const char *pVer;
    uint8_t size;
    float   fData;
    uint8_t byte;

    Trace("onCmdQuery: %d", cmd);
    switch(cmd)
    {

        case CMD_ID_DEVICE_ID:
            protocalSendCmd(&communicator, CMD_TYPE_SET,
                CMD_ID_DEVICE_ID,
                (void*)&deviceID, sizeof(deviceID));
            break;

        case CMD_ID_DEVICE_NAME:
            protocalSendCmd(&communicator, CMD_TYPE_SET,
                CMD_ID_DEVICE_NAME,
                (void*)deviceName, sizeof(deviceName));
            break;

        case CMD_ID_FIRMWARE_VERSION:
            electronicVehicleCustomGetFirmwareVersion(&pVer, &size);
            protocalSendCmd(&communicator, CMD_TYPE_SET,
                CMD_ID_FIRMWARE_VERSION,
                (void*)pVer, size);
            break;

        case CMD_ID_MAINBOARD_TEMPERITURE:
            electronicVehicleCustomGetTemperature(&fData);
            protocalSendCmd(&communicator, CMD_TYPE_SET,
                CMD_ID_MAINBOARD_TEMPERITURE,
                (void*)&fData, sizeof(float));
            break;

        case CMD_ID_SPEED:
            electronicVehicleCustomGetSpeed(&fData);
            protocalSendCmd(&communicator,CMD_TYPE_SET,
                CMD_ID_SPEED,
                (void*)&fData, sizeof(float));
            break;

        case CMD_ID_BATTERY_VOLTAGE:
            electronicVehicleCustomGetBatteryVoltage(&fData);
            protocalSendCmd(&communicator,CMD_TYPE_SET,
                CMD_ID_BATTERY_VOLTAGE,
                (void*)&fData, sizeof(float));
            break;

        case CMD_ID_CHARGE_STATUS:
            electronicVehicleCustomGetChargeStatus(&byte);
            protocalSendCmd(&communicator, CMD_TYPE_SET,
                CMD_ID_CHARGE_STATUS,
                (void*)&byte, 1);
            break;

        case CMD_ID_MILE:
            break;

        default:
            break;
    }

    return EV_SUCCESS;
}

int32_t onCmdSet(uint8_t cmd, uint8_t *pData, uint8_t len)
{
    Trace("onCmdSet: %d", cmd);
    switch(cmd)
    {
        case CMD_ID_MILE:
            break;

        case CMD_ID_RESET_TARGET:
            electronicVehicleCustomEvents(EV_EVENT_BLUETOOTH_CONNECTION,
                (uint32_t)(*pData));
            break;

        default:
            break;
    }
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
    int32_t ret = EV_SUCCESS;

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
void electronicVehicleOnChar(uint8_t ch)
{
    protocalUartReceiveChar(&communicator, ch);
}

/*
 * @brief This function dispatch received events to customer applications
 * Notice, this function must be invoked in an individual thread, make sure it
 * wont be blocked by anything!
 *
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t electronicVehicleDispatchEvents(void)
{
    return protocalDispatchEvents(&communicator);
}

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
uint8_t electronicVehicleIsConnected(void)
{
    return protocalIsConnected(&communicator);
}

