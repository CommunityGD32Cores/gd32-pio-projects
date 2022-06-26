#include <gd32_include.h>
#include <printf_over_x.h>
#include <stdio.h>

// Our bootloader will occupy the first 16 kByte of flash
// which is 0x4000 bytes
// after that we expect the application binary to be flashed
#define APP_START_OFFSET 0x4000u
#define FLASH_START_ADDR 0x8000000u
#define APP_ADDR (uint32_t)(FLASH_START_ADDR + APP_START_OFFSET)

static void __attribute__((naked)) start_app(uint32_t pc, uint32_t sp)
{
    __asm("           \n\
          msr msp, r1 /* load r1 into MSP */\n\
          bx r0       /* branch to the address at r0 */\n\
    ");
}

void simple_delay(uint32_t us)
{
	/* simple delay loop */
	while (us--) {
		asm volatile ("nop");
	}
}

int main(void)
{
    init_printf_transport();

    printf("==== Bootloader start ====\n");
    uint32_t *app_code = (uint32_t *)APP_ADDR;
    // first two entries of the vector table:
    uint32_t app_sp = app_code[0];    // initial SP
    uint32_t app_start = app_code[1]; // Reset_Handler location
    printf("Booting app starting at 0x%08x, initial SP is 0x%08x, entry point 0x%08x\n",
           (unsigned) APP_ADDR, (unsigned) app_sp, (unsigned) app_start);
    printf("Jumping now..\n");
    fflush(stdout);  // make sure all output is flushed out
    simple_delay(10000); // tiny bit of delay (eliminates uart garbage)
    start_app(app_start, app_sp);
    /* Not Reached */
    while (1)
    {
    }
}