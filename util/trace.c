#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "trace.h"

/*
 * @brief Initialize Fdtrace with a serial instance
 *
 * @param pSerial[i] an instance of serial port
 *
 * @return none
 */
void TraceInit(void)
{
}

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
void Trace(const char *fmt, ...)
{
  char buf[128];
  uint32_t len;
  va_list argList;
  va_start(argList,fmt);

  len = strlen(fmt);
  if (len > 0)
  {
    /* to prevent empty string of 'fmt' */
    vsnprintf((char*)buf, 127, fmt, argList);
    va_end(argList);
    len = strlen(buf);
    len = len > 125 ? 125 : len;
    buf[len] = 0;
    printf(buf);
  }
  printf("\r\n");
}

/*
 * @brief Draw a line with '-' with CRCL
 *
 * @param none
 *
 * @return
 */
void TraceLine(void)
{
  int i;

  for (i = 0; i < 16; i++)
    printf("#");
  printf("\r\n");
}

/*
 * @brief Print a formated string without CRCL
 *
 * @param fmt[i]  same as @Trace
 *
 * @return none
 */
void TracePrint(const char *fmt, ...)
{
  char buf[128];
  uint32_t len;
  va_list argList;
  va_start(argList,fmt);

  len = strlen(fmt);
  if (len > 0)
  {
    /* to prevent empty string of 'fmt' */
    buf[127] = 0;
    buf[0] = 0;
    buf[1] = 0;
    vsnprintf(buf, 127, fmt, argList);
    va_end(argList);
    len = strlen((const char*)buf);
    len = len > 127 ? 127 : len;
    buf[len] = 0;
    printf(buf);
    fflush(stdout);
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  Trace("assert failed: file %s on line %d\r\n", file, line);
  /* Infinite loop */
  while (1)
  {
  }
}

#endif


