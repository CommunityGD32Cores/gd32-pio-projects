/*!
    \file    wlan_cfg.h
    \brief   Configuration for GD32W51x WiFi driver

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

#ifndef _WLAN_CFG_H_
#define _WLAN_CFG_H_

#include "platform_def.h"

#define CONFIG_DEBUG_SUPPORT

#define RF_GDM32106             0
#define RF_GDM32101             1

#define RF_CRYSTAL_26M          0
#define RF_CRYSTAL_40M          1

#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
    #define CONFIG_RF_TYPE      RF_GDM32106
#elif CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    #define CONFIG_RF_TYPE      RF_GDM32101
#else
    #error unknown RF type for CONFIG_PLATFORM
#endif

#if CONFIG_PLATFORM == PLATFORM_FPGA_3210X
    #define PHY_VERSION         2
#elif CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    #define PHY_VERSION         3
#else
    #error not catched case for CONFIG_PLATFORM
#endif

#define CONFIG_LITTLE_ENDIAN

// #define CONFIG_80211AC_SUPPORT

#define INTERNAL_WPA_WPA2_PSK

#define CONFIG_PMF_SUPPORT
#define CONFIG_WPA3_SAE
#if defined(CONFIG_WPA3_SAE) && !defined(CONFIG_PMF_SUPPORT)
    #error CONFIG_PMF_SUPPORT should be enabled to support CONFIG_WPA3_SAE
#endif

#define WLAN_DYN_CHK_TIMEOUT    2000

#define CONFIG_RATE_ADAPTIVE

// #define CONFIG_LAYER2_ROAMING

// #define SW_ENCRYPT_SUPPORT
// #define SW_DECRYPT_SUPPORT

// #define CONFIG_IDLE_PS_SUPPORT

#define CONFIG_LPS_SUPPORT

// #define CONFIG_RX_QUEUE

#ifdef CONFIG_HW_SECURITY_ENGINE
#define CONFIG_HW_HMAC_SHA
#define CONFIG_HW_AES_CRYP
#endif

// #define CONFIG_RX_REORDER_SUPPORT
#define CONFIG_PHY_STATUS_ENABLE
// #define CONFIG_5G_BAND_ENABLE

#define CONFIG_DM_SUPPORT

#define WIFI_EFUSE

// #define CONFIG_CSI_REPORT_ENABLE

// #define CONFIG_HEAP_USAGE_TRACKING

// #define CONFIG_SDIO_HCI
#define CONFIG_SOC_HCI

#define CONFIG_SOFTAP_SUPPORT
#ifdef CONFIG_SOFTAP_SUPPORT
#define SOFTAP_STA_PS
// #define SOFTAP_GROUP_REKEY  // if defined, configTIMER_TASK_STACK_DEPTH should be 512 dword
#define MAX_STATION_NUM         3
#else /* CONFIG_SOFTAP_SUPPORT */
#define MAX_STATION_NUM         1
#endif /* CONFIG_SOFTAP_SUPPORT */

/* please define CONFIG_WIFI_HIGH_PERFORMANCE in app_cfg.h too */
// #define CONFIG_WIFI_HIGH_PERFORMANCE
#ifdef CONFIG_WIFI_HIGH_PERFORMANCE
  #ifndef CONFIG_RX_REORDER_SUPPORT
    #define CONFIG_RX_REORDER_SUPPORT
  #endif
#endif /* CONFIG_WIFI_HIGH_PERFORMANCE */

// #define WIFI_LOGO
#ifdef WIFI_LOGO
  #ifndef CONFIG_RX_REORDER_SUPPORT
    #define CONFIG_RX_REORDER_SUPPORT
  #endif
  #ifndef CONFIG_RX_DEFRAG_SUPPORT
    #define CONFIG_RX_DEFRAG_SUPPORT
  #endif
    #define CONFIG_20_40_BSS_COEX
#endif /* WIFI_LOGO */

#endif  /* _WLAN_CFG_H_ */
