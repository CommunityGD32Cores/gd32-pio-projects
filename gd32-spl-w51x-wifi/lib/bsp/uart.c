/*!
    \file    uart.c
    \brief   UART BSP for GD32W51x WiFi SDK

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

#include <stdio.h>
#include <ctype.h>
#include "app_cfg.h"
#include "bsp_inc.h"
#include "uart.h"
#include "malloc.h"
#include "wifi_netlink.h"
#include "wakelock.h"
#include "console.h"

char uart_buf[UART_BUFFER_SIZE];
int bsp_uart_index = 0;

#if defined(__ARMCC_VERSION)
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)ch);

    return ch;
}
#elif defined(__ICCARM__)
int putchar(int ch)
{
    /* Send byte to USART */
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)ch);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
int _write(int fd, char *str, int len)
{
    (void)fd;
    int32_t i = 0;

    /* Send string and return the number of characters written */
    while (i != len) {
        while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
        usart_data_transmit(LOG_UART, *str);
        str++;
        i++;
    }

    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TC));

    return i;
}
#endif

/*!
    \brief      configure usart
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_config(uint32_t usart_periph)
{
    if (usart_periph == USART0) {
        rcu_periph_clock_enable(RCU_USART0);
        rcu_periph_clock_enable(RCU_GPIOA);
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_15);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_9);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_15);
    } else if (usart_periph == USART1) {
        rcu_periph_clock_enable(RCU_USART1);
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_GPIOB);
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
    }

    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, 115200);
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(usart_periph, USART_INT_RBNE);
    usart_enable(usart_periph);
}

/*!
    \brief      reconfigure usart
    \param[in]  uart_conf: pointer to the input structure
                  usart_periph: USARTx(x=0,1,2)
                  baudrate: baud rate value
                  databits: USART word length configure
                  only one parameter can be selected which is shown as below:
                    \arg        USART_WL_8BIT: 8 bits
                    \arg        USART_WL_9BIT: 9 bits
                  stopbits: USART stop bit configure
                  only one parameter can be selected which is shown as below:
                    \arg        USART_STB_1BIT: 1 bit
                    \arg        USART_STB_0_5BIT: 0.5bit
                    \arg        USART_STB_2BIT: 2 bits
                    \arg        USART_STB_1_5BIT: 1.5bit
                  parity: USART parity configure
                  only one parameter can be selected which is shown as below:
                    \arg        USART_PM_NONE: no parity
                    \arg        USART_PM_ODD: odd parity
                    \arg        USART_PM_EVEN: even parity
                  flow_ctrl: flow control
                  only one parameter can be selected which is shown as below:
                    \arg        USART_RTS_ENABLE: enable RTS
                    \arg        USART_RTS_DISABLE: disable RTS
                    \arg        USART_CTS_ENABLE:  enable CTS
                    \arg        USART_CTS_DISABLE: disable CTS
    \param[out] none
    \retval     none
*/
void usart_reconfig(uart_config_t *uart_conf)
{
    usart_tx_idle_wait(uart_conf->usart_periph);
    usart_deinit(uart_conf->usart_periph);
    usart_baudrate_set(uart_conf->usart_periph, uart_conf->baudrate);
    usart_word_length_set(uart_conf->usart_periph, uart_conf->databits);
    usart_stop_bit_set(uart_conf->usart_periph, uart_conf->stopbits);
    usart_parity_config(uart_conf->usart_periph, uart_conf->parity);
    usart_hardware_flow_cts_config(uart_conf->usart_periph, uart_conf->flow_ctrl);
    usart_hardware_flow_rts_config(uart_conf->usart_periph, uart_conf->flow_ctrl);
    usart_receive_config(uart_conf->usart_periph, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart_conf->usart_periph, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(uart_conf->usart_periph, USART_INT_RBNE);
    usart_enable(uart_conf->usart_periph);

}

/*!
    \brief      wait usart transmission completed
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_tx_idle_wait(uint32_t usart_periph)
{
    while (RESET == usart_flag_get(usart_periph, USART_FLAG_TC));
}

/*!
    \brief      wait LOG_UART usart transmit data register empty and transmit a data
    \param[in]  c: transmit data
    \param[out] none
    \retval     none
*/
void uart_putc_noint(uint8_t c)
{
    while (RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
    usart_data_transmit(LOG_UART, (uint8_t)c);
}

/*!
    \brief      wait LOG_UART usart transmit data register empty and transmit data
    \param[in]  s: pointer to the transmit datas
    \param[out] none
    \retval     none
*/
void uart_puts_noint(const char *s)
{
    if (*s == 0) {
        return;
    }

    while (1) {
        while (RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
        usart_data_transmit(LOG_UART, *s++);
        if (*s == 0) {
            return;
        }
    }
}

/*!
    \brief      wait LOG_UART usart transmit data register empty and transmit data
    \param[in]  d: pointer to the transmit data
    \param[in]  size: put data length
    \param[out] none
    \retval     none
*/
void uart_put_data_noint(const uint8_t *d, int size)
{
    if (size == 0){
        return;
    }

    while (1) {
        while (RESET == usart_flag_get(LOG_UART, USART_FLAG_TBE));
        usart_data_transmit(LOG_UART, *d++);
        size--;
        if (size == 0) {
            return;
        }
    }
}

/*!
    \brief      log uart interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void log_uart_interrupt_handler(void) SECTION_RAM_CODE
{
    uint8_t rx_char;
    char ch;
    os_task_t *receiver_tcb;
    WIFI_MESSAGE_TYPE_E msg_type;

    if ((RESET != usart_interrupt_flag_get(LOG_UART, USART_INT_FLAG_RBNE)) &&
        (RESET != usart_flag_get(LOG_UART, USART_FLAG_RBNE))) {
        rx_char = (uint8_t)usart_data_receive(LOG_UART);
#ifndef CONFIG_USE_GD32F4XX_UART
        if (RESET != usart_flag_get(LOG_UART, USART_FLAG_ORERR)) {
            usart_flag_clear(LOG_UART, USART_FLAG_ORERR);
        }
#endif
        ch = (char)rx_char;

        if (isprint(ch)) {
            if (bsp_uart_index == 0) {
                sys_wakelock_acquire(LOCK_ID_USART);
            }
            uart_buf[bsp_uart_index++] = ch;
            if (bsp_uart_index >= UART_BUFFER_SIZE)
                bsp_uart_index = 0;
            uart_putc_noint(ch);
        } else if (ch == '\r') { /* putty doesn't transmit '\n' */
            int32_t msg_waiting = 0;

            uart_buf[bsp_uart_index] = '\0';
            bsp_uart_index = 0;

            msg_type = WIFI_MESSAGE_SHELL_COMMAND;
            receiver_tcb = &console_task_tcb;

            msg_waiting = sys_task_msg_num(receiver_tcb, 1);
            if ((msg_waiting >= 0) && (msg_waiting < (CONSOLE_TASK_QUEUE_SIZE - 1))) {
                sys_task_post(receiver_tcb, &msg_type, 1);
            }

            sys_wakelock_release(LOCK_ID_USART);
        } else if (ch == '\b') { /* non-destructive backspace */
            if (bsp_uart_index > 0)
                uart_buf[--bsp_uart_index] = '\0';
            uart_putc_noint(ch);
        }
    }
}

#if 0
/*!
    \brief      uart2 interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void uart2_interrupt_handler(void)
{
        uint8_t rx_char;
        char ch;
    OS_ERR   err;
    void    *msg_ptr = NULL;
    OS_MSG_SIZE msg_size = 0;
    OS_TC   *receiver_tcb;
    WIFI_MESSAGE_T *wifi_msg;

    if (uart_get_intr(UART2, UART_IID_Rx_Line_Status)) {
        if (uart_get_status(UART2, UART_LSR_Frame_Err | UART_LSR_Parity_Err | UART_LSR_Overrun_Err)) {

        }
    }

    if (uart_get_intr(UART2, UART_IID_Rx_Data_Available)) {
           rx_char = uart_recv_data(UART2);
           ch = (char)rx_char;
           if(ch == '\r') { // enter
                uart_buf[index] = '\0';
                index=0;

            wifi_msg = (WIFI_MESSAGE_T *)sys_malloc(sizeof(WIFI_MESSAGE_T));

            wifi_msg->msg_type = WIFI_MESSAGE_SHELL_COMMAND;
            wifi_msg->msg_data = NULL;

            receiver_tcb = &console_task_tcb;
            msg_size = sizeof(*wifi_msg);
            msg_ptr = (void *)wifi_msg;
            OSTaskQPost(receiver_tcb, msg_ptr, msg_size, OS_OPT_POST_FIFO, &err);
            }
            if(ch == '\b') { // backspace
            if(index>0)
                uart_buf[--index] = '\0';
            }
            if((ch > 31) && (ch < 128)) {
                uart_buf[index++] = ch;
                if(index >= UART_BUFFER_SIZE)
                    index=0;
                uart_putc_noint(ch);
            }
    }
}
#endif
