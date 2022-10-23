
cmake_minimum_required(VERSION 3.15)

add_library(common_api INTERFACE)

add_library(common)

target_sources(common
    PUBLIC
        dlist.c
        cJSON.c
    PRIVATE
        debug_print.c
        delay.c
        malloc.c
        wakelock.c
)


target_include_directories(common_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(common
    common_api
    gd32w51x_peripheral_api
    gd32w51x_wifi_api
    os_api
    wifi_api
)
