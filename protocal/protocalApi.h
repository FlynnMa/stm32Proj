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




/*!
 * Protocal schedule for doing regular analysis and reporting
 *
 * @param[in] duration duration in ms since last report
 *
 * @return none
 */
typedef void (*pfnProtocalSchedule)(uint32_t duration, void *pData);

/*!
 * protocal events response, to be implemented by customer
 *
 * @param[in] event event to be responsed, relate to @EV_EVENT_TYPE
 * @param[in] data  data corresponding with event
 *
 * @return none
 */
//void protocalCustomEvents(EV_EVENT_TYPE event, uint32_t data);

/*!
 * Response for receiving character, and input to protocal analysis
 *
 * @param[in] ch char input into protocal
 *
 * @return @ERROR_TYPE
 */
int32_t protocalUartReceiveChar(ProtocalStatusType *pProtocal, uint8_t ch);

#endif
