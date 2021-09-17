#include "mbed.h"
#include "rtos.h"

Thread thread;
DigitalOut myled(LED1);
BufferedSerial pc(USBTX, USBRX);

/* overkill for just blinking an LED, but here to prove a point that you can use a Thread. */
void blinky_thread() {
    while (true) {
        myled = 1;
        ThisThread::sleep_for(1000);
        myled = 0;
        ThisThread::sleep_for(1000);
    }
}

int main() {
    pc.set_baud(MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
    thread_sleep_for(1000);
    printf("This is the RTOS blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    thread.start(callback(blinky_thread));
    //(eternally) wait for thread to exit
    thread.join();
    return 0;
}