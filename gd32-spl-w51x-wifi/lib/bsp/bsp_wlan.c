/*!
    \file    bsp_wlan.c
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

#include "app_cfg.h"
#include "app_type.h"
#include "nspe_region.h"
#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_inc.h"
#include "bsp_wlan.h"
#include "soc_intf.h"
#include "wakelock.h"
#include "wlan_intf_def.h"
#if defined(CONFIG_TZ_ENABLED)
#include "mbl_nsc_api.h"
#endif
#include "delay.h"
#include "uart.h"
#include "dma.h"
#include "init_rom.h"
#include "trng.h"

__IO uint32_t prescaler_a = 0, prescaler_s = 0;
static void rtc_pre_config(void);

#include "bsp_gd32w51x.c.in"

/*!
    \brief      configure RTC pre-configure
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rtc_pre_config(void)
{
    #if defined(RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

          prescaler_s = 0x3E7;  // 0x13F;
          prescaler_a = 0x1F;  // 0x63;
    #elif defined(RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #elif defined(RTC_CLOCK_SOURCE_HXTAL_DIV_RTCDIV)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          /* 32K clock selection */
          rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV_RTCDIV);
    #else
    #error RTC clock source should be defined.
    #endif  /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / OS_TICK_RATE_HZ)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0f);

    /* configure the PendSV handler priority */
    NVIC_SetPriority(PendSV_IRQn, 0x0f);
}

void wifi_memmap_config(void)
{
    wifi_set_reg_base(WIFI_BASE);
    wifi_set_dump_sram_base(0x20068000);// offset 416KB
    wifi_set_nvic_base(NVIC_BASE);
}

/*!
    \brief      configure wifi setting
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_setting_config(void)
{
    wifi_param_t wifi_param_set;

    wifi_memmap_config();
#if PLATFORM_CRYSTAL == CRYSTAL_40M
    wifi_param_set.crystal_freq = 40;
#elif PLATFORM_CRYSTAL == CRYSTAL_26M
    wifi_param_set.crystal_freq = 26;
#else
    wifi_param_set.crystal_freq = 40;
#endif
    wifi_set_param(&wifi_param_set);
    // DEBUGPRINT("Crystal Frequency is %dMHz\r\n",wifi_param_set.crystal_freq);
}

/*!
    \brief      get wifi irq number
    \param[in]  int_type: wifi irq type
                only one parameter can be selected which is shown as below:
      \arg        0: WLAN_Rx_IRQn
      \arg        1: WLAN_Tx_IRQn
      \arg        2: WLAN_Cmn_IRQn
      \arg        3: WLAN_WKUP_IRQn
      \arg        other: 0xff
    \param[out] none
    \retval     wifi irq number: WLAN_Rx_IRQn, WLAN_Tx_IRQn, WLAN_Cmn_IRQn, WLAN_WKUP_IRQn or 0xff
*/
uint8_t wifi_irq_num_get(uint8_t int_type)
{
    IRQn_Type irqn;

    switch (int_type) {
    case 0:
        irqn = WLAN_Rx_IRQn;
        break;
    case 1:
        irqn = WLAN_Tx_IRQn;
        break;
    case 2:
        irqn = WLAN_Cmn_IRQn;
        break;
    case 3:
        irqn = WLAN_WKUP_IRQn;
        break;
    default:
        irqn = (IRQn_Type)0xff;
        break;
    }

    return (uint8_t)irqn;
}

/*!
    \brief      acquire wifi wakelock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_wakelock_acquire(void)
{
    sys_wakelock_acquire(LOCK_ID_WLAN);
}

/*!
    \brief      release wifi wakelock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_wakelock_release(void)
{
    sys_wakelock_release(LOCK_ID_WLAN);
}

/*!
    \brief      clear exti line
    \param[in]  line_no: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
      \arg        EXTI_x (x=0..28): EXTI line x
    \param[out] none
    \retval     none
*/
static void exti_line_clear(exti_line_enum line_no)
{
    /* disable interrupt mask */
    exti_interrupt_disable(line_no);

    /* clear pending interrupt bit */
    if (SET == exti_flag_get(line_no)) {
        exti_interrupt_flag_clear(line_no);
    }
}

/*!
    \brief      enter deep sleep mode
    \param[in]  sleep_time: sleep time(0x0000-0x7fff)
    \param[out] none
    \retval     none
*/
void deep_sleep_enter(uint16_t sleep_time)
{
    exti_init(LOG_USART_RX_PIN_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    exti_init(WLAN_WAKEUP_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    exti_init(RTC_WAKEUP_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    rtc_flag_clear(RTC_STAT_WTF);
    rtc_wakeup_disable();
    rtc_wakeup_timer_set(sleep_time * 2);  /* unit: 500us */
    rtc_wakeup_enable();

    trng_close();

    /* wait usart transmition complete */
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TC));

    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
}
/*!
    \brief      exit deep sleep mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void deep_sleep_exit(void)
{
    exti_line_clear(LOG_USART_RX_PIN_EXTI_LINE);
    exti_line_clear(WLAN_WAKEUP_EXTI_LINE);
    exti_line_clear(RTC_WAKEUP_EXTI_LINE);

    if(RESET != rtc_flag_get(RTC_STAT_WTF)) {
        rtc_flag_clear(RTC_STAT_WTF);
    }

    system_clock_config_nspe();
}


/*!
    \brief      get rtc 32k time
    \param[in]  cur_time: pointer to the input structure
                  tv_sec: second of current time
                  tv_msec: miliseconds of current time
     \param[in]  is_wakeup: wakeup state
    \param[out] none
    \retval     none
*/
void rtc_32k_time_get(struct time_rtc *cur_time, uint32_t is_wakeup)
{
    rtc_parameter_struct rtc_time;
    uint32_t second;
    uint32_t sub_second;

    if (is_wakeup) {
        rtc_register_sync_wait();
    }

    /* if sub_second is 0, rtc_time is most likely be updated */
    do {
        sub_second = rtc_subsecond_get();
    } while (sub_second == 0);

    rtc_current_time_get(&rtc_time);
    second = ((rtc_time.second >> 4) * 10) + (rtc_time.second & 0x0f);

    cur_time->tv_sec = second;
    if (sub_second <= 1000) {
        cur_time->tv_msec = 1000 - sub_second;
    } else {
        DEBUGPRINT("rtc sub second unexpected\r\n");
        cur_time->tv_msec = 0;
    }
}

/*!
    \brief      set lpds preconfig
    \param[in]  settle_time: time want to set
    \param[out] none
    \retval     none
*/
void lpds_preconfig(uint8_t settle_time)
{
    /* set wakeup time of WLAN_OFF domain to max 64us (if 32101 HW bug fixed, default 0x20 is ok) */
    // REG32(PMU + 0x10) |= 0xFF00;

    /* set HXTAL settle time */
    REG32(PMU + 0x24) &= 0xFFFFFF00;
    REG32(PMU + 0x24) |= settle_time;
}

/*!
    \brief      set deep sleep WFI
    \param[in]  none
    \param[out] none
    \retval     none
*/
void deepsleep_wfi_set(void)
{
    /* set wakeup time of WLAN_OFF domain to max 64us (if 32101 HW bug fixed, default 0x20 is ok) */
    // REG32(PMU + 0x10) |= 0xFF00;

    /* set HXTAL settle time to 500us (HXTAL off in deepsleep, so need time to on when wakeup) */
    REG32(PMU + 0x24) &= 0xFFFFFF00;
    REG32(PMU + 0x24) |= 0x04;

    exti_interrupt_flag_clear(EXTI_20);
    exti_init(EXTI_20, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    trng_close();
    // wait usart transmition complete
    while(RESET == usart_flag_get(LOG_UART, USART_FLAG_TC));
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
}
