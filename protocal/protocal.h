#ifndef protocal_h
#define protocal_h
#include <stdint.h>
#include "list.h"
#include "protocalApi.h"

#define CMD_ID_INTERNAL_START                 100
/*!
 * ask remote side to do a reset
 */
#define CMD_ID_RESET_TARGET                    100

/*! handshake with remote side connection status
 */
#define CMD_ID_DEVICE_CONNECTION               101

/*! protocal version num
 * format 4bytes number
 */
#define CMD_ID_PROTOCAL_VERSION                102

/*!
 * protocal copyright
 * format string
 */
#define CMD_ID_PROTOCAL_COPYRIGHT              103

/*! protocal release date time */
#define CMD_ID_PROTOCAL_DATETIME               104


/*! invalid command id, default value for cmd area */
#define CMD_ID_INVALID                         0xff

/*! max package size for each frame */
#define PROTOCAL_MAX_PACKAGE_LENGTH            32

/*!
 * package is conposed of following meteria:
 * header(4Bytes) + type(1Byte) + length(1Bytes) + command(1Byte) + data(nBytes)
 * + checkSum(1Byte), hence totally system used 8 bytes
 */
#define PROTOCAL_MAX_DATA_LENGTH               (PROTOCAL_MAX_PACKAGE_LENGTH - 8)

/*! max event number in stack
 */
#define PROTOCAL_MAX_EVENT_NUM                 8

/*!
 * header size
 */
#define PROTOCAL_HEADER_SIZE                   4


#define PROTOCAL_WAIT_TIMES                   100


/*!
 * Status of protocal analysising commands in TLD mode
 */
typedef enum ProtocalStatus {
    PROTOCAL_STATUS_HEADER,
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

    uint8_t     header[PROTOCAL_HEADER_SIZE];

    uint8_t     sendDevice; /**< which device to send,
                        this will chose a magic header for sending */
    uint8_t     recvDevice; /**< which device received from,
                        recognised via magic header */

    EventStackType  eventPool[PROTOCAL_MAX_EVENT_NUM];
    list_t      eventStack; /**< event statck to be processed */

    volatile ProtocalWaitingStatus waitingStat;
    uint8_t waitingCmd;
    uint8_t replyLen;
    uint8_t replyErr; /* the most recent ack result error code */
    pfnProtocalOnCmd pfnOnCmd; /**< on command received */
    pfnSendData      pfnSendData;  /**< send data to host or slave end */
    pfnAck           pfnAck;
    pfnWait          pfnWait;
    pfnScheduleDispatch  pfnSchedule;
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
 * This function set a device to receive the next command
 *
 * @param[i] devID  device ID @ProtocalDeviceType
 *
 * @return none
 */
void protocalSetDevice(ProtocalStatusType *pProtocal, uint8_t devID);


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
        uint8_t cmd, const void *pData, uint8_t lengthOfData);

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
        uint8_t cmd, const void *pData, uint8_t lengthOfData);

/*!
 * Send again last package
 *
 * @param[i] pProtocal an instance of ProtocalStatusType
 *
 * @return none
 */
void protocalResendLastPackage(ProtocalStatusType *pProtocal);

/*!
 * @brief send command to host or slave, and wait for the acknowledgement
 * Notice! This function must be invoked outside of callback function
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
