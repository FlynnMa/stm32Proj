#include "mbed.h"
#include "rtos.h"
#include "electronicVehicleThread.h"
#include "electronicVehicle.h"
#include "electronicVehicleCustom.h"
#include "trace.h"

void electronicVehicleThread::rxInterrupt(void)
{
    uint32_t ch;

    while (pBtUart->readable()) {
        ch = (uint32_t)pBtUart->getc();
        m_serialQ.put((uint32_t*)ch);
    }
}

electronicVehicleThread::electronicVehicleThread()
{
    reportDuration = 1000;
    pBtUart = new Serial(SERIAL_TX, SERIAL_RX);
    pBtUart->baud(115200);
    pBtUart->attach(this, &electronicVehicleThread::rxInterrupt, Serial::RxIrq);

    electronicVehicleInitialize();

}

void electronicVehicleThread::uartThread(void const *argument)
{
    char ch;
    int i = 0;

    while(1)
    {
        osEvent evt = m_serialQ.get();
        if (evt.status != osEventMessage)
        {
            Trace("queue->get() return %02x status", evt.status);
            continue;
        }

        ch = (char)evt.value.v;
        TracePrint("0x%02x", ch);
        if (i ++ >= 7)
        {
            i = 0;
            Trace("");
        }
        electronicVehicleOnChar(ch);

        TracePrint(", ", ch);
    }
}

void electronicVehicleThread::timerThread(void const *argument)
{
    while(1)
    {
        Thread::wait(1);

        if (electronicVehicleIsConnected() == 0)
        {
            continue;
        }

        electronicVehicleDispatchEvents();

//        electronicVehicleSchedule(reportDuration);
    }
}

extern "C" {
void electronicVehicleCustomWait(void)
{
    Thread::wait(10);
}

}



