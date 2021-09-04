#if defined(GD32F10x)
#include "gd32f10x.h"
#elif defined(GD32F1x0)
#include "gd32f1x0.h"
#elif defined(GD32F3x0)
#include "gd32f3x0.h"
#elif defined(GD32F30x)
#include "gd32f30x.h"
#elif defined(GD32F4xx)
#include "gd32f4xx.h"
#elif defined(GD32F403)
#include "gd32f403.h"
#elif defined(GD32E10X)
#include "gd32e10x.h"
#else
#error "Unknown chip series"
#endif
#include <stdio.h>
#include <string.h>

#ifndef USE_ALTERNATE_USART0_PINS
/* settings for used USART (UASRT0) and pins, TX = PA9, RX = PA10 */
#define RCU_GPIO            RCU_GPIOA
#define RCU_UART            RCU_USART0
#define USART               USART0
#define UART_TX_RX_GPIO     GPIOA
#define UART_TX_GPIO_PIN    GPIO_PIN_9
#define UART_RX_GPIO_PIN    GPIO_PIN_10

/* only for certain series: set pin to alternate function x for UART */
#if defined(GD32F3x0) || defined(GD32F1x0) || defined(GD32F4xx)
#define UART_TX_AF  GPIO_AF_1
#define UART_RX_AF  GPIO_AF_1
#endif
#else
/* settings for USART0 alternate settings, TX = PB6, RX = PB7 */
#define RCU_GPIO            RCU_GPIOB
#define RCU_UART            RCU_USART0
#define USART               USART0
#define UART_TX_RX_GPIO     GPIOB
#define UART_TX_GPIO_PIN    GPIO_PIN_6
#define UART_RX_GPIO_PIN    GPIO_PIN_7

/* only for certain series: set pin to alternate function x for UART */
#if defined(GD32F3x0) || defined(GD32F1x0) || defined(GD32F4xx)
#define UART_TX_AF  GPIO_AF_0 /* PB6 AF0 is USART0_TX */
#define UART_RX_AF  GPIO_AF_0 /* PB7 AF0 is USART0_RX */
#endif
#endif 

void systick_config(void);
void delay_1ms(uint32_t count);

void hexDump(const char * desc, const void * addr, const int len) {
    const int perLine = 16;
    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;
    // Output description if given.
    if (desc != NULL) printf ("%s:\n", desc);
    // Length checks.
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).
        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.
            if (i != 0) printf ("  %s (%d bytes)\n", buff, len);
            // Output the offset of current line.
            printf ("  %04x ", i);
        }
        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);
        // And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }
    // Pad out last line if not exactly perLine characters.
    while ((i % perLine) != 0) {
        printf ("   ");
        i++;
    }
    // And print the final ASCII buffer.
    printf ("  %s\n", buff);
}

/* see https://interrupt.memfault.com/blog/how-to-write-linker-scripts-for-firmware for details */
/* sections for CCRAM are listed in the linker script template, currently */
/* https://github.com/CommunityGD32Cores/gd32-pio-spl-package/blob/7ae775696fcef9646b86425af066122519c18563/platformio/ldscripts/tpl/linker.tpl#L132-L153 */

/* define convenience macros to place variables into CCRAM sections */
/* CCRAM_DATA is for initialized variables, CCRAM_BSS for uninitialized variables*/
#define CCRAM_DATA __attribute__ ((section (".ccram_data")))
#define CCRAM_BSS __attribute__ ((section (".ccram_bss")))

uint8_t CCRAM_BSS ccram_buf[64];
int CCRAM_DATA ccram_initialized_data = 1234;
uint8_t CCRAM_DATA ccram_buf2[4] = {1, 2, 3, 4};

extern int _si_ccram_data; /* provided by linker script, start of initialization data in flash  */
extern int __ccram_start_bss__; /* provided by linker script, start of uninitialized objects in CCRAM */
extern int __ccram_end_bss__; /* provided by linker script, end of uninitialized objects in CCRAM */
extern int _ccram_start_data; /* provided by linker script, start of initialized objects in CCRAM */
extern int _ccram_end_data; /* provided by linker script, end of initialized objects in CCRAM */

/* init function basically from https://vivonomicon.com/2020/09/10/bare-metal-stm32-programming-part-13-running-temporary-ram-programs-and-using-tightly-coupled-memories/ */
void init_ccram_bss_and_data() {
    /* zero-init BSS section */
    uint32_t ccram_bss_length = (uint32_t)((uint8_t*)&__ccram_end_bss__ - (uint8_t*)&__ccram_start_bss__);
    memset(&__ccram_start_bss__, 0, ccram_bss_length);
    /* load ccram_data initialization data from flash. */
    uint32_t ccram_data_length = (uint32_t)((uint8_t*)&_ccram_end_data - (uint8_t*)&_ccram_start_data);;
    memcpy(&_ccram_start_data, &_si_ccram_data, ccram_data_length);
}

int main(void)
{
    /* configure systick for more precise delays */
    systick_config();

    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIO);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART);

    /* connect port to USARTx_Tx and USARTx_Rx  */
#if defined(GD32F3x0) || defined(GD32F1x0) || defined(GD32F4xx)
    gpio_af_set(UART_TX_RX_GPIO, UART_TX_AF, UART_TX_GPIO_PIN);
    gpio_af_set(UART_TX_RX_GPIO, UART_RX_AF, UART_RX_GPIO_PIN);

    gpio_mode_set(UART_TX_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART_TX_GPIO_PIN);
    gpio_output_options_set(UART_TX_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_TX_GPIO_PIN);
    gpio_mode_set(UART_TX_RX_GPIO, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART_RX_GPIO_PIN);
    gpio_output_options_set(UART_TX_RX_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_RX_GPIO_PIN);
#else /* valid for GD32F10x, GD32F30x */
    gpio_init(UART_TX_RX_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, UART_TX_GPIO_PIN);
    gpio_init(UART_TX_RX_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, UART_RX_GPIO_PIN);
#endif

    /* USART configure 115200 8N1 */
    usart_deinit(USART);
    usart_word_length_set(USART, USART_WL_8BIT);
    usart_stop_bit_set(USART, USART_STB_1BIT);
    usart_parity_config(USART, USART_PM_NONE);
    usart_baudrate_set(USART, 115200U);
    usart_receive_config(USART, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART, USART_TRANSMIT_ENABLE);
    usart_enable(USART);

    /* short delay to make sure that the serial monitor was started before we print something */
    delay_1ms(500);

    printf("CCRAM Demo!\n");
    printf("This demo shows how to use CCRAM, starting at 0x10000000.\n");
    printf("General information:\n");

    printf("Start of CCRAM initialization data in flash: %p\n", &_si_ccram_data);
    printf("Start of uninitialized objects in CCRAM: %p\n", &__ccram_start_bss__);
    printf("End of uninitialized objects in CCRAM: %p\n", &__ccram_end_bss__);
    printf("Start of initialized objects in CCRAM: %p\n", &_ccram_start_data);
    printf("Start of initialized objects in CCRAM: %p\n", &_ccram_end_data);

    printf("Printing CCRAM variables and buffers before CCRAM initialization..\n");

    hexDump("CCRAM buffer (unitialized, BSS)", ccram_buf, (int)sizeof(ccram_buf));
    hexDump("CCRAM buffer (initialized, DATA)", ccram_buf2, (int)sizeof(ccram_buf2));
    printf("CCRAM initialized variable: %d\n", ccram_initialized_data);

    printf("Calling CCRAM init function now..\n");
    init_ccram_bss_and_data();
    printf("..done. BSS segment should be zeroed, DATA segment should be initialized.\n");
    printf("Priting CCRAM variables again.\n");

    hexDump("CCRAM buffer (unitialized, BSS)", ccram_buf, (int)sizeof(ccram_buf));
    hexDump("CCRAM buffer (initialized, DATA)", ccram_buf2, (int)sizeof(ccram_buf2));
    printf("CCRAM initialized variable: %d\n", ccram_initialized_data);

    while (1)
    {
        printf("CCRAM Demo ended! Press reset to restart.\n");
        delay_1ms(1000);
    }
}

/* retarget the gcc's C library printf function to the USART */
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        usart_data_transmit(USART, (uint8_t)data[i]);
        while (RESET == usart_flag_get(USART, USART_FLAG_TBE))
            ;
    }

    // return # of bytes written - as best we can tell
    return len;
}

/* code for delaying using the SysTick */

volatile static uint32_t delay;

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        /* capture error */
        while (1)
        {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_1ms(uint32_t count)
{
    delay = count;

    while (0U != delay)
    {
    }
}

void delay_decrement(void)
{
    if (0U != delay)
    {
        delay--;
    }
}

void SysTick_Handler(void)
{
    delay_decrement();
}
