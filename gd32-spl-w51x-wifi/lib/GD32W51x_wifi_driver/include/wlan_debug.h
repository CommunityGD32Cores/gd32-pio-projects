/*!
    \file    wlan_debug.h
    \brief   Debug configuration for GD32W51x WiFi driver

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

#ifndef __WLAN_DEBUG_H__
#define __WLAN_DEBUG_H__

#define WLAN_MODULE_NAME "WLAN_DRV"

#define _drv_err_            1
#define _drv_warning_        2
#define _drv_notice_         3
#define _drv_stat_           4
#define _drv_info_           5
#define _drv_debug_          6
#define _drv_dump_           7

#define WLAN_DBG_XMIT        BIT(0)
#define WLAN_DBG_RECV        BIT(1)
#define WLAN_DBG_MLME        BIT(2)
#define WLAN_DBG_WPAS        BIT(3)
#define WLAN_DBG_WORK        BIT(4)
#define WLAN_DBG_IO          BIT(5)
#define WLAN_DBG_SEC         BIT(6)
#define WLAN_DBG_AP          BIT(7)
#define WLAN_DBG_INIT        BIT(8)
#define WLAN_DBG_IOCTL       BIT(9)
#define WLAN_DBG_PWR         BIT(10)
#define WLAN_DBG_INT         BIT(11)
#define WLAN_DBG_OSAL        BIT(12)
#define WLAN_DBG_SOC         BIT(13)
#define WLAN_DBG_HAL         BIT(14)
#define WLAN_DBG_RA          BIT(15)
#define WLAN_DBG_CALIB       BIT(16)

#define dbgdump(...)
#define DBG_PRINT(_Comp, _Level, Fmt) do{}while(0)
#define WLAN_ASSERT(_Exp,Fmt) do{}while(0)

#define _func_line_ {}

extern uint32_t GlobalDebugLevel;
extern uint32_t GlobalDebugComponents;
extern uint32_t GlobalDumpOption;

#ifdef CONFIG_DEBUG_SUPPORT

#undef dbgdump
#define dbgdump(...)     do {\
    co_printf(__VA_ARGS__);  \
}while(0)

#undef DBG_PRINT
#define DBG_PRINT(_Comp, _Level, Fmt)                                     \
do {                                                                      \
    if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) { \
        dbgdump Fmt;                                                      \
    }                                                                     \
}while(0)

#undef WLAN_ASSERT
#define WLAN_ASSERT(_Exp,Fmt)             \
do {                                      \
    if(!(_Exp))                           \
    {                                     \
        dbgdump("%s: ", WLAN_MODULE_NAME);\
        dbgdump Fmt;                      \
    }                                     \
} while (0)

#endif

#define DUMP_RX_PKT_EN         (1)
#define DUMP_TX_PKT_EN         (1 << 1)
#define DUMP_SEC_KEY_EN        (1 << 2)
#define DUMP_PSK_EAPOL_EN      (1 << 3)
#define DUMP_SOC_TX_DESC_EN    (1 << 4)
#define DUMP_SOC_RX_DESC_EN    (1 << 5)
#define DUMP_RX_PHY_STATUS_EN  (1 << 6)
#define DUMP_PS_LOG_EN         (1 << 7)
#define DUMP_RX_CSI_REPORT_EN  (1 << 8)
#define DUMP_WPA3_SAE_EN       (1 << 9)

#define DUMP_RX_PKT            (GlobalDumpOption & DUMP_RX_PKT_EN)
#define DUMP_TX_PKT            (GlobalDumpOption & DUMP_TX_PKT_EN)
#define DUMP_SEC_KEY           (GlobalDumpOption & DUMP_SEC_KEY_EN)
#define DUMP_PSK_EAPOL         (GlobalDumpOption & DUMP_PSK_EAPOL_EN)
#define DUMP_SOC_TX_DESC       (GlobalDumpOption & DUMP_SOC_TX_DESC_EN)
#define DUMP_SOC_RX_DESC       (GlobalDumpOption & DUMP_SOC_RX_DESC_EN)
#define DUMP_RX_PHY_STATUS     (GlobalDumpOption & DUMP_RX_PHY_STATUS_EN)
#define DUMP_PS_LOG            (GlobalDumpOption & DUMP_PS_LOG_EN)
#define DUMP_RX_CSI_REPORT     (GlobalDumpOption & DUMP_RX_CSI_REPORT_EN)
#define DUMP_WPA3_SAE          (GlobalDumpOption & DUMP_WPA3_SAE_EN)

#define RATELIMIT            10
extern uint32_t limit_in_period;

static __inline int print_ratelimit() {
    if (++limit_in_period > RATELIMIT) {
        return 1;
    }
    return 0;
}

#endif
