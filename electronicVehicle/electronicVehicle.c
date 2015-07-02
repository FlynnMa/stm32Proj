#include "protocal.h"
#include "electronicVehicle.h"
#include "protocalApi.h"
#ifdef _WINDOWS
#include "simulator.h"
#endif

ElectronicVehicleDataType vehicle;
static ProtocalStatusType     communicator;

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
    int32_t ret = SUCCESS;

    ret = electronicVehicleCustomInit();
    if (SUCCESS != ret)
    {
      return ret;
    }

    ret = protocalInit(&communicator);
    if (SUCCESS != ret)
    {
        return ret;
    }

    ret = protocalCustomGetBatteryRange(&vehicle.minVoltage,
              &vehicle.maxVoltage, &vehicle.lowVoltage);
    if (SUCCESS != ret)
    {
      return ret;
    }

    ret = protocalCustomGetFirmwareVersion(&vehicle.pFirmwareVersion,
      &vehicle.firmwareVersionLen);
    if (SUCCESS != ret)
    {
      return ret;
    }

    ret = protocalCustomGetMaxSpeed(&vehicle.maxSpeed);
    if (SUCCESS != ret)
    {
      return ret;
    }

    protocalInit(&communicator);
    protocalSetOnCmdCallback(&communicator, electronicVehicleOnCmd);
    protocalSetSendDataFunc(&communicator, electronicVehicleCustomUartSendData);

    return ret;
}

int32_t electronicVehicleOnCmd(uint8_t cmd, uint8_t *pData, uint32_t len)
{
    int32_t ret = SUCCESS;

    switch(cmd)
    {
        case CMD_ID_QUERY_DEVICE_ID:
            break;

        case CMD_ID_QUERY_DEVICE_NAME:
            break;

        case CMD_ID_QUERY_FIRMWARE_VERSION:
            break;

        case CMD_ID_QUERY_MAINBOARD_TEMPERITURE:
            break;

        case CMD_ID_QUERY_SPEED:
            break;

        case CMD_ID_QUERY_BATTERY_VOLTAGE:
            break;

        case CMD_ID_QUERY_CHARGE_STATUS:
            break;

        case CMD_ID_SET_REPORT_DURATION:
            if (len != 4)
                return ERROR_INVALID_PARAM;
//            vehicle.reportDuration = *(uint32_t*)pData;
            break;

        case CMD_ID_SET_BLUETOOTH_CONNECTION:
//            vehicle.connectionStatus = *pData;
//            protocalCustomEvents(EV_EVENT_BLUETOOTH_CONNECTION,
//                vehicle.connectionStatus);
            break;

        case CMD_ID_SET_UART_HARDWARE_FLOW:
            break;

        case CMD_ID_RESET_STATUS:
//            vehicle.protocalStatus = PROTOCAL_STATUS_CMD;
//            vehicle.cmd = 255;
            break;

        case CMD_ID_SET_BATTERY_INTERVAL:
            memcpy(&vehicle.intervalBattery, pData, sizeof(vehicle.intervalBattery));
            break;

       default:
        break;
    }

    return ret;
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
    if(result == SUCCESS)
    {
        return;
    }

    switch(cmd)
    {
        // ERROR_UNSUPPORTED : unsupported device
        case CMD_ID_REPORT_DEVICE_ID:
            break;

        // ERROR_UNSUPPORTED : version is not matching for this apk
        case CMD_ID_REPORT_FIRMWARE_VERSION:
            break;

        default:
            break;
    }
}

/*!
 * Protocal schedule for doing regular analysis and reporting
 *
 * @param[in] duration duration in ms since last report
 *
 * @return none
 */
void electronicVehicleSchedule(uint32_t duration, void *pData)
{
    float speed;
    float batteryVoltage;
    int32_t ret;

    vehicle.timerBattery += duration;
    if (vehicle.timerBattery >= vehicle.intervalBattery)
    {
        ret = protocalCustomGetBatteryVoltage(&batteryVoltage);
        if (ret != SUCCESS)
        {
            return;
        }

        vehicle.timerBattery = 0;
        protocalSendCmd(&communicator, CMD_ID_REPORT_BATTERY_VOLTAGE,
            (void*)&batteryVoltage, sizeof(batteryVoltage));
    }

    vehicle.timerSpeed += duration;
    if (vehicle.timerSpeed >= vehicle.intervalSpeed)
    {
        ret = protocalCustomGetSpeed(&speed);
        if (ret != SUCCESS)
        {
            return;
        }

        vehicle.timerSpeed = 0;
        protocalSendCmd(&communicator, CMD_ID_REPORT_CURRENT_SPEED,
            (void*)&speed, sizeof(speed));
    }

}

void electronicVehicleSetBatteryInterval(uint32_t interval)
{
    vehicle.intervalBattery = interval;
}


void electronicVehicleSetSpeedInterval(uint32_t interval)
{
    vehicle.intervalSpeed = interval;
}

/*!
 * This function response characters when data is received
 *
 * @param[in] ch
 *
 * @return none
 */
void electronicVehicleOnChar(uint8_t ch)
{
    protocalUartReceiveChar(&communicator, ch);
}

