
cmake_minimum_required(VERSION 3.15)

add_library(gd32w51x_wifi_api INTERFACE)

target_include_directories(gd32w51x_wifi_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmn>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/osal>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/soc>
)

if (NOT CONFIG_STATIC_LIB_BUILDED)
    add_library(gd32w51x_wifi STATIC)
    file(GLOB_RECURSE SRC_WIFI_DRIVER  ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

    MACRO(HEADER_DIRECTORIES return_list)
        FILE(GLOB_RECURSE new_list ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
        SET(dir_list "")
        FOREACH(file_path ${new_list})
            GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
            SET(dir_list ${dir_list} ${dir_path})
        ENDFOREACH()
        LIST(REMOVE_DUPLICATES dir_list)
        SET(${return_list} ${dir_list})
    ENDMACRO()

    HEADER_DIRECTORIES(header_list)

    target_sources(gd32w51x_wifi
        PRIVATE
            ${SRC_WIFI_DRIVER}
    )

    target_include_directories(gd32w51x_wifi
        PRIVATE
            ${header_list}
    )

    target_sources(gd32w51x_wifi
        PRIVATE
            _wifi_version.h
    )

    add_custom_command(OUTPUT _wifi_version.h
        COMMAND ${PROJECT_SOURCE_DIR}/NSPE/Project/WIFI_IOT/wifi_version.bat ${PROJECT_SOURCE_DIR}/NSPE
    )

    add_custom_command(TARGET gd32w51x_wifi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gd32w51x_wifi> ${PROJECT_SOURCE_DIR}/NSPE/WIFI_IOT/lib/GCC/libgd32w51x_wifi.a
    )
endif()