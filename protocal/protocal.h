#ifndef protocal_h
#define protocal_h
#include <stdint.h>
#include "list.h"


#define CMD_ID_RESET_TARGET                   100

#define CMD_ID_DEVICE_CONNECTION              101

#define CMD_ID_INVALID                       0xff


#define PROTOCAL_MAX_PACKAGE_LENGTH           32

#define PROTOCAL_MAX_DATA_LENGTH              (PROTOCAL_MAX_PACKAGE_LENGTH - 4)

#define PROTOCAL_MAX_EVENT_NUM                8


/*!
 * Status of protocal analysising commands in TLD mode
 */
typedef enum ProtocalStatus {
    PROTOCAL_STATUS_CMD_TYPE,
    PROTOCAL_STATUS_LENGTH,
    PROTOCAL_STATUS_CMD,
    PROTOCAL_STATUS_QUERY,
    PROTOCAL_STATUS_DATA,
    PROTOCAL_STATUS_CHECKSUM,
    PROTOCAL_STATUS_MAX
} ProtocalStatus;

typedef enum ProtocalWaitingStatus {
    PROTOCAL_WAITING_NONE,
    PROTOCAL_WAITING_ACK = 1,
    PROTOCAL_WAITING_QUERY = 2,
    PROTOCAL_WAITING_MAX
} ProtocalWaitingStatus;
typedef int32_t (*pfnProtocalOnCmd)(uint8_t cmdType,
    uint8_t cmd, uint8_t *pData, uint8_t len);
typedef int32_t (*pfnSendData)(uint8_t* pBuf, uint32_t len);
typedef void (*pfnAck)(uint8_t cmd, uint8_t result);
typedef void (*pfnWait)(void);

typedef struct EventStackType {
    uint8_t cmdType;
    uint8_t cmd;
    uint8_t buff[PROTOCAL_MAX_DATA_LENGTH];
    uint8_t buffSize;
} EventStackType;

/*!
 * Stats data of protocal
 */
typedef struct ProtocalStats{
    uint8_t     connectionStatus; /**< 0 : disconnected 1 : connected*/
    uint8_t     uartReady;        /**< 0 : not ready    1 : ready*/
    uint8_t     checkSum;         /**< checksum */
    uint8_t     checkSumRecv;     /**< received checksum from uplayer */
    uint8_t     cmd;              /**< current command ID */
    uint8_t     cmdType;
    uint16_t    protocalStatus;
    uint16_t    reportDuration;  /**< In milli-seconds */
    uint8_t     buffer[PROTOCAL_MAX_PACKAGE_LENGTH];
                                /**< A command buffer for
                                     receiving serial data */
    uint8_t     cmdSize; /* contents contained in buffer */
    uint8_t     bufferIndex;  /**< Index of @pCmdBuffer */

    uint8_t     package[PROTOCAL_MAX_PACKAGE_LENGTH];
    uint8_t     packageSize;

    EventStackType  eventPool[PROTOCAL_MAX_EVENT_NUM];
    list_t      eventStack; /**< event statck to be processed */

    volatile ProtocalWaitingStatus waitingStat;
    uint8_t waitingCmd;
    uint8_t replyLen;
    pfnProtocalOnCmd pfnOnCmd; /**< on command received */
    pfnSendData      pfnSendData;  /**< send data to host or slave end */
    pfnAck           pfnAck;
    pfnWait          pfnWait;
} ProtocalStatusType;

/*!
 * Initialize protocal

*
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t protocalInit(ProtocalStatusType *pProtocal);

/*!
 * Calculate checksum
 *
 * @param[in] buf  buffer of contents to be calculated
 * @param[in] len  length of @buf contents
 *
 * @return checksum value
 */
uint8_t protocalCalculateChecksum(ProtocalStatusType *pProtocal,
        uint8_t *buf, uint32_t len);

/*!
 * Ackknoledge to the host or slave
 *
 * @param[in] result
 *
 * @return none
 */
void protocalAck(ProtocalStatusType *pProtocal, uint8_t result);

/*!
 * send a query command to remote target and wait until reply
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 */
int protocalQueryWait(ProtocalStatusType *pProtocal, uint8_t cmd,
    void *pData, uint8_t len);

/*!
 * Set a callback function for responsing commands
 *
 * @param[in] pfnOnCmd
 *
 */
void protocalSetOnCmdCallback(ProtocalStatusType *pProtocal,
            pfnProtocalOnCmd pfnOnCmd);

/*!
 * set a callback function on responsing acknlodgement
 *
 * @param[in] pProtocal instance of protocal
 * @param[in] ackFunc   acknowledge callback function
 *
 * @return none
 */
void protocalSetOnAckCallback(ProtocalStatusType *pProtocal, pfnAck ackFunc);

/*!
 * Set a callback function on waiting response
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 * @param[in] waitFunc   wait callback function
 *
 * @return none
 */
void protocalSetWaitFunc(ProtocalStatusType *pProtocal,
    pfnWait waitFunc);

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
        uint8_t cmd, void *pData, uint8_t lengthOfData);

/*!
 * @brief send command to host or slave
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return none
 */
int protocalSendCmd(ProtocalStatusType *pProtocal, uint8_t cmdType,
        uint8_t cmd, void *pData, uint8_t lengthOfData);

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
    uint8_t cmd, void *pData, uint8_t lengthOfData);

/*!
 * Set a callback function for responsing commands
 *
 * @param[in] pfnOnCmd
 *
 */
void protocalSetOnCmdCallback(ProtocalStatusType *pProtocal, pfnProtocalOnCmd pfnOnCmd);

/*!
 * Set a callback function for sending data
 *
 * @param[in] pfnOnCmd
 *
 * @return none
 */
void protocalSetSendDataFunc(ProtocalStatusType *pProtocal, pfnSendData pfnSend);

/*!
 * Response for receiving character, and input to protocal analysis
 *
 * @param[in] ch character input into protocal
 *
 * @return @ERROR_TYPE
 */
int32_t protocalUartReceiveChar(ProtocalStatusType *pProtocal, uint8_t ch);

/*!
 * reset protocal status
 *
 * @param[in] pProtocal  instance of protocal
 *
 * @return none
 */
void protocalReset(ProtocalStatusType *pProtocal);

/*!
 * This function will invoke corresponding handlers
 * which was registerred by uplayer
 *
 * @param[i] pProtocal an instance of @ProtocalStatusType
 *
 * @return ERROR_TYPE
*/
int32_t protocalDispatchEvents(ProtocalStatusType *pProtocal);

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
uint8_t protocalIsConnected(ProtocalStatusType *pProtocal);

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
    uint8_t blConnected);

#endif
