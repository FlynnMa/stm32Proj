#include "mbed.h"
#include "rtos.h"
#include "util.h"
#include "shell.h"

DigitalOut myled(LED1);

extern const Command custCommands[];
//Serial pc(USBTX, USBRX);

#if 0
void test(void const *argument)
{
//    int i = 0;
    int addr = 0;
    int addr7 = 0;
    char data[4] = {0x55,0xaa,0x44,0x88};

    pc.baud(115200);
//    i2c.frequency(400);

    pc.printf("start master thread\r\n");
    while(1)
    {
        addr7 = addr * 2;
        data[0] = 0;
        i2c.write(addr7, data, 1, true);
        data[0] = 0;
        data[1] = 0;
        i2c.read(addr7, data, 2);

        if (data[0] != 0)
        {
            pc.printf("detected:%x, %x, %x,%x\r\n", addr, addr7, data[0], data[1]);
        }
        addr++;
        if (addr >= 127)
        {
            Thread::wait(10000);
            addr = 0;
        }
        Thread::wait(200);
  }
}
#endif

Thread *pShellThread;
void shellThreadProc(void const *argument)
{
    Shell *mShell = Shell::instance();

    mShell->setCustCommand(custCommands);

    mShell->process(pShellThread);
}
unsigned char shellStack[1024];

extern void sensorsAccEnable(int en);
int main()
{

//        pc.baud(115200);
//        BMA  *acc = new BMA(i2c);
//        Thread::wait(100);
        Thread shellThread(shellThreadProc, NULL, osPriorityBelowNormal, sizeof(shellStack), shellStack);
        pShellThread = &shellThread;

        while(1)
        {
//            printf("new printf. %d\r\n", i++);
            myled = !myled;
            Thread::wait(2000);
//            wait(1);
        }
}

