# SPL CCRAM Example

## Description 

This is an extension of the [SPL USART printf example](../gd32-spl-usart).

The purpose is to show how to use the CCRAM ("core-coupled memory (CCM)" or "tightly-coupled memory (TCM)") of certain GD32 chips which have this type of RAM.

Currently this example is for GD32F405, GD32F407 and GD32F450 series chips which feature 64 kilobytes of CCRAM.

## Technicalities

Using CCRAM requires a *conscious effort*, since CCRAM is special in the sense that *only* the Cortex-M core can access the memory via the DBUS, but not some peripherals. 

The linker script template features a [memory definition](https://github.com/CommunityGD32Cores/gd32-pio-spl-package/blob/7ae775696fcef9646b86425af066122519c18563/platformio/ldscripts/tpl/linker.tpl#L18-L23) and two [section defintions](https://github.com/CommunityGD32Cores/gd32-pio-spl-package/blob/7ae775696fcef9646b86425af066122519c18563/platformio/ldscripts/tpl/linker.tpl#L132-L153), which are:
* `.ccram_data`: Section where initialized variables should go
* `.ccram_bss`: Section where unitilialized variables (no initial value) should go, such as buffers

The firmware code can place objects (variables, functions, ..) etc in these sections using GCC's `attribute` feature, as e.g. outlined in [this blogpost](https://mcuoneclipse.com/2012/11/01/defining-variables-at-absolute-addresses-with-gcc/).

This firmware defines two convenience macros to let the user place objects in CCRAM more easily.

```cpp
#define CCRAM_DATA __attribute__ ((section (".ccram_data")))
#define CCRAM_BSS __attribute__ ((section (".ccram_bss")))

//..

uint8_t CCRAM_BSS ccram_buf[64];
int CCRAM_DATA ccram_initialized_data = 1234;
uint8_t CCRAM_DATA ccram_buf2[4] = {1, 2, 3, 4};
```

The CCRAM **must** be initialized before proper usage. The RAM content on startup will be random, meaning the BSS variables (which are usually expected to be zero-initialized on startup) and the "initialized" variables in the data section will all be filled with random data.

The function `init_ccram_bss_and_data()` initialilzes these two sections accordingly. For initialized variables (e.g., `int CCRAM_DATA ccram_initialized_data = 1234;`), the initialization value (`1234` in this case) is stored in flash and must be copied over to the appropriate place in CCRAM. Further, the BSS section must be zeroed.

By using special variables declared in the linker script which mark the start and end of these sections in CCRAM and flash, the function can initialize these sections.

```cpp
void init_ccram_bss_and_data() {
    /* zero-init BSS section */
    uint32_t ccram_bss_length = (uint32_t)((uint8_t*)&__ccram_end_bss__ - (uint8_t*)&__ccram_start_bss__);
    memset(&__ccram_start_bss__, 0, ccram_bss_length);
    /* load ccram_data initialization data from flash. */
    uint32_t ccram_data_length = (uint32_t)((uint8_t*)&_ccram_end_data - (uint8_t*)&_ccram_start_data);;
    memcpy(&_ccram_start_data, &_si_ccram_data, ccram_data_length);
}
```

After calling this function, the CCRAM has the initialized and expected values.

## Expected output

*TODO*. This has not been run on real hardware yet, it's a best-effort implementation.

By default, the output should appear as UART output, 115200 baud, on TX = PA9. If the macro `USE_ALTERNATE_USART0_PINS` is defined, TX = PB6. This is the same as for the SPL-USART demo.