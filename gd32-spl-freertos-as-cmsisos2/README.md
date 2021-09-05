# SPL + FreeRTOS example

## Description 

Showcases the usage of FreeRTOS in SPL.

The FreeRTOS library is implemented in a generic way so that it works for all GD32 ARM chips. It is based on the current FreeRTOS V10.4.4 version.

## Technicalities

The FreeRTOS library auto-detects the CPU type from the board manifest and compiles the FreeRTOS base files plus the needed port files (e.g., `ARM_CM4F`).

To configure FreeRTOS, a `FreeRTOSConfig.h` file must be in the global include path. This is usually done via the `build_flags` of the project's `platformio.ini`.

In accordance to the selected configuration options, the user may need to implement additional functions needed by FreeRTOS. For example, when static allocation is switched on, FreeRTOS requires that the function `vApplicationGetIdleTaskMemory()` be implemented to return the static memory for the idle task, et cetera. This project implements these functions in `src/freertos_callbacks.c`.

## Expected output

The firmware uses one task to blink the LED defined at the top of `src/main.c`, and another task to print runtime statistics about running tasks.

The output is configured in the same way as in the [spl-usart](../gd32-spl-usart) example.

```
Starting FreeRTOS demo!
Blinky!
Total runtime: 26370 ticks
Task: "IDLE", Prio: 0, Runtime: 26358, CPU Usage: 100%
Task: "TaskManag", Prio: 0, Runtime: 0, CPU Usage:<1% 
Task: "Blinky", Prio: 0, Runtime: 9, CPU Usage:<1%    
Task: "Tmr Svc", Prio: 9, Runtime: 0, CPU Usage:<1%   
Blinky!
Blinky!
Total runtime: 53001 ticks
Task: "TaskManag", Prio: 0, Runtime: 276, CPU Usage:<1%
Task: "IDLE", Prio: 0, Runtime: 52695, CPU Usage: 99%  
Task: "Blinky", Prio: 0, Runtime: 27, CPU Usage:<1%
Task: "Tmr Svc", Prio: 9, Runtime: 0, CPU Usage:<1%
Blinky!
Blinky!
...
```