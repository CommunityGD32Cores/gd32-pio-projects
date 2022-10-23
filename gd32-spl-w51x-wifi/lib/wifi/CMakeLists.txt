
cmake_minimum_required(VERSION 3.15)

add_library(wifi_api INTERFACE)

add_library(wifi)

target_sources(wifi
    PRIVATE
        wifi_eloop.c
        wifi_management.c
        wifi_netlink.c
)


target_include_directories(wifi_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(wifi
    wifi_api
    lwIP_api
    os_api
    gd32w51x_peripheral
    gd32w51x_wifi_api
    gd32w51x_wifi
    lwIP
)
