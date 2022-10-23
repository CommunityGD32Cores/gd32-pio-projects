
cmake_minimum_required(VERSION 3.15)

add_library(freertos_api INTERFACE)

add_library(freertos)

target_sources(freertos
    PRIVATE
        Source/croutine.c
        Source/event_groups.c
        Source/list.c
        Source/queue.c
        Source/stream_buffer.c
        Source/tasks.c
        Source/timers.c
        CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c
        CMSIS/RTOS2/FreeRTOS/Source/freertos_evr.c
        CMSIS/RTOS2/FreeRTOS/Source/os_systick.c
        CMSIS/RTOS2/FreeRTOS/Source/ARM/clib_arm.c
)

target_sources(freertos
    PRIVATE
        Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
        Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
        Source/portable/Common/tickless_sleep.c
        Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm1.s
)



target_include_directories(freertos_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/portable/Common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source>
)

target_link_libraries(freertos
    freertos_api
    gd32w51x_wifi_api
    gd32w51x_peripheral_api
    wifi_api
    os_api
)
