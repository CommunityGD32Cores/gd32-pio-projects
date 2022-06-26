# SPL Bootloader example

## Description 

This demo showcases how to create a bootloader and application pair within the PlatformIO GD32 integration.

## Technicalities

Please read the article https://interrupt.memfault.com/blog/how-to-write-a-bootloader-from-scratch.

The `platformio.ini` basically configures the build settings so that the bootloader and applications are at the correct addresses and maximum sizes. By grouping repeadedly needed settings into its own environment section which is then inherited from other environment using [`extends`](https://docs.platformio.org/en/latest/projectconf/section_env_advanced.html#extends) keeps things nice and tidy.

The general chosen settings for the bootloader are:
* Bootloader size: 16 kilobytes (= 0x4000 bytes)
* Bootloader loads application from address 0x8004000
* loading is performed without any special firmware header, it expects a "regular" firmware at this address that directly starts with the vector table, whose first two entries are the initial stack pointer and the address of the first to be executed instruction (i.e., in `Reset_Handler` in the startup assembly file)

The specific settings are:
* `board_upload.maximum_size`: Sets maximum firmware image size (size of .bin) file that is allowed to generate. Since we chose the bootloader to be 16kBytes big, for a 64K chip, the booloader has 16K and the app has 48K of flash space. By default, this is the full flash size of the chip.
* [`build_src_filter`](https://docs.platformio.org/en/latest/projectconf/section_env_build.html#build-src-filter): The project's `src/` folder is organized so that all source files for the bootloader is under `src/bootloader`, and all app sources are in `src/application`. By using a soure filter, we can create a single project that builds effectively two firmwares -- individual environments can just set their source filter to exclude or include a specific set of files. So, when we want to build the bootloader, we exclude the application using a `+<*> -<application/>` filter.
* `board_upload.offset_address`: By default 0x8000000, so no offset. The platform-gd32 scripts react to this in two ways: When the "Upload" task is used, it uploads the `.bin` file to the specified address. Second, the SPL builder script generates the linker script with this flash origin address ([source](https://github.com/CommunityGD32Cores/platform-gd32/blob/417eefefbf5e4c66bf1fd7a923a8cef8b68ae7aa/builder/frameworks/spl.py#L89-L104)) -- this makes the `.elf` file link with an effective new start address. The interrupt vector and all the rest of the firmware will start at this address.
* `app_build_flags = -DVECT_TAB_OFFSET=0x4000`: The value of this attribute is later included in a [`build_flags`](https://docs.platformio.org/en/latest/projectconf/section_env_build.html#build-flags) expression using `build_flags = ${application_settings.app_build_flags}`. This activates the macro `VECT_TAB_OFFSET` with value `0x4000` (=16*1024). The SPL framework code reacts [here](https://github.com/CommunityGD32Cores/gd32-pio-spl-package/blob/main/gd32/cmsis/variants/gd32f3x0/system_gd32f3x0.c#L43-L45) and [here](https://github.com/CommunityGD32Cores/gd32-pio-spl-package/blob/9e9c16ba8574e88894f7e13ac4c1e4758cce1a15/gd32/cmsis/variants/gd32f3x0/system_gd32f3x0.c#L208-L212) to the value of this macro and will set the vector table address offset to this value. Note that this must be an **offset** as interpreted from 0x8000000, not an absolute address. If the offset is to be interpreted as an offset from SRAM (vector table in SRAM instead of flash), additionally activate the macro `VECT_TAB_SRAM`.  Setting the vector table address is crucial for interrupts to reach the functions actually registerd in the application firmware, and not the bootloader. The firmware tests this by making use of the SysTick interrupt for the delay function.

## Adding new boards to this project

This project supports all GD32 ARM chips.

Duplicate the `genericGD32F103C8_bootloader` and `genericGD32F103C8_application` under a new name (with changed chip name), then, in the `_application` environment, change the `board` value in both environments (e.g. `board = genericGD32E503C8`) and fixup the `board_upload.maximum_size` to 16*1024 - \<flash chip size in bytes\>.

## Observing the output

The output is configured in the same way as in the [spl-usart](../gd32-spl-usart) example. Make sure to connect the USB-UART adapter to the configured pins: With no `USE_ALTERNATE_USART0_PINS`: TX=PA9, otherwise PB6.

You must upload **both** the `_bootloader` and the `_application` firmware for this to work correctly using the [project tasks](https://docs.platformio.org/en/latest/integration/ide/vscode.html#project-tasks) for these environments.

After uploading bootloader and application and using the "Monitor" project task, you should press the reset button to start from the beginning again.

First, the bootlaoder will start and load the stack pointer and entry point address from the expected addresses in flash (0x8004000 and 0x8004004). 
It will print those, then jump to the application's entry point. 

The startup routine of the application will set the vector table address to itself (0x8004000), then start printing its own output.

```
==== Bootloader start ====
Booting app starting at 0x08004000, initial SP is 0x20002000, entry point 0x08004231
Jumping now..
==== Application start ====
** Application is alive (0) **
** Application is alive (1) **
** Application is alive (2) **
```