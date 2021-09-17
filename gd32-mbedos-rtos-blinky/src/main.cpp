#include "mbed.h"
#include "rtos.h"

Thread thread;
DigitalOut myled(LED1);

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
    thread.start(callback(blinky_thread));
    //(eternally) wait for thread to exit
    thread.join();
    return 0;
}