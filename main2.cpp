#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut iLed3(LED3);

Serial pc(SERIAL_TX, SERIAL_RX);

int main_bk(void)
{
//  led1 = 1;
//  led2 = 1;
	iLed3 = 1;
	while(1)
	{
//		led1 = !led1;
//		led2 = !led2;
		iLed3 = !iLed3;
		wait(1);
	}

	return 0;
}

void led1Thread(void const *args) {

    while (true) {
        led1 = !led1;
        Thread::wait(1000);
    }
}

extern "C" void USBD_Init (void);
extern "C" void USBD_Connect (uint32_t con);
int main() {
    Thread thread(led1Thread);
    int i = 0;

//	USBD_Init();
//	USBD_Connect(true);
    pc.baud(115200);
    while (true) {
    	led1 = !led1;
//        iLed3 = !iLed3;
        pc.printf("Hello World : %d\r\n", i++);
//        printf("hello printf");
//        Thread::wait(500);
    }
    return 0;
}

