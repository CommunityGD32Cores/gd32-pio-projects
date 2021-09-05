# SPL + FreeRTOS (as CMSIS-OS2) example

## Description 

Showcases the usage of FreeRTOS in SPL, with the activated CMSIS-OS2 abstraction layer turned on. This simplifies some of the API usages and follows a CMSIS standard for RTOS functions (`cmsis_os2.h`).

This example builds on the FreeRTOS library found in the [spl-freertos](../gd32-spl-freertos) example.

The FreeRTOS library is implemented in a generic way so that it works for all GD32 ARM chips. It is based on the current FreeRTOS V10.4.4 version.

## Technicalities

The FreeRTOS library auto-detects the CPU type from the board manifest and compiles the FreeRTOS base files plus the needed port files (e.g., `ARM_CM4F`).

To configure FreeRTOS, a `FreeRTOSConfig.h` file must be in the global include path. This is usually done via the `build_flags` of the project's `platformio.ini`.

In accordance to the selected configuration options, the user may need to implement additional functions needed by FreeRTOS. However, most of these functions are already implemented by the CMSIS-OS2 layer. In this case, we only need to implement `vAssertCalled()` and some runtime timer functions because the `FreeRTOSConfig.h` references this. This project implements these functions in `src/freertos_callbacks.c`. 

## Expected output

The firmware uses one task to blink the LED defined at the top of `src/main.c`, and another task to print its own thread name and ID.

The output is configured in the same way as in the [spl-usart](../gd32-spl-usart) example.

```
Starting FreeRTOS + CMSIS-OS2 demo! Running on "FreeRTOS V10.4.4"
Blinky!
Hello from thread "Printer" (Thread ID 0x20001238)!
Blinky!
Blinky!
Hello from tBlinky!
hread "Printer" (Thread ID 0x20001238)!
Blinky!
...
```