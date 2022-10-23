
cmake_minimum_required(VERSION 3.15)
set(TARGET_EXE mbl-ns)
set(TAGET_PROJECT_DIR ${PROJECT_SOURCE_DIR}/MBL/Project)
add_executable(${TARGET_EXE})

add_definitions(
    -DPLATFORM_GDM32
)

set_target_properties(${TARGET_EXE}
    PROPERTIES
        SUFFIX ".axf"
        ARCHIVE_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/lib"
        LIBRARY_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/lib"
        RUNTIME_OUTPUT_DIRECTORY "${TAGET_PROJECT_DIR}/GCC/output/bin"
)

target_sources(${TARGET_EXE}
    PRIVATE
        mbl_api.c
        mbl_flash.c
        mbl_image.c
        mbl_ns.c
        mbl_qspi_flash.c
        mbl_sys.c
        mbl_uart.c
    )

target_sources(${TARGET_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_fmc.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_fwdgt.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_gpio.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_icache.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_misc.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_qspi.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_rcu.c
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Source/gd32w51x_usart.c
    )

target_sources(${TARGET_EXE}
    PRIVATE
        ../platform/gdm32/cmsis_core/mbl_system_gdm32.c
        ../platform/gdm32/gcc/mbl_startup_gdm32.s
    )

target_include_directories(${TARGET_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/NSPE/Firmware/GD32W51x_standard_peripheral/Include
)

target_add_scatter_file(${TARGET_EXE}
    ../platform/gdm32/gcc/mbl_gdm32_ns.ld
)


target_link_options(${TARGET_EXE}
    PUBLIC
    -Wl,--just-symbols=${PROJECT_SOURCE_DIR}/ROM-EXPORT/symbol/rom_symbol.gcc
    -Wl,-Map=${RUNTIME_OUTPUT_DIRECTORY}/${TARGET_EXE}.map
)

add_custom_command(TARGET ${TARGET_EXE} POST_BUILD
    COMMAND ${TAGET_PROJECT_DIR}/mbl_afterbuild.bat ${TAGET_PROJECT_DIR}/GCC/output/bin ${TARGET_EXE} GCC \"\"
    COMMENT "Building ${TARGET_EXE}"
)
