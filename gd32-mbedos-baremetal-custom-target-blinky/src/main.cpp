#include "mbed.h"

#define WAIT_TIME_MS 500 
DigitalOut led1(LED1);

BufferedSerial pc(USBTX, USBRX);

int main()
{
    pc.set_baud(MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
    thread_sleep_for(1000);
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    while (true)
    {
        led1 = !led1;
        thread_sleep_for(WAIT_TIME_MS);
    }
}