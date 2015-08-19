#ifndef protocalApi_h
#define protocalApi_h
#include <stdint.h>
#include "protocalTypes.h"

#define CMD_TYPE_QUERY                        1
                                    /**< query + 2 + cmd + checksum */

#define CMD_TYPE_SET                          2
                                    /**< set + n + cmd + data + checksum */

#define CMD_TYPE_ACK                          3
                                    /**< ack + 2 + cmd + result + checksum */

#define CMD_TYPE_ROUTER                       4
                                    /**< simply router a package into stack,
                                    the package will be sent later */

#define CMD_TYPE_MULTI                       5

/*!
 * callback function for sending data.
 * This function is invoked when low level protocal send data to remote device
 */
typedef int32_t (*pfnSendData)(uint8_t* pBuf, uint32_t len);

/*! callback function type for command event.
 * This callback function is invoked on event received from remove side
 */
typedef int32_t (*pfnProtocalOnCmd)(uint8_t cmdType,
    uint8_t cmd, uint8_t *pData, uint8_t len);

/*!
 * callback function type for transaction acknowledgement
 * This callback function is invoked when a transmission is complemented
 */
typedef void (*pfnAck)(uint8_t cmd, uint8_t result);

/*!
 * callback function type for waiting minimum 10 milli-seconds
 * This function is invoked when protocal needs waiting acknowledgements.
 */
typedef void (*pfnWait)(void);

/*!
 * When an package of data is received, if an event is decoded, protocal may
 * require a response, application cann't handle the event inside this handle
 * but have to start an individual thread or timer to receive the event and
 * handle it
 */
typedef void (*pfnScheduleDispatch)(void);


#ifdef __cplusplus
extern "C" {
#endif


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
    pfnAck ackFunc, pfnWait waitFunc, pfnScheduleDispatch pScheduleFunc);

/*!
 * This function set one byte data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetU8(uint8_t cmd, uint8_t data);

/*
 * This function set an unsigned 32bits data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetU32(uint8_t cmd, uint32_t data);

/*!
 * This function set a float data to remote side
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetFloat(uint8_t cmd, float data);

/*!
 * This function set string data to remote side
 * Note: string length must be lessthan 24 bytes
 *
 * @param[i] cmd   command id to be operated
 * @param[i] data  data to be send to remote device
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetStr(uint8_t cmd, const char* str);

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
                    const void* data, uint8_t len);

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
int32_t protocalApiReceiveChar(uint8_t ch);

/*
 * @brief This function dispatch received events to customer applications
 * Notice, this function must be invoked in an individual thread, make sure it
 * wont be blocked by anything!
 *
 * @param none
 *
 * @return @ERROR_TYPE
 */
int32_t protocalApiDispatchEvents(void);

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
uint8_t protocalApiIsConnected(void);

/*!
 * send a query command to remote target and wait until reply
 * Note: this function must be invoked outside of protocal events callback
 * including: @pfnProtocalOnCmd and @pfnAck
 *
 * @param[in] pProtocal  instance of @ProtocalStatusType
 */
int protocalApiQueryWait(uint8_t cmd, void *pData, uint8_t len);

/*
 * This function make a handshake with remote target for establishing
 * physical connection
 *
 * @param[i] pProtocal     an instance of @ProtocalStatusType
 * @param[i] blConnected   connected status
 *
 * @return ERROR_TYPE
 */
int32_t protocalApiSetConnected(uint8_t blConnected);

#ifdef __cplusplus
}
#endif

#endif
