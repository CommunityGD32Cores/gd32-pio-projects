/*!
    \file    bsp_wlan.h
    \brief   WLAN related BSP for GD32W51x WiFi SDK

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

#ifndef _BSP_WLAN_H_
#define _BSP_WLAN_H_

/*============================ INCLUDES ======================================*/
#include "app_type.h"
#include "delay.h"
#include "dma.h"
#include "init_rom.h"

/*============================ MACROS ========================================*/
#define RF_BASE    0x40017800

#define NVIC_CCR   ((volatile unsigned long *)(0xE000ED14))

#ifdef CONFIG_PLATFORM_ASIC
#define RTC_CLOCK_SOURCE_IRC32K
#else
#define RTC_CLOCK_SOURCE_HXTAL_DIV_RTCDIV
#endif

#define WLAN_WAKEUP_EXTI_LINE           EXTI_20
#define RTC_WAKEUP_EXTI_LINE            EXTI_24
#if defined(CONFIG_BOARD) && (CONFIG_BOARD == PLATFORM_BOARD_32W515P_EVAL)
#define LOG_USART_RX_PIN_EXTI_LINE      EXTI_11
#else
#define LOG_USART_RX_PIN_EXTI_LINE      EXTI_8
#endif

/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/

struct time_rtc {
    uint32_t tv_sec;         /* seconds */
    uint32_t tv_msec;        /* and miliseconds */
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
void wifi_pmu_config(void);
void wifi_rcu_config(void);
void wifi_memmap_config(void);
void wifi_setting_config(void);
void hw_sec_engine_enable(void);
void set_rf_standby_mode(void);
void set_rf_tx_mode(void);
void set_rf_shutdown_mode(void);
void reset_rf(void);
void set_rf_power_on(void);
void pass_rf_ctrl_to_bb(void);
void reset_wlan_adc(void);
uint8_t wifi_irq_num_get(uint8_t int_type);
void wifi_power_state_set(uint32_t power_state);
void wifi_waked_wait(void);
void wifi_lpds_set(void);
void wifi_lpds_exit(void);
void wifi_lpds_on_clear(void);
void wifi_clock_gate(void);
void wifi_clock_enable(void);
void rf_pll_cal_trigger(void);
void hw_cryp_aes_ecb(uint8_t mode, uint8_t *key, uint16_t key_size, uint8_t *input, uint32_t in_length, uint8_t *output);
void hw_hmac_sha1(uint8_t *key, uint32_t key_size, uint8_t *input, uint32_t in_length, uint8_t output[20]);
void hw_hmac_sha256(uint8_t *key, uint32_t key_size, uint8_t *input, uint32_t in_length, uint8_t output[32]);
void wifi_wakelock_acquire(void);
void wifi_wakelock_release(void);
void deep_sleep_enter(uint16_t sleep_time);
void deep_sleep_exit(void);
void rtc_32k_time_get(struct time_rtc *cur_time, uint32_t is_wakeup);
void lpds_preconfig(uint8_t settle_time);
void deepsleep_wfi_set(void);
void ad_spi_transmit(uint32_t data);
uint8_t ad_spi_receive(uint32_t data);

#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
void spi_write(uint16_t add, uint32_t data);
uint32_t spi_read(uint16_t add);
void spi_config(uint32_t spi_periph);
#endif
void hw_crypt_lock_init(void);
extern int co_printf(const char *format, ...);
void wifi_rcu_deinit(void);
void platform_init(void);
void sys_reset_flag_check(void);

/*============================ IMPLEMENTATION ================================*/

#endif  // _BSP_WLAN_H_
