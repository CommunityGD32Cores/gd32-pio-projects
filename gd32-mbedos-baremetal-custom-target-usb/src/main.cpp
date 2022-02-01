#include "mbed.h"
#include "USBCDC.h"

#define WAIT_TIME_MS 500 
DigitalOut led1(LED1);

BufferedSerial pc(USBTX, USBRX);

USBCDC cdc(false, 0x1f00, 0x2012, 0x0001);

int main()
{
    pc.set_baud(MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
    thread_sleep_for(1000);
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    while (true)
    {
        led1 = !led1;
        if(cdc.ready()) {
            char msg[] = "Hello world\r\n";        
            cdc.send((uint8_t *)msg, strlen(msg));
        } else {
            printf("CDC was not ready yet.\n");
        }
        thread_sleep_for(WAIT_TIME_MS);
    }
}