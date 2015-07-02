#include <stdlib.h>
#include "protocal.h"
#include "protocalApi.h"

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
    return 0;
}

void protocalAckOK(ProtocalStatusType *pProtocal)
{
    uint8_t buf[3];

    buf[0] = ACK;
    buf[1] = pProtocal->cmd;
    buf[2] = SUCCESS;
    pProtocal->pfnSendData(buf, 3);
}

void protocalAck(ProtocalStatusType *pProtocal, uint8_t result)
{
    uint8_t buf[3];

    buf[0] = ACK;
    buf[1] = pProtocal->cmd;
    buf[2] = result;
    pProtocal->pfnSendData(buf, 3);
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
void protocalMakePackage(ProtocalStatusType *pProtocal, uint8_t cmd, void *pData, uint8_t lengthOfData)
{
    uint8_t checkSum = 0;
    uint8_t i;

    pProtocal->package[0] = cmd;
    pProtocal->package[1] = lengthOfData + 1;
    memcpy(&pProtocal->package[2], pData, lengthOfData);

    for(i = 0; i < lengthOfData + 2; i++)
    {
        checkSum += pProtocal->package[i];
    }
    pProtocal->package[lengthOfData + 2] = checkSum;
    pProtocal->packageSize = lengthOfData + 3;
}

/*!
 * @brief send command to host or slave
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return none
 */
void protocalSendCmd(ProtocalStatusType *pProtocal, uint8_t cmd, void *pData, uint8_t lengthOfData)
{
    protocalMakePackage(pProtocal, cmd, pData, lengthOfData);
    if (pProtocal->pfnSendData)
    {
        pProtocal->pfnSendData(pProtocal->package, pProtocal->packageSize);
    }
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
void protocalSetOnAckCallback(ProtocalStatusType *pProtocal, pfnAck ackFunc)
{
    pProtocal->pfnAck = ackFunc;
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

void protocalPreprocessCmd(ProtocalStatusType *pProtocal, uint8_t cmd,
    uint8_t *pData, uint32_t len)
{
    switch(cmd)
    {
        case CMD_ID_SET_BLUETOOTH_CONNECTION:
            break;

        case CMD_ID_RESET_STATUS:
            pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
            break;

        default:
            break;
    }

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
    int32_t ret = SUCCESS;

    switch(pProtocal->protocalStatus)
    {
        case PROTOCAL_STATUS_CMD:
            /* set command type and update status */
            pProtocal->checkSum = 0;
            pProtocal->cmd = ch;
            pProtocal->protocalStatus = PROTOCAL_STATUS_LENGTH;
            pProtocal->bufferIndex = 0;
            if (ch == ACK)
            {
                pProtocal->protocalStatus = PROTOCAL_STATUS_ACK;
            }
            break;

        case PROTOCAL_STATUS_LENGTH:
            /* set buffer length and update status */
            pProtocal->cmdSize = ch;
            pProtocal->protocalStatus = PROTOCAL_STATUS_DATA;
            break;

        case PROTOCAL_STATUS_DATA:
            /* fill buffer */
            if ((pProtocal->bufferIndex + 1) < pProtocal->cmdSize)
            {
                pProtocal->buffer[pProtocal->bufferIndex++] = ch;
                break;
            }

            if ((pProtocal->bufferIndex + 1) > pProtocal->cmdSize)
            {
                pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
                return ERROR_OUT_RANGE;
            }

            /* Get checksum*/
            pProtocal->checkSumRecv = ch;

            /* verify checkSum */
            if (pProtocal->checkSumRecv != pProtocal->checkSum)
            {
                protocalReset(pProtocal);
                pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
                protocalAck(pProtocal, (uint8_t)ERROR_CHECKSUM);
                return ERROR_CHECKSUM;
            }

            protocalPreprocessCmd(pProtocal, pProtocal->cmd, pProtocal->buffer, pProtocal->bufferIndex);
            if (0 != pProtocal->pfnOnCmd)
            {
                ret = pProtocal->pfnOnCmd(pProtocal->cmd, pProtocal->buffer, pProtocal->bufferIndex);
            }
            protocalAck(pProtocal, (uint8_t)ret);

            /* reset status */
            pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
            break;

          case PROTOCAL_STATUS_ACK:
            pProtocal->buffer[pProtocal->bufferIndex++] = ch;
            if ((pProtocal->bufferIndex == 2) && pProtocal->pfnAck)
            {
                pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
                pProtocal->pfnAck(pProtocal->cmd,
                    pProtocal->buffer[0], pProtocal->buffer[1]);
            }
            break;

          default:
            break;
    }

    pProtocal->checkSum += ch;
    return ret;
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
    pProtocal->protocalStatus = PROTOCAL_STATUS_CMD;
    pProtocal->bufferIndex = 0;
    pProtocal->checkSum = 0;
}



