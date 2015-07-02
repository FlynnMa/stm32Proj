#ifndef protocal_h
#define protocal_h
#include <stdint.h>

#define PROTOCAL_DATA_BUFFER_LENGTH           255


/*!
 * Status of protocal analysising commands in TLD mode
 */
typedef enum ProtocalStatus {
    PROTOCAL_STATUS_CMD,
    PROTOCAL_STATUS_LENGTH,
    PROTOCAL_STATUS_DATA,
    PROTOCAL_STATUS_ACK,
    PROTOCAL_STATUS_MAX
} ProtocalStatus;

typedef int32_t (*pfnProtocalOnCmd)(uint8_t cmd, uint8_t *pData, uint32_t len);
typedef int32_t (*pfnSendData)(uint8_t* pBuf, uint32_t len);
typedef void (*pfnAck)(uint8_t cmd, uint8_t result);

/*!
 * Stats data of protocal
 */
typedef struct ProtocalStats{
    uint8_t     connectionStatus; /**< 0 : disconnected 1 : connected*/
    uint8_t     uartReady;        /**< 0 : not ready    1 : ready*/
    uint8_t     checkSum;         /**< checksum */
    uint8_t     checkSumRecv;     /**< received checksum from uplayer */
    uint8_t     cmd;              /**< current command ID */
    uint16_t    protocalStatus;
    uint16_t    reportDuration;  /**< In milli-seconds */
    uint8_t     buffer[PROTOCAL_DATA_BUFFER_LENGTH];      /**< A command buffer configured by
                                customer for storing temp information */
    uint8_t     cmdSize; /* contents contained in buffer */
    uint32_t    bufferIndex;  /**< Index of @pCmdBuffer */

    uint8_t     package[PROTOCAL_DATA_BUFFER_LENGTH];
    uint8_t     packageSize;

    pfnProtocalOnCmd pfnOnCmd; /**< on command received */
    pfnSendData      pfnSendData;  /**< send data to host or slave end */
    pfnAck           pfnAck;
} ProtocalStatusType;

/*!
 * Calculate checksum
 *
 * @param[in] buf  buffer of contents to be calculated
 * @param[in] len  length of @buf contents
 *
 * @return checksum value
 */
uint8_t protocalCalculateChecksum(ProtocalStatusType *pProtocal,uint8_t *buf, uint32_t len);

/*!
 * Ackknoledge to the host or slave
 *
 * @param[in] result
 *
 * @return none
 */
void protocalAck(ProtocalStatusType *pProtocal, uint8_t result);

/*!
 * Set a callback function for responsing commands
 *
 * @param[in] pfnOnCmd
 *
 */
void protocalSetOnCmdCallback(ProtocalStatusType *pProtocal, pfnProtocalOnCmd pfnOnCmd);

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
 * @brief make data package
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return none
 */
void protocalMakePackage(ProtocalStatusType *pProtocal, uint8_t cmd, void *pData, uint8_t lengthOfData);

/*!
 * @brief send command to host or slave
 *
 * @param[in] cmd               command id
 * @param[in] pData             arguments of cmd
 * @param[in] lengthOfData      length of @pData
 *
 * @return none
 */
void protocalSendCmd(ProtocalStatusType *pProtocal, uint8_t cmd, void *pData, uint8_t lengthOfData);

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

#endif
