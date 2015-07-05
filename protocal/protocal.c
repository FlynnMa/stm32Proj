#include <stdlib.h>
#include <string.h>
#include "protocalTypes.h"
#include "protocal.h"
#include "protocalApi.h"
#include "trace.h"

/*!
 * Initialize protocal

*
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t protocalInit(ProtocalStatusType *pProtocal)
{
    memset(pProtocal, 0, sizeof(ProtocalStatusType));
    pProtocal->bufferIndex = 0;
    pProtocal->connectionStatus = 0;
    pProtocal->checkSum = 0;
    pProtocal->cmd = CMD_ID_INVALID;
    return 0;
}

/*!
 * @brief make data package
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return none
 */
void protocalMakePackage(ProtocalStatusType *pProtocal, uint8_t cmdType,
        uint8_t cmd, void *pData, uint8_t lengthOfData)
{
    uint8_t checkSum = 0;
    uint8_t i;

    pProtocal->package[0] = cmdType;
    pProtocal->package[1] = lengthOfData + 2;
    pProtocal->package[2] = cmd;
    if (NULL != pData)
    {
        memcpy(&pProtocal->package[3], pData, lengthOfData);
    }

    for(i = 0; i < lengthOfData + 3; i++)
    {
        checkSum += pProtocal->package[i];
    }
    pProtocal->package[lengthOfData + 3] = checkSum;
    pProtocal->packageSize = lengthOfData + 4;
}

void protocalAck(ProtocalStatusType *pProtocal, uint8_t result)
{
/*
    uint8_t buf[5];
    uint8_t i;

    buf[0] = CMD_TYPE_ACK;
    buf[1] = 2;
    buf[2] = pProtocal->cmd;
    buf[3] = result;
    buf[4] = 0;
    for (i = 0; i < 4; i++)
    {
        buf[4] += buf[i];
    }*/
    protocalMakePackage(pProtocal, CMD_TYPE_ACK, pProtocal->cmd, &result, 1);
    if (NULL != pProtocal->pfnSendData)
    {
        pProtocal->pfnSendData(pProtocal->package,
            pProtocal->packageSize);
    }
}

/*!
 * @brief send command to host or slave
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return @ERROR_TYPE
 */
int protocalSendCmd(ProtocalStatusType *pProtocal, uint8_t cmdType,
    uint8_t cmd, void *pData, uint8_t lengthOfData)
{
    protocalMakePackage(pProtocal, cmdType, cmd, pData, lengthOfData);
    if (pProtocal->pfnSendData)
    {
        pProtocal->pfnSendData(pProtocal->package, pProtocal->packageSize);
    }

    return EV_SUCCESS;
}

/*!
 * @brief send command to host or slave, and wait until the response
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return @ERROR_TYPE
 */
int32_t protocalSendCmdWait(ProtocalStatusType *pProtocal, uint8_t cmdType,
    uint8_t cmd, void *pData, uint8_t lengthOfData)
{
    protocalMakePackage(pProtocal, cmdType, cmd, pData, lengthOfData);
    if (pProtocal->pfnSendData)
    {
        pProtocal->waitingStat |= PROTOCAL_WAITING_ACK;
        pProtocal->pfnSendData(pProtocal->package, pProtocal->packageSize);
    }

    if (NULL == pProtocal->pfnWait)
    {
        return EV_SUCCESS;
    }

    while(pProtocal->waitingStat & PROTOCAL_WAITING_ACK)
    {
        if (pProtocal->pfnWait != NULL)
            pProtocal->pfnWait();
    }

    return (int)pProtocal->buffer[1];
}

/*
 * Send a command to slave & get the status or data
 *
 * @param[in] cmd command id to be queried
 *
 * @return none
 *
 */
void protocalQuery(ProtocalStatusType *pProtocal, uint8_t cmd)
{
    protocalSendCmd(pProtocal, CMD_TYPE_QUERY, cmd, NULL, 0);
}

/*!
 * send a query command to remote target and wait until reply
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 */
int protocalQueryWait(ProtocalStatusType *pProtocal, uint8_t cmd,
    void *pData, uint8_t len)
{

    uint8_t copyLen;
    int ret;

    while(pProtocal->waitingStat != PROTOCAL_WAITING_NONE)
    {
        if (pProtocal->pfnWait)
        {
            pProtocal->pfnWait();
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

    /* start waiting */
    while(pProtocal->waitingStat & PROTOCAL_WAITING_QUERY)
    {
        if (pProtocal->pfnWait)
        {
            pProtocal->pfnWait();
        }
    }

    copyLen = len < pProtocal->replyLen ? len : pProtocal->replyLen;
    memcpy(pData, pProtocal->buffer, copyLen);

    return EV_SUCCESS;
}

/*!
 * Set a callback function for responsing commands
 *
 * @param[in] pfnOnCmd
 *
 */
void protocalSetOnCmdCallback(ProtocalStatusType *pProtocal, pfnProtocalOnCmd pfnOnCmd)
{
    pProtocal->pfnOnCmd = pfnOnCmd;
}

/*!
 * set a callback function on responsing acknlodgement
 *
 * @param[in] pProtocal instance of protocal
 * @param[in] ackFunc   acknowledge callback function
 *
 * @return none
 */
void protocalSetOnAckCallback(ProtocalStatusType *pProtocal,
 pfnAck ackFunc)
{
    pProtocal->pfnAck = ackFunc;
}

/*!
 * Set a callback function on waiting response
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 * @param[in] waitFunc   wait callback function
 *
 * @return none
 */
void protocalSetWaitFunc(ProtocalStatusType *pProtocal,
    pfnWait waitFunc)
{
    pProtocal->pfnWait = waitFunc;
}

/*!
 * Set a callback function for sending data
 *
 * @param[in] pfnOnCmd
 *
 * @return none
 */
void protocalSetSendDataFunc(ProtocalStatusType *pProtocal, pfnSendData pfnSend)
{
    pProtocal->pfnSendData = pfnSend;
}

/*
 * @brief Do the protocal level command handler
 *
 * @param[i] pProtocal  instance of ProtocalStatusType
 * @param[i] cmdType    command type
 * @param[i] cmd        command id
 * @param[i] pData      data to be handled
 * @param[i] len        length of pData context
 *
 * @return
 *  0 : continue process
 *  1 : this is an internal command, no needer further process
 */
uint32_t protocalPreprocessCmd(ProtocalStatusType *pProtocal,
    uint8_t cmdType, uint8_t cmd, uint8_t *pData, uint8_t len)
{
    uint32_t ret = 0;

    if (cmdType == CMD_TYPE_ACK)
    {
        return ret;
    }

    switch(cmd)
    {
        case CMD_ID_RESET_TARGET:
            protocalReset(pProtocal);
            break;

        case CMD_ID_DEVICE_CONNECTION:
            pProtocal->connectionStatus = *pData;            ret = 1;
            break;

        default:
            break;
    }

    return ret;
}

/*!
 * This function alloc memory for pushing asynchoronous events, the reason why
 * pushing events to be handled asynchoronous is because we don't want the process
 * will impact the input of characters. The asynchoronous mechanism allows
 * whatever the process doing, the receiving part will never be impacted
 *
 * @param[i] pProtocal instance of @ProtocalStatusType
 * @param[o] ppEvent   event memory allocated
 *
 * @return @ERROR_TYPE
 */
static int32_t protocalAllocEmptyEventMemory(
        ProtocalStatusType *pProtocal, EventStackType **ppEvent)
{
    EventStackType *pstack = pProtocal->eventPool;
    int i;

    for (i = 0; i < PROTOCAL_MAX_EVENT_NUM; i++)
    {
        if (pstack[i].cmdType == 0)
        {
            break;
        }
    }

    if (i >= PROTOCAL_MAX_EVENT_NUM)
    {
        return ERROR_NO_RESOURCE;
    }

    pstack[i].cmdType |= 0x80;
    *ppEvent = &pstack[i];
    return EV_SUCCESS;
}

/*!
 * Response for receiving character, and input to protocal analysis
 *
 * @param[in] ch character input into protocal
 *
 * @return @ERROR_TYPE
 */
int32_t protocalUartReceiveChar(ProtocalStatusType *pProtocal, uint8_t ch)
{
    int32_t ret = EV_SUCCESS;
    EventStackType *pEvent;
    int copyLen;
    uint32_t stop;

    Trace("status:%d, data:%d", pProtocal->protocalStatus, ch);
    switch(pProtocal->protocalStatus)
    {
        case PROTOCAL_STATUS_CMD_TYPE:
            pProtocal->cmdType = ch;
            /* set command type and update status */
            pProtocal->checkSum = 0;
            pProtocal->protocalStatus = PROTOCAL_STATUS_LENGTH;
            pProtocal->bufferIndex = 0;
            pProtocal->cmd = CMD_ID_INVALID;
            break;

        case PROTOCAL_STATUS_LENGTH:
            /* set buffer length and update status */
            pProtocal->cmdSize = ch;
            pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
            break;

        case PROTOCAL_STATUS_CMD:
            /* set command type and update status */
            pProtocal->cmd = ch;
            pProtocal->protocalStatus = PROTOCAL_STATUS_DATA;
            if (pProtocal->cmdSize == 2) /* if no data */
            {
                pProtocal->protocalStatus = PROTOCAL_STATUS_CHECKSUM;
            }
            break;

        case PROTOCAL_STATUS_DATA:
            /* fill buffer */
            pProtocal->buffer[pProtocal->bufferIndex++] = ch;

            /* command size contain cmd(1byte) + nData + chksum(1byte) */
            if ((pProtocal->bufferIndex + 2) == pProtocal->cmdSize)
            {
                pProtocal->protocalStatus = PROTOCAL_STATUS_CHECKSUM;
            }
            break;

          case PROTOCAL_STATUS_CHECKSUM:
            Trace("========");
            pProtocal->protocalStatus = PROTOCAL_STATUS_CMD_TYPE;

            /* Get checksum*/
            pProtocal->checkSumRecv = ch;
            /* verify checkSum */
            if (pProtocal->checkSumRecv != pProtocal->checkSum)
            {
                protocalReset(pProtocal);
                return ERROR_CHECKSUM;
            }

            if (pProtocal->cmdType != CMD_TYPE_ACK)
            {
                protocalAck(pProtocal, (uint8_t)ret);
            }

            stop = protocalPreprocessCmd(pProtocal, pProtocal->cmdType,
                pProtocal->cmd, pProtocal->buffer, pProtocal->bufferIndex);
            if (stop)
            {
                Trace("return after preprocess:%d", stop);
                return ret;
            }
            /* if customer is waiting the answer, return immediately */
            if ((pProtocal->waitingStat & PROTOCAL_WAITING_ACK) &&
                (pProtocal->cmdType == CMD_TYPE_ACK))
            {
                Trace("break on waiting ack");
                pProtocal->waitingStat &= (~PROTOCAL_WAITING_ACK);
                protocalReset(pProtocal);
                break;
            }
            else if ((pProtocal->waitingStat & PROTOCAL_WAITING_QUERY) &&
                      (pProtocal->cmdType == CMD_TYPE_SET))
            {
                Trace("break on waiting query");
                pProtocal->waitingStat &= (~PROTOCAL_WAITING_QUERY);
                pProtocal->replyLen = pProtocal->bufferIndex;
                protocalReset(pProtocal);
                break;
            }
            else if (pProtocal->waitingStat)
            {
                Trace("waitiing state! %x, on:%d", pProtocal->waitingStat,
                    pProtocal->cmd);
//                break;
            }
            ret = protocalAllocEmptyEventMemory(pProtocal, &pEvent);
            if (ret)
            {
                return ret;
            }
            pEvent->cmd = pProtocal->cmd;
            pEvent->cmdType |= pProtocal->cmdType;
            pEvent->buffSize = pProtocal->bufferIndex;
            copyLen = pProtocal->bufferIndex < PROTOCAL_MAX_DATA_LENGTH ?
                      pProtocal->bufferIndex : PROTOCAL_MAX_DATA_LENGTH;
            memcpy(pEvent->buff, pProtocal->buffer, copyLen);
            ret = listAdd(&pProtocal->eventStack, (uint32_t)pEvent);
            protocalReset(pProtocal);
            break;

          default:
            break;
    }

    pProtocal->checkSum += ch;
    return ret;
}

/*!
 * This function will invoke corresponding handlers
 * which was registerred by uplayer
 *
 * @param[i] pProtocal an instance of @ProtocalStatusType
 *
 * @return ERROR_TYPE
*/
int32_t protocalDispatchEvents(ProtocalStatusType *pProtocal)
{
    int32_t count;
    int32_t ret;
    EventStackType *pEvent;
    uint8_t cmdType;

    (void)listCount(&pProtocal->eventStack, &count);
    if (count == 0)
    {
        return ERROR_NO_RESOURCE;
    }

    /* pop event from stack */
    ret = listGetAt(&pProtocal->eventStack, 0,(uint32_t *)&pEvent);
    if (ret)
        return ret;

    listRemoveData(&pProtocal->eventStack, (uint32_t)pEvent);

    /* Get command type and reset cmdType to release memory */
    cmdType = pEvent->cmdType & 0x7f;
    pEvent->cmdType = 0;

    switch(cmdType)
    {
        case CMD_TYPE_ACK:
            if (pProtocal->pfnAck)
            {
                pProtocal->pfnAck(pEvent->cmd, pEvent->buff[0]);
            }
            break;

        case CMD_TYPE_SET:
        case CMD_TYPE_QUERY:
            if (0 != pProtocal->pfnOnCmd)
            {
                ret = pProtocal->pfnOnCmd(cmdType, pEvent->cmd,
                    pEvent->buff, pEvent->buffSize);
            }
            break;

        default:
            break;
    }

    return EV_SUCCESS;
}

/*!
 * reset protocal status
 *
 * @param[in] pProtocal  instance of protocal
 *
 * @return none
 */
void protocalReset(ProtocalStatusType *pProtocal)
{
    pProtocal->protocalStatus = PROTOCAL_STATUS_CMD_TYPE;
    pProtocal->bufferIndex = 0;
    pProtocal->checkSum = 0;
    pProtocal->cmd = CMD_ID_INVALID;
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
uint8_t protocalIsConnected(ProtocalStatusType *pProtocal)
{
    return pProtocal->connectionStatus;
}


/*
 * This function make a handshake with remote target for establishing
 * physical connection
 *
 * @param[i] pProtocal     an instance of @ProtocalStatusType
 * @param[i] blConnected   connected status
 *
 * @return ERROR_TYPE
 */
int32_t protocalSetDeviceConnected(ProtocalStatusType *pProtocal,
    uint8_t blConnected)
{

    return protocalSendCmdWait(pProtocal, CMD_TYPE_SET, CMD_ID_DEVICE_CONNECTION,
        (void*)&blConnected, sizeof(blConnected));
}
