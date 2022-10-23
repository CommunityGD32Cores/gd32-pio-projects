/*!
    \file    mbl_uart.c
    \brief   Non-secure MBL uart file for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "platform_def.h"
#include "gd32w51x.h"
#include "mbl_trace.h"


/*!
    \brief      send byte to usart
    \param[in]  c: data to transmission
    \param[out] none
    \retval     none
*/
void uart_putc(uint8_t c)
{
    while (RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)c);
}

#if defined(__ARMCC_VERSION)
FILE __stdout;

/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
    /* Send byte to USART */
    uart_putc(ch);

    /* Return character written */
    return ch;
}
#elif defined(__ICCARM__)
int putchar(int ch)
{
    /* Send byte to USART */
    uart_putc(ch);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
int _write(int fd, char *str, int len)
{
    (void)fd;
    uint32_t i = 0;

    /* Send string and return the number of characters written */
    for (i = 0; i < len; i ++) {
        uart_putc(*str++);
    }
    return len;
}
#endif

/*!
    \brief      configure usart
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void uart_config(uint32_t usart_periph)
{
    if (usart_periph == USART1) {
        rcu_periph_clock_enable(RCU_USART1);
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_GPIOA);

        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_15);
        gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_8);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_15);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_8);
    } else if (usart_periph == USART2) {
        rcu_periph_clock_enable(RCU_USART2);
        rcu_periph_clock_enable(RCU_GPIOB);

        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_10);
        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_11);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_10);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_11);
    } else {
        return;
    }

    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, 115200U);
    //usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
    usart_enable(usart_periph);

    //usart_interrupt_enable(usart_periph, USART_INT_RBNE);
}

/*!
    \brief      initialize log usart
    \param[in]  none
    \param[out] none
    \retval     none
*/
void log_uart_init(void)
{
    uart_config(LOG_UART);
}

/*!
    \brief      wait log usart transmit complete
    \param[in]  none
    \param[out] none
    \retval     none
*/
void log_uart_idle_wait(void)
{
    while (RESET == usart_flag_get(LOG_UART, USART_FLAG_TC));
}
