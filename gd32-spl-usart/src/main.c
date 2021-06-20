#if defined(GD32F10x)
#include "gd32f10x.h"
#elif defined(GD32F1x0)
#include "gd32f1x0.h"
#elif defined(GD32F3x0)
#include "gd32f3x0.h"
#elif defined(GD32F30x)
#include "gd32f30x.h"
#endif
#include <stdio.h>

/* settings for used USART and pin */
#define RCU_GPIO            RCU_GPIOA
#define RCU_UART            RCU_USART0
#define USART               USART0
#define UART_TX_RX_GPIO     GPIOA
#define UART_TX_GPIO_PIN    GPIO_PIN_9
#define UART_RX_GPIO_PIN    GPIO_PIN_10

/* only for certain series: set pin to alternate function x for UART */
#if defined(GD32F3x0) || defined(GD32F1x0)
#define UART_TX_AF  GPIO_AF_1
#define UART_RX_AF  GPIO_AF_1
#endif

void simple_delay(uint32_t us)
{
    /* simple delay loop */
    while (us--)
    {
        asm volatile("nop");
    }
}

int main(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIO);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_UART);

    /* connect port to USARTx_Tx and USARTx_Rx  */
#if defined(GD32F3x0) || defined(GD32F1x0)
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

    int i = 0;
    while (1)
    {
        printf("a usart transmit test example! iteration %d\n", i++);
        simple_delay(10000000UL);
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