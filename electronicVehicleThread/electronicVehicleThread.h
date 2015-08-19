#ifndef electronicVehicleThread_h
#define electronicVehicleThread_h
#include "mbed.h"

class electronicVehicleThread {
    public:
      electronicVehicleThread();

      void uartThread(void const *argument);
      void timerThread(void const *argument);

      void rxInterrupt(void);

      Serial *pBtUart;

    private:
      Queue<uint32_t, 20> m_serialQ;
      uint32_t   reportDuration;
};


#endif
