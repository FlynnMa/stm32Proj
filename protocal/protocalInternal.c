#include "protocal.h"
#include "protocalApi.h"
#include "protocalInternal.h"


static const uint8_t version[4] = {PROTOCAL_VERSION_MAJOR,
        PROTOCAL_VERSION_SUB, PROTOCAL_VERSION_MODIFY, PROTOCAL_VERSION_NUM};

static const char copyright[] = "Ma Yunfei @ 2015";

static const char date[] = __DATE__;


static int32_t onSet(ProtocalStatusType *pProtocal,
    uint8_t cmd, uint8_t *pData, uint8_t len)
{
    int32_t ret = 0;

    switch(cmd)
    {
        case CMD_ID_RESET_TARGET:
            protocalReset(pProtocal);
            break;

        case CMD_ID_DEVICE_CONNECTION:
            pProtocal->connectionStatus = *pData;
            ret = 1;
            break;

        default:
            break;
    }
    return ret;
}

static int32_t onQuery(ProtocalStatusType *pProtocal,
    uint8_t cmd, uint8_t *pData, uint8_t len)
{
    int32_t ret = 0;

    switch(cmd)
    {
        case CMD_ID_PROTOCAL_VERSION:
            protocalApiSetU32(cmd, *(uint32_t*)version);
            ret = 1;
            break;

        case CMD_ID_PROTOCAL_COPYRIGHT:
            protocalApiSetStr(cmd, copyright);
            ret = 1;
            break;

        case CMD_ID_PROTOCAL_DATETIME:
            protocalApiSetStr(cmd, date);
            ret = 1;
            break;

        case CMD_ID_DEVICE_CONNECTION:
            protocalApiSetU8(cmd, pProtocal->connectionStatus);
            break;

        default:
            break;
    }
    return ret;
}

/*
 * @brief Do the protocal level command handler
 * Note: this function does hiden process in embeded target, mobile phone can
 * get those information via commands to identify it.
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
int32_t protocalPreprocessCmd(ProtocalStatusType *pProtocal,
    uint8_t cmdType, uint8_t cmd, uint8_t *pData, uint8_t len)
{
    uint32_t ret = 0;

    switch(cmdType)
    {
        case CMD_TYPE_ACK:
            break;

        case CMD_TYPE_QUERY:
            ret = onQuery(pProtocal, cmd, pData, len);
            break;

        case CMD_TYPE_SET:
            ret = onSet(pProtocal, cmd, pData, len);
            break;

        default:
            break;
    }

    return ret;
}

