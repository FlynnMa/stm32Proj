#include <stdint.h>
#include <string.h>
#include "protocal.h"
#include "protocalApi.h"

static ProtocalStatusType     communicator;

/*
 * This function initialize protocal, and register platform functions
 *
 * @param[i]  sendFunc    send data function
 * @param[i]  onCmdFunc   response on command function
 * @param[i]  ackFunc     acknowledge function
 * @param[i]  waitFunc    wait function, recommend more than 10 milli-seconds
 *
 * @return @ERROR_TYPE
 */
int32_t protocalApiInit(pfnSendData sendFunc, pfnProtocalOnCmd onCmdFunc,
    pfnAck ackFunc, pfnWait waitFunc, pfnScheduleDispatch pScheduleFunc)
{
    int32_t ret = EV_SUCCESS;

    ret = protocalInit(&communicator);
    if (EV_SUCCESS != ret)
    {
        return ret;
    }

    protocalSetOnCmdCallback(&communicator, onCmdFunc);
    protocalSetSendDataFunc(&communicator, sendFunc);
    protocalSetWaitFunc(&communicator, waitFunc);
    protocalSetOnAckCallback(&communicator,ackFunc);
    protocalSetScheduleFunc(&communicator, pScheduleFunc);

    return ret;

}

/*!
 * This function set one byte data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetU8(uint8_t cmd, uint8_t data)
{
    return protocalSendCmd(&communicator, CMD_TYPE_SET,
                cmd,
                (void*)&data, sizeof(uint8_t));
}

/*!
 * This function set an unsigned 32bits data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetU32(uint8_t cmd, uint32_t data)
{
    return protocalSendCmd(&communicator, CMD_TYPE_SET,
                cmd,
                (void*)&data, sizeof(uint32_t));
}

/*!
 * This function set a float data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetFloat(uint8_t cmd, float data)
{
    return protocalSendCmd(&communicator, CMD_TYPE_SET,
                cmd,
                (void*)&data, sizeof(float));

}

/*!
 * This function set string data to remote side
 * Note: string length must be lessthan 24 bytes
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetStr(uint8_t cmd, const char* str)
{
    int32_t len;

    len = strlen(str);
    if (len > PROTOCAL_MAX_DATA_LENGTH)
    {
        len = PROTOCAL_MAX_DATA_LENGTH;
    }

    return protocalSendCmd(&communicator, CMD_TYPE_SET, cmd,
        (void *)str, (uint8_t)len);
}

/*!
 * This function set string data to remote side
 * Note: string length must be lessthan 24 bytes
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetBuff(uint8_t cmd,
                    const void* data, uint8_t len)
{
    return protocalSendCmd(&communicator, CMD_TYPE_SET, cmd,
        (const void *)data, (uint8_t)len);
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
int32_t protocalApiReceiveChar(uint8_t ch)
{
    return protocalUartReceiveChar(&communicator, ch);
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
int32_t protocalApiDispatchEvents(void)
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
uint8_t protocalApiIsConnected(void)
{
    return communicator.connectionStatus;
}

/*!
 * send a query command to remote target and wait until reply
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 */
int protocalApiQueryWait(uint8_t cmd, void *pData, uint8_t len)
{

    uint8_t copyLen;
    int ret;
    int count = 0;
    ProtocalStatusType *pProtocal = &communicator;

    while(pProtocal->waitingStat != PROTOCAL_WAITING_NONE)
    {
        if (pProtocal->pfnWait)
        {
            pProtocal->pfnWait();
            if (count++ > PROTOCAL_WAIT_TIMES)
            {
                protocalReset(pProtocal);
                return ERROR_TIME_OUT;
            }
        }
    }


    /* set waiting stat */
    pProtocal->waitingCmd = cmd;
    pProtocal->waitingStat |= PROTOCAL_WAITING_QUERY;

    /* send query command */
    ret = protocalSendCmdWait(pProtocal, CMD_TYPE_QUERY, cmd, NULL, 0);
    if (ret != EV_SUCCESS)
    {
        return ret;
    }

    count = 0;
    /* start waiting */
    while(pProtocal->waitingStat & PROTOCAL_WAITING_QUERY)
    {
        if (pProtocal->pfnWait)
        {
            pProtocal->pfnWait();
        }
        if (count++ > PROTOCAL_WAIT_TIMES)
        {
            protocalReset(pProtocal);
            return ERROR_TIME_OUT;
        }
    }

    copyLen = len < pProtocal->replyLen ? len : pProtocal->replyLen;
    memcpy(pData, pProtocal->buffer, copyLen);

    return EV_SUCCESS;
}

