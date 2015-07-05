#ifndef _fdTrace_h
#define _fdTrace_h

#define  TraceIDQuiet    0
#define  TraceIDFatal    1
#define  TraceIDError    2
#define  TraceIDWarning  3
#define  TraceIDInfo     4
#define  TraceIDDebug    5

#define TraceFormat(fmt,tag) "%s"fmt"\r\n", tag

#define AssertFormat(fmt) "Assert@%s:%d"fmt"\r\n", __FUNCTION__, __LINE__

#define AssertFull(condition, fmt, args) do{\
            if (1 == condition) {\
              Trace(AssertFormat(fmt), ##args);\
              while(1);\
            }\
          }while(0)

#define Assert(condition) do{\
            if (1 == condition) {\
              Trace(AssertFormat(""),"");\
              while(1);\
              }\
          }while(0)

#define TraceFatalDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "FATAL: "), ##args);\
        Assert(0);\
    }while(0)

#define TraceInfoDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "info: "), ##args);\
    }while(0)

#define TraceWarningDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "warn: "), ##args);\
    }while(0)

#define TraceDebugDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "debg: "), ##args);\
    }while(0)

#define TraceErrorDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "error: "), ##args);\
    }while(0)

#define TraceSuccessDefine(fmt, args...) do{\
        Trace(TraceFormat(fmt, "Success!\r\n"), ##args);\
    }while(0)

#define TRACE_LEVEL TraceIDQuiet

/*
#ifdef __TRACE_LEVEL_QUIET__
#define TRACE_LEVEL   TraceIDQuiet
#elif (defined __TRACE_LEVEL_ERROR__)
#define TRACE_LEVEL   TraceIDFatal
#elif (defined __TRACE_LEVEL_WARNING__)
#define TRACE_LEVEL   TraceIDWarning
#elif (defined __TRACE_LEVEL_INFO__)
#define TRACE_LEVEL   TraceIDInfo
#elif (defined __TRACE_LEVEL_DEBUG__)
#define TRACE_LEVEL   TraceIDDebug
#endif
*/
#if (TRACE_LEVEL >= TraceIDQuiet)
#define TraceFatal(fmt, args...) TraceFatalDefine(fmt, ##args)
#else
#define TraceFatal(fmt, args...) TraceFatalDefine(fmt, ##args)
#endif

#if (TRACE_LEVEL >= TraceIDError)
#define TraceError(fmt, args...) TraceErrorDefine(fmt, ##args)
#else
#define TraceError(fmt, args...)
#endif

#if (TRACE_LEVEL >= TraceIDWarning)
#define TraceWarn(fmt, args...) TraceWarningDefine(fmt, ##args)
#else
#define TraceWarn(fmt, args...)
#endif

#if (TRACE_LEVEL >= TraceIDInfo)
#define TraceInfo(fmt, args...) TraceInfoDefine(fmt, ##args)
#else
#define TraceInfo(fmt, args...)
#endif

#if (TRACE_LEVEL >= TraceIDDebug)
#define TraceSuccess(fmt, args...) TraceSuccessDefine(fmt, ##args)
#define TraceDebug(fmt, args...) TraceDebugDefine(fmt, ##args)
#else
#define TraceSuccess(fmt, args...)
#define TraceDebug(fmt, args...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Initialize Fdtrace with a serial instance
 *
 * @param pSerial[i] an instance of serial port
 *
 * @return none
 */
void TraceInit(void);


/*
 * @brief Trace information via configured console, the configued console
 *   can be either via uart or usb CDC devices
 *
 * @param fmt[i] const c string contains a format string that follows the same
 * specifications as format in printf@http://www.cplusplus.com/reference/cstdio/printf/.
 * A format specifier follows this prototype: [see compatibility note below]
*  %[flags][width][.precision][length]specifier
*
* @return none
 */
void Trace(const char *fmt, ...);

/*
 * @brief Draw a line with '-' with CRCL
 *
 * @param none
 *
 * @return
 */
void TraceLine(void);

/*
 * @brief Print a formated string without CRCL
 *
 * @param fmt[i]  same as @Trace
 *
 * @return none
 */
void TracePrint(const char *fmt, ...);

#ifdef __cplusplus
}
#endif



#endif
