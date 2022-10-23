
cmake_minimum_required(VERSION 3.15)

add_library(bsp_api INTERFACE)

add_library(bsp)

target_sources(bsp
    PUBLIC
        rf_efuse.c
        gd32_it.c
    PRIVATE
        bsp_wlan.c
        dma.c
        flash_api.c
        init_rom.c
        trng.c
        uart.c
)


target_include_directories(bsp_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(bsp
    gd32w51x_peripheral
    gd32w51x_wifi_api
    bsp_api
    os_api
    wifi_api
)
