#ifndef evType_h
#define evType_h

#ifndef NULL
#define NULL            ((void *) 0)
#endif

/*!
 * error status
 */
enum ERROR_TYPE{
    EV_SUCCESS,
    ERROR_HW_FAILURE,
    ERROR_UNSUPPORTED,
    ERROR_INVALID_PARAM,
    ERROR_BAD_STATUS,
    ERROR_CHECKSUM,
    ERROR_OUT_RANGE,
    ERROR_NO_RESOURCE,
    ERROR_BUSY_STAT,
    ERROR_TIME_OUT,
    ERROR_MAX
} ;

#endif

