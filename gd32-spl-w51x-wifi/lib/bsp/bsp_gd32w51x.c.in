/*!
    \file    bsp_gd32w51x.c
    \brief   BSP functions for GD32W51x WiFi SDK

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

#if defined(CONFIG_BOARD) && (CONFIG_BOARD == PLATFORM_BOARD_32W515P_EVAL)
#define LOG_UART_IRQn           USART2_IRQn
#define UART_RXPIN_EXTI_IRQn    EXTI10_15_IRQn
#define UART_RXPIN_EXTI_PORT    EXTI_SOURCE_GPIOB
#define UART_RXPIN_EXTI_PIN     EXTI_SOURCE_PIN11
#else
#define LOG_UART_IRQn           USART1_IRQn
#define UART_RXPIN_EXTI_IRQn    EXTI5_9_IRQn
#define UART_RXPIN_EXTI_PORT    EXTI_SOURCE_GPIOA
#define UART_RXPIN_EXTI_PIN     EXTI_SOURCE_PIN8
#endif

#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
#define BufferSize          8
#define AD_SPI_CS_LOW()     gpio_bit_reset(GPIOA, GPIO_PIN_12)
#define AD_SPI_CS_HIGH()    gpio_bit_set(GPIOA, GPIO_PIN_12)

static uint8_t spi0_buf_tx[BufferSize];
static uint8_t spi0_buf_rx[BufferSize];

/*!
    \brief      SPI write data to specify address
    \param[in]  add: the address SPI want to write
    \param[in]  data: the data SPI want to write
    \param[out] none
    \retval     none
*/
void spi_write(uint16_t add, uint32_t data)
{
    uint32_t temp = 0;
    spi0_buf_tx[0] = 0x02;
    spi0_buf_tx[1] = (uint8_t)((add & 0xFF00) >> 8);
    spi0_buf_tx[2] = (uint8_t)(add & 0x00FF);
    spi0_buf_tx[3] = (uint8_t)((data & 0xFF000000) >> 24);
    spi0_buf_tx[4] = (uint8_t)((data & 0x00FF0000) >> 16);
    spi0_buf_tx[5] = (uint8_t)((data & 0x0000FF00) >> 8);
    spi0_buf_tx[6] = (uint8_t)(data & 0x000000FF);

    AD_SPI_CS_LOW();
    for (temp = 0; temp < 7; temp++) {
        while (RESET == spi_i2s_flag_get(RFAD_SPI, SPI_FLAG_TBE));
        spi_i2s_data_transmit(RFAD_SPI, spi0_buf_tx[temp]);
    }

    while (SET == spi_i2s_flag_get(RFAD_SPI, SPI_FLAG_TRANS));
    AD_SPI_CS_HIGH();
}

/*!
    \brief      read SPI data from specify address
    \param[in]  add: the address SPI want to read(0~0xFFFF)
    \param[out] none
    \retval     read data value: 0~0xFFFFFFFF
*/
uint32_t spi_read(uint16_t add)
{
    uint32_t temp = 0;
    spi0_buf_tx[0] = 0x03;
    spi0_buf_tx[1] = (uint8_t)((add & 0xFF00) >> 8);
    spi0_buf_tx[2] = (uint8_t)(add & 0x00FF);
    spi0_buf_tx[3] = 0;  // DUMMY
    spi0_buf_tx[4] = 0;
    spi0_buf_tx[5] = 0;
    spi0_buf_tx[6] = 0;
    spi0_buf_tx[7] = 0;

    while (SET == spi_i2s_flag_get(RFAD_SPI, SPI_FLAG_RBNE)) {
        spi0_buf_rx[0] = spi_i2s_data_receive(RFAD_SPI);  // Clear RXNE
    }

    AD_SPI_CS_LOW();
    for (temp = 0; temp < 8; temp++) {
        while (RESET == spi_i2s_flag_get(RFAD_SPI, SPI_FLAG_TBE));
        spi_i2s_data_transmit(RFAD_SPI, spi0_buf_tx[temp]);
        while (RESET == spi_i2s_flag_get(RFAD_SPI, SPI_FLAG_RBNE));
        spi0_buf_rx[temp] = spi_i2s_data_receive(RFAD_SPI);
    }
    AD_SPI_CS_HIGH();

    temp = ((uint32_t)spi0_buf_rx[4] << 24)
             | ((uint32_t)spi0_buf_rx[5] << 16)
             | ((uint32_t)spi0_buf_rx[6] << 8)
             |  (uint32_t)spi0_buf_rx[7];
    return temp;
}

/*!
    \brief      configure SPI
    \param[in]  spi_periph: SPIx(x=0)
    \param[out] none
    \retval     none
*/
void spi_config(uint32_t spi_periph)
{
    spi_parameter_struct spi_init_structure;

    if (spi_periph == SPI0) {
        rcu_periph_clock_enable(RCU_SPI0);
        rcu_periph_clock_enable(RCU_GPIOA);

        gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9);
        gpio_af_set(GPIOA, GPIO_AF_6, GPIO_PIN_12);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9);

        gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_12);

        spi_init_structure.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
        spi_init_structure.frame_size = SPI_FRAMESIZE_8BIT;
    } else {
        return;
    }

    spi_init_structure.device_mode = SPI_MASTER;
    spi_init_structure.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_structure.nss = SPI_NSS_SOFT;
    spi_init_structure.endian = SPI_ENDIAN_MSB;
    spi_init_structure.prescale = SPI_PSC_32;
    spi_init(spi_periph, &spi_init_structure);

    spi_enable(spi_periph);
}
#endif

/*!
    \brief      enable hardware encrypt/decrypt engine
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hw_sec_engine_enable(void)
{
#ifdef CONFIG_HW_SECURITY_ENGINE
    rcu_periph_clock_enable(RCU_PKCAU);
    rcu_periph_clock_enable(RCU_CAU);
    rcu_periph_clock_enable(RCU_HAU);
#endif
}

extern os_mutex_t hw_pka_lock;
extern os_mutex_t hw_cryp_lock;
extern os_mutex_t hw_hash_lock;

/*!
    \brief      initialize hardware crypto lock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void hw_crypt_lock_init(void)
{
    sys_mutex_init(&hw_pka_lock);
    sys_mutex_init(&hw_cryp_lock);
    sys_mutex_init(&hw_hash_lock);
}

/*!
    \brief      hardware encrypt and decrypt using AES in ECB mode
    \param[in]  mode: 0(CAU_DECRYPT)
    \param[in]  key:  key (in bytes)
    \param[in]  key_size:  key size in bits, must be either 128, 192 or 256
    \param[in]  input:  input data (in bytes)
    \param[in]  in_length:  input data length in bytes, must be a multiple of 16 bytes
    \param[out] output: pointer to the returned buffer
    \retval     none
*/
void hw_cryp_aes_ecb(uint8_t mode, uint8_t *key, uint16_t key_size, uint8_t *input, uint32_t in_length, uint8_t *output)
{
#if defined(CONFIG_TZ_ENABLED)
    param_vec in_vec[5];
    uint32_t in_len;
    in_vec[0].base = &mode;
    in_vec[0].len = sizeof(mode);
    in_vec[1].base = key;
    in_vec[1].len = key_size;
    in_vec[2].base = input;
    in_vec[2].len = in_length;
    in_len = 3;
    sys_mutex_get(&hw_cryp_lock);
    cryp_aes_ecb_nsc(in_vec, in_len, output);
    sys_mutex_put(&hw_cryp_lock);
#else  /* CONFIG_TZ_ENABLED */
    sys_mutex_get(&hw_cryp_lock);
    {
        cau_parameter_struct cau_param;

        cau_deinit();
        cau_struct_para_init(&cau_param);
        if (mode == 0) {
            cau_param.alg_dir = CAU_DECRYPT;
        }
        cau_param.key = key;
        cau_param.key_size = key_size;
        cau_param.input = input;
        cau_param.in_length = in_length;
        cau_aes_ecb(&cau_param, output);
    }
    sys_mutex_put(&hw_cryp_lock);
#endif   /* CONFIG_TZ_ENABLED */
}
/*!
    \brief      hardware calculate digest using SHA1 in HMAC mode
    \param[in]  key: pointer to the key used for HMAC
    \param[in]  key_size: length of the key used for HMAC
    \param[in]  input: pointer to the input buffer
    \param[in]  in_length: length of the input buffer
    \param[out] output: the result digest
    \retval     none
*/
void hw_hmac_sha1(uint8_t *key, uint32_t key_size, uint8_t *input, uint32_t in_length, uint8_t output[20])
{
#if defined(CONFIG_TZ_ENABLED)
    param_vec in_vec[5];
    uint32_t in_len;
    in_vec[0].base = key;
    in_vec[0].len = key_size;
    in_vec[1].base = input;
    in_vec[1].len = in_length;
    in_len = 2;
    sys_mutex_get(&hw_hash_lock);
    hmac_sha1_nsc(in_vec, in_len, output);
    sys_mutex_put(&hw_hash_lock);
#else  /* CONFIG_TZ_ENABLED */
    sys_mutex_get(&hw_hash_lock);
    hau_hmac_sha_1(key, key_size, input, in_length, output);
    sys_mutex_put(&hw_hash_lock);
#endif  /* CONFIG_TZ_ENABLED */
}
/*!
    \brief      hardware calculate digest using SHA256 in HMAC mode
    \param[in]  key: pointer to the key used for HMAC
    \param[in]  key_size: length of the key used for HMAC
    \param[in]  input: pointer to the input buffer
    \param[in]  in_length: length of the input buffer
    \param[out] output: the result digest
    \retval     none
*/
void hw_hmac_sha256(uint8_t *key, uint32_t key_size, uint8_t *input, uint32_t in_length, uint8_t output[32])
{
#if defined(CONFIG_TZ_ENABLED)
/*
    param_vec in_vec[5];
    uint32_t in_len;
    in_vec[0].base = key;
    in_vec[0].len = key_size;
    in_vec[1].base = input;
    in_vec[1].len = in_length;
    in_len = 2;
    sys_mutex_get(&hw_hash_lock);
    HMAC_SHA256_nsc(in_vec, in_len, output);
    sys_mutex_put(&hw_hash_lock);
*/
#else  /* CONFIG_TZ_ENABLED */
    sys_mutex_get(&hw_hash_lock);
    hau_hmac_sha_256(key, key_size, input, in_length, output);
    sys_mutex_put(&hw_hash_lock);
#endif  /* CONFIG_TZ_ENABLED */
}

/*!
    \brief      set wifi peripheral power state
    \param[in]  power_state: the power state of wifi peripheral
                  \arg PMU_WIFI_SLEEP
                  \arg PMU_WIFI_WAKE
    \param[out] none
    \retval     none
*/
void wifi_power_state_set(uint32_t power_state)
{
    pmu_wifi_sram_control(power_state);

    if (power_state == PMU_WIFI_WAKE) {
        /* polling PMU to confirm wifi is in active state */
        while ((PMU_CS1 & PMU_CS1_WPS_ACTIVE) == 0) {
        }
    }
}

/*!
    \brief      wait wifi peripheral is waked
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_waked_wait(void)
{
    /* polling RF PLL ready to confirm wifi clock is valid */
    while ((RCU_CTL & RCU_CTL_PLLDIGSTB) == 0) {
    }
}

/*!
    \brief      set wifi peripheral to lpds mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_lpds_set(void)
{
    /* set wlan_lpds_on command to PMU to switch wifi to LPDS mode */
    PMU_CTL1 |= BIT(17);
}

/*!
    \brief      set wifi peripheral to exit lpds mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_lpds_exit(void)
{
    if (PMU_CS1 & PMU_CS1_WPS_SLEEP) {
        /* clear wlan_lpds_on to exit wifi LPDS mode */
        PMU_CTL1 &= (~BIT(17));
        /* polling PMU to confirm wifi is in active state */
        while ((PMU_CS1 & PMU_CS1_WPS_ACTIVE) == 0) {
        }
    } else {
        /* clear wlan_lpds_on to exit wifi LPDS mode */
        PMU_CTL1 &= (~BIT(17));
        /* wait 100us */
        sys_us_delay(100);
        /* polling PMU to confirm wifi is in active state */
        while ((PMU_CS1 & PMU_CS1_WPS_ACTIVE) == 0) {
        }
    }
}

/*!
    \brief      clear wifi peripheral lpds on bit
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_lpds_on_clear(void)
{
    /* clear wlan_lpds_on to exit wifi LPDS mode */
    PMU_CTL1 &= (~BIT(17));
}

/*!
    \brief      set wifi peripheral gated clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_clock_gate(void)
{
    /* rfip apb disable */
    //REG32(RCU + 0x44) &= ~BIT(31);
    /* wifi enable, wifi run enable */
    //REG32(RCU + 0x30) &= 0xFFFF9FFF;
    /* PLLDIG disable */
    //REG32(RCU + 0) &= ~BIT(21);

    rcu_periph_clock_disable(RCU_RF);
    rcu_periph_clock_disable(RCU_WIFI);
    rcu_periph_clock_disable(RCU_WIFIRUN);
    rcu_plldig_enable();
}

/*!
    \brief      enable wifi peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_clock_enable(void)
{
    /* enable wifi clock */
    //REG32(RCU + 0x30) |= 0x00004000;
    rcu_periph_clock_enable(RCU_WIFIRUN);
}

/*!
    \brief      trigger RF PLL calculation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rf_pll_cal_trigger(void)
{
    RCU_CFG1 = RCU_CFG1 | (RCU_CFG1_RFPLLCALEN);
    while (!(RCU_CFG1 & RCU_CFG1_RFPLLLOCK));
    RCU_CFG1 = RCU_CFG1 & (~RCU_CFG1_RFPLLCALEN);
}

/*!
    \brief      configure nvic
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    *NVIC_CCR = *NVIC_CCR | 0x200; // STKALIGN
    nvic_irq_enable(LOG_UART_IRQn, 7, 0);
    nvic_irq_enable(WLAN_Rx_IRQn, 8, 0);
    nvic_irq_enable(WLAN_Tx_IRQn, 8, 0);
    nvic_irq_enable(WLAN_Cmn_IRQn, 8, 0);
#if CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    nvic_irq_enable(WLAN_WKUP_IRQn, 6, 0);
    nvic_irq_enable(RTC_WKUP_IRQn, 6, 0);
    nvic_irq_enable(UART_RXPIN_EXTI_IRQn, 6, 0);
#endif
}

void sys_reset_flag_check(void)
{
    uint32_t flag = RCU_RSTSCK;

    DEBUGPRINT("\r\n\rSystem reset mode: ");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_EPRSTF) ? "pin, " : "");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_PORRSTF) ? "power on, " : "");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_SWRSTF) ? "soft, " : "");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_FWDGTRSTF) ? "free watchdog, " : "");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_WWDGTRSTF) ? "window watchdog, " : "");
    DEBUGPRINT("%s", (flag & RCU_RSTSCK_LPRSTF) ? "low power, " : "");
    DEBUGPRINT("\r\n");

    RCU_RSTSCK |= RCU_RSTSCK_RSTFC;
}

#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
/*!
    \brief      configure wifi pmu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_pmu_config(void)
{
    /* PMU enable WLAN power save */
    //REG32(PMU + 0x08) |= BIT(1);
    pmu_wifi_power_enable();
    /* wifi is in sleep state by default, set wifi_power_state_set(PMU_WIFI_WAKE) command to PMU to switch wifi power state */
    //REG32(PMU + 0x08) |= BIT(3);
    pmu_wifi_sram_control(PMU_WIFI_WAKE);
    /* polling PMU to confirm wifi is switched from sleep state */
    while ((PMU_CS1 & PMU_CS1_WPS_ACTIVE) == 0) {
    }
}

/*!
    \brief      configure wifi rcu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_rcu_config(void)
{
    /* wifi enable, wifi run enable, RF PU, RF reset  */
    REG32(RCU + 0x30) |= 0xc0006000;

    /* pwr */
    REG32(0x40007080) = 0x00040000;

    REG32(RCU + 0xa0) = 0x0;  // adjust bus phase
    REG32(RCU + 0xa0) = 0xc0005450;
}

/*!
    \brief      configure rtc 32k clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rtc_32k_config(void)
{
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    rtc_pre_config();
}

/*!
    \brief      initialize gdm3210x fpga
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gdm3210x_fpga_init(void)
{
    nvic_config();

    usart_config(LOG_UART);
    DEBUGPRINT("SDK first message for FPGA GDM3210X\r\n");

    wifi_pmu_config();
    wifi_rcu_config();

    spi_config(SPI0);

    wifi_memmap_config();

    rom_init();
}

#elif CONFIG_PLATFORM == PLATFORM_ASIC_32W51X

/*!
    \brief      configure wifi rf crystal
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void wifi_rf_crystal_config(void)
{
#if PLATFORM_CRYSTAL == CRYSTAL_26M
    /* rfip apb enable */
    //REG32(RCU + 0x44) |= 0x80000000;
    rcu_periph_clock_enable(RCU_RF);
    /* rfip mdll_div_num, 0x7c[18:15], 26M:1000, 40M:0101(default) */
    REG32(RF_BASE + 0x7C) = 0x70044400;
    /* rfip plldig */
    REG32(RF_BASE + 0x64) = 0x49D89D8;
#endif
}

/*!
    \brief      configure wifi pmu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_pmu_config(void)
{
    /* PMU enable WLAN power */
    //REG32(PMU + 0x08) |= BIT(1);
    pmu_wifi_power_enable();
    /* wifi is in sleep state by default, set wifi_power_state_set(PMU_WIFI_WAKE) command to PMU to switch wifi power state */
    //REG32(PMU + 0x08) |= BIT(3);
    pmu_wifi_sram_control(PMU_WIFI_WAKE);
    /* polling PMU to confirm wifi is switched from sleep state */
    while ((PMU_CS1 & PMU_CS1_WPS_ACTIVE) == 0) {
    }

    /* set HXTAL settle time to 100us */
    REG32(PMU + 0x24) &= 0xFFFFFF00;
}

/*!
    \brief      configure wifi rcu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_rcu_config(void)

{
    /* polling RF DPLL ready */
    while ((RCU_CTL & RCU_CTL_PLLDIGSTB) == 0) {
    }

    /* PLLDIG enable */
    //REG32(RCU + 0) |= BIT(21);
    rcu_plldig_enable();

    /* wifi enable, wifi run enable */
    //REG32(RCU + 0x30) |= 0x00006000;
    rcu_periph_clock_enable(RCU_WIFI);
    rcu_periph_clock_enable(RCU_WIFIRUN);

    /* rfip apb enable */
    //REG32(RCU + 0x44) |= 0x80000000;
    rcu_periph_clock_enable(RCU_RF);

    rcu_periph_reset_enable(RCU_WIFIRST);
    rcu_periph_reset_disable(RCU_WIFIRST);
}

/*!
    \brief      setup rtc
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rtc_setup(void)
{
    rtc_parameter_struct rtc_initpara;

    /* setup RTC time value */
    uint32_t tmp_hh = 0, tmp_mm = 0, tmp_ss = 0;

    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x20;
    rtc_initpara.day_of_week = RTC_MONDAY;
    rtc_initpara.month = RTC_NOV;
    rtc_initpara.date = 0x2;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;
    rtc_initpara.hour = tmp_hh;
    rtc_initpara.minute = tmp_mm;
    rtc_initpara.second = tmp_ss;

    /* RTC current time configuration */
    if (ERROR == rtc_init(&rtc_initpara)) {
        DEBUGPRINT("RTC time configuration failed!\r\n");
    } else {
           rtc_current_time_get(&rtc_initpara);
    }
}

/*!
    \brief      configure rtc 32k clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rtc_32k_config(void)
{
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    rtc_pre_config();
    /* 32K clock selection */
    REG32(RCU + 0x70) = (REG32(RCU + 0x70) & 0xfffffcff) | 0x00000200;

    rtc_deinit();
    rtc_setup();

    /* clear wakeup timer occurs flag */
    rtc_flag_clear(RTC_STAT_WTF);
    /* RTC wakeup configuration */
    rtc_interrupt_enable(RTC_INT_WAKEUP);
    /* set wakeup clock as RTCCK_DIV16 */
    rtc_wakeup_clock_set(WAKEUP_RTCCK_DIV16);
}

/*!
    \brief      configure usart rx pin exti
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void usart_rxpin_exti_config()
{
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* connect EXTI line to usart rx pin */
    syscfg_exti_line_config(UART_RXPIN_EXTI_PORT, UART_RXPIN_EXTI_PIN);
}

/*!
    \brief      initialize gd32w51x asic
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gd32w51x_asic_init(void)
{
    nvic_config();

    usart_config(LOG_UART);
    DEBUGPRINT("SDK first message for GDM32W51x\r\n");

    wifi_rf_crystal_config();
    wifi_pmu_config();
    wifi_rcu_config();
    rtc_32k_config();
    usart_rxpin_exti_config();

    hw_sec_engine_enable();
    dma1_config();

    wifi_setting_config();

    rom_init();
}
#endif

/*!
    \brief      reset wifi rcu
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_rcu_deinit(void)
{
    /* clock: wifi disable, wifi run disable */
    wifi_clock_gate();
    /* set PMU to power off wifi */
    if ((PMU_CS1 & PMU_CS1_WPS_ACTIVE))
        wifi_power_state_set(PMU_WIFI_SLEEP);
}

/*!
    \brief      initialize platform
    \param[in]  none
    \param[out] none
    \retval     none
*/
void platform_init(void)
{
#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
    gdm3210x_fpga_init();
#elif CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    gd32w51x_asic_init();
#endif
}
