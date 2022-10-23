/*!
    \file    wlan_intf_def.h
    \brief   WLAN interface definition for GD32W51x WiFi driver

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

#ifndef __WLAN_INTF_DEF_H
#define __WLAN_INTF_DEF_H

/*============================ INCLUDES ======================================*/
#include "osal_types.h"

/*============================ MACROS ========================================*/
#define WIFI_BSSID_LEN              6
#define WIFI_SSID_MAX_LEN           32
#define WEP_IV_LEN                  3
#define WEP_64BITS_KEY_LEN          8
#define WEP_128BITS_KEY_LEN         16
#define WEP_KEY_ID_MAX              4
#define WEP_64BITS_KEY_HEX_LEN      5
#define WEP_128BITS_KEY_HEX_LEN     13
// #define WPA_PSK_LEN              63
#define WPA_MAX_PSK_LEN             63
#define WPA_MIN_PSK_LEN             8

#define TEST_TX_MODE_BC_UC_NOENC    0x1
#define TEST_TX_MODE_UC_ENC         0x2
#define TEST_TX_MODE_RAW_FRAME      0x3
#define TEST_TX_MODE_CONTINUOUS_TX  0x4
#define TEST_TX_MODE_DAC_TX         0x5
#define TEST_TX_MODE_TYPE_MASK      0x7F
#define TEST_TX_MODE_DESC_CACHED    0x80

#define TX_FRAME_TYPE_SKB_BUFFER    0
#define TX_FRAME_TYPE_XMIT_BUFFER   1
#define TX_FRAME_TYPE_FW_BUFFER     2
#define TX_FRAME_TYPE_SINGLE_BUFFER 3   /* no need to release for single buffer */

#define SUPPORT_MAX_AP_NUM          48  // 32

// MP_MODE
#define MP_DISABLED                 0x00
#define MP_TEST_MODE                0x01
#define MP_RF_NORMAL_TEST_MODE      0x02
#define MP_RF_TEMP_TEST_MODE        0x03

/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/
/**
 * @brief WiFi network mode
 */
typedef enum {
    WIFI_NETWORK_MODE_INFRA,
    WIFI_NETWORK_MODE_ADHOC,
    WIFI_NETWORK_MODE_MAX
} WIFI_NETWORK_MODE_E;

/**
 * @brief WiFi scan result type
 */
typedef enum {
    WIFI_SCAN_RESULT_SUCC,
    WIFI_SCAN_RESULT_FAIL,
} WIFI_SCAN_RESULT_E;

/**
 * @brief WiFi connect result type
 */
typedef enum {
    WIFI_CONN_RESULT_SUCC,
    WIFI_CONN_RESULT_AP_NOT_FOUND,
    WIFI_CONN_RESULT_AUTH_FAIL,
    WIFI_CONN_RESULT_ASSOC_FAIL,
    WIFI_CONN_RESULT_HANDSHAKE_FAIL,
    WIFI_CONN_RESULT_UNSPECIFIED,
} WIFI_CONN_RESULT_E;

/**
 * @brief WiFi disconnect reason
 */
typedef enum {
    WIFI_DISCON_REKEY_FAIL,
    WIFI_DISCON_MIC_FAIL,
    WIFI_DISCON_RECV_DEAUTH,
    WIFI_DISCON_RECV_DISASSOC,
    WIFI_DISCON_NO_BEACON,
    WIFI_DISCON_AP_CHANGED,
    WIFI_DISCON_FROM_UI,
    WIFI_DISCON_SA_QUERY_FAIL,
    WIFI_DISCON_UNSPECIFIED
} WIFI_DISCON_REASON_E;

/**
 * @brief WiFi encryption protocol type
 */
typedef enum {
    WIFI_ENCRYPT_PROTOCOL_OPENSYS,
    WIFI_ENCRYPT_PROTOCOL_WEP,
    WIFI_ENCRYPT_PROTOCOL_WPA,
    WIFI_ENCRYPT_PROTOCOL_WPA2,
    WIFI_ENCRYPT_PROTOCOL_WPA3_TRANSITION,
    WIFI_ENCRYPT_PROTOCOL_WPA3_ONLY,
    WIFI_ENCRYPT_PROTOCOL_WAPI,
    WIFI_ENCRYPT_PROTOCOL_EAP,
    WIFI_ENCRYPT_PROTOCOL_MAX
} WIFI_ENCRYPT_PROTOCOL_E;

/**
 * @brief WiFi WEP key type
 */
typedef enum {
    WIFI_WEP_KEY_TYPE_64BITS,
    WIFI_WEP_KEY_TYPE_128BITS,
    WIFI_WEP_KEY_TYPE_MAX
} WIFI_WEP_KEY_TYPE_E;

/**
 * @brief WiFi WPA, WPA2 or WPA3 credential type
 */
typedef enum {
    WIFI_WPA_CREDENTIAL_TYPE_PSK,
    WIFI_WPA_CREDENTIAL_TYPE_SAE,
    WIFI_WPA_CREDENTIAL_TYPE_EAP,
    WIFI_WPA_CREDENTIAL_TYPE_MAX
} WIFI_WPA_CREDENTIAL_TYPE_E;

/**
 * @brief WiFi credential type
 */
typedef enum {
    WIFI_CREDENTIAL_TYPE_PSK,
    WIFI_CREDENTIAL_TYPE_RADIUS,
    WIFI_CREDENTIAL_TYPE_CER,
    WIFI_CREDENTIAL_TYPE_MAX
} WIFI_CREDENTIAL_TYPE_E;

/**
 * @brief WiFi encryption cipher type
 */
typedef enum {
    WIFI_CIPHER_TKIP,
    WIFI_CIPHER_CCMP,
    WIFI_CIPHER_WEP,
    WIFI_CIPHER_SMS4,
    WIFI_CIPHER_MAX
} WIFI_CIPHER_E;

/**
 * @brief WiFi PHY DAC tx test type
 */
typedef enum {
    DAC_TX_SINGLE_TONE,
    DAC_TX_SQUARE_WAVE,
    DAC_TX_DC_MODE,
    DAC_TX_FROM_SRAM,
    DAC_TX_MAX
} PHY_DAC_TX_TYPE_E;

/**
 * @brief WiFi continuous tx test type
 */
typedef enum {
    CONTINUOUS_TX_STF,
    CONTINUOUS_TX_CCK_DATA,
    CONTINUOUS_TX_OFDM_DATA,
    CONTINUOUS_TX_HT_DATA,
    CONTINUOUS_TX_MAX
} CONTINUOUS_TX_TYPE_E;

/**
 * @brief WiFi dump ADC trigger type
 */
typedef enum {
    DUMP_ADC_TRIG_RX_SIG = 0,
    DUMP_ADC_TRIG_RX_HTSIG,
    DUMP_ADC_TRIG_RX_DECODE_ERR,
    DUMP_ADC_TRIG_PATTERN_MATCH,
    DUMP_ADC_MAC_RX_BUS,
    DUMP_ADC_TRIG_TXEN_RISING,
    DUMP_ADC_TRIG_TX_HTSIG,
    DUMP_ADC_TRIG_TX_UDF,
    DUMP_ADC_TRIG_MAX
} DUMP_ADC_TRIG_TYPE_E;

/**
 * @brief WiFi PHY RX PSD Type
 */
typedef enum {
    RX_PSD_TRIGGER,
    RX_PSD_IMMEDIATE,
    RX_PSD_MAX
} PHY_RX_PSD_TYPE_E;

/**
 * @brief WiFi private request type
 */
typedef enum {
    WIFI_PRIV_READ_MAC_REG,
    WIFI_PRIV_WRITE_MAC_REG,
    WIFI_PRIV_DUMP_MAC,
    WIFI_PRIV_READ_PHY_REG,
    WIFI_PRIV_WRITE_PHY_REG,
    WIFI_PRIV_DUMP_PHY,
    WIFI_PRIV_READ_RF_REG,
    WIFI_PRIV_WRITE_RF_REG,
    WIFI_PRIV_DUMP_RF,
    WIFI_PRIV_READ_AADC,
    WIFI_PRIV_SET_TX_RATE,
    WIFI_PRIV_SET_TX_POWER,
    WIFI_PRIV_GET_TX_POWER,
    WIFI_PRIV_FORCE_TX_POWER,
    WIFI_PRIV_SET_RTS_CTS,
    WIFI_PRIV_DAC_TX_TEST,
    WIFI_PRIV_ENABLE_PS,
    WIFI_PRIV_DISABLE_PS,
    WIFI_PRIV_TEST_TX,
    WIFI_PRIV_CONTINUOUS_TX,
    WIFI_PRIV_TX_STOP,
    WIFI_PRIV_TEST_RX,
    WIFI_PRIV_CONF_TRIG,
    WIFI_PRIV_DUMP_ADC,
    WIFI_PRIV_AD_SPI_READ,
    WIFI_PRIV_AD_SPI_WRITE,
    WIFI_PRIV_SET_LPBK_MODE,
    WIFI_PRIV_SET_RA_PARAM,
    WIFI_PRIV_MP_MODE,
    WIFI_PRIV_SET_MP_TAGETPWR,
    WIFI_PRIV_SET_MP_PWROFFSET,
    WIFI_PRIV_TX_IQK,
    WIFI_PRIV_TX_DCK,
    WIFI_PRIV_TX_LOK,
    WIFI_PRIV_RX_IQK,
    WIFI_PRIV_RX_DCK,
    WIFI_PRIV_RCK,
    WIFI_PRIV_LNAK,
    WIFI_PRIV_SET_CRYSTAL_CAP,
    WIFI_PRIV_GET_CRYSTAL_CAP,
    WIFI_PRIV_GET_BANDWIDTH,
    WIFI_PRIV_HAL_WDOG_EN,
    WIFI_PRIV_RESET_TRX_COUNTERS,
    WIFI_PRIV_GET_TX_COUNTERS,
    WIFI_PRIV_GET_RX_COUNTERS,
    WIFI_PRIV_RX_EVM,
    WIFI_PRIV_RX_PSD,
    WIFI_PRIV_RX_FILTER,
    WIFI_PRIV_DM,
    WIFI_PRIV_DM_OPTION,
    WIFI_POWER_BY_RATE,
    WIFI_COUNTRY_CODE,
    WIFI_PRIV_WRITE_EFUSE_CHECK,
    WIFI_PRIV_GET_THERMAL,
    WIFI_PRIV_SET_RF_FREQ,
    WIFI_PRIV_SET_PLLDIG,
    WIFI_PRIV_MAX
} WIFI_PRIV_REQ_E;

/**
 * @brief WiFi ssid struct
 */
struct wifi_ssid {
    uint8_t ssid[WIFI_SSID_MAX_LEN+1];
    uint8_t ssid_len;
};

/**
 * @brief WiFi bssid struct
 */
struct wifi_bssid {
    uint8_t bssid[WIFI_BSSID_LEN];
};

/**
 * @brief WiFi scan information used in getting scan result
 */
struct wifi_scan_info {
    struct wifi_scan_info *next_ptr;
    struct wifi_ssid ssid;
    struct wifi_bssid bssid_info;
    int32_t rssi;
    uint16_t channel;
    uint16_t rate;
    WIFI_ENCRYPT_PROTOCOL_E encryp_protocol;
    WIFI_NETWORK_MODE_E network_mode;
    WIFI_CIPHER_E pairwise_cipher;
    WIFI_CIPHER_E group_cipher;
    WIFI_CREDENTIAL_TYPE_E credential_type;
};

/**
 * @brief WiFi WPA or WPA2 PSK struct
 */
struct wifi_wpa_psk {
    uint8_t psk[WPA_MAX_PSK_LEN];
};

/**
 * @brief WiFi disconnect request
 */
struct wifi_disconnect_req {
    struct wifi_ssid ssid;
};

/**
 * @brief WiFi WPA or WPA2 credential struct
 */
struct wifi_wpa_credential {
    WIFI_WPA_CREDENTIAL_TYPE_E  credential_type;
    struct wifi_wpa_psk  wpa_psk_info;
};

/**
 * @brief WiFi WEP credential struct
 */
struct wifi_wep_credential {
    WIFI_WEP_KEY_TYPE_E key_type;
    uint8_t is_open_mode;
    uint32_t key_in_use;
    union {
        uint8_t key_64bits_arr[WEP_KEY_ID_MAX][WEP_64BITS_KEY_LEN - WEP_IV_LEN];
        uint8_t key_128bits_arr[WEP_KEY_ID_MAX][WEP_128BITS_KEY_LEN - WEP_IV_LEN];
    } key;
};

/**
 * @brief WiFi ssid config information used when connecting to AP
 */
struct wifi_ssid_config {
    struct wifi_ssid ssid;
    struct wifi_bssid bssid_info;
    WIFI_NETWORK_MODE_E network_mode;
    WIFI_ENCRYPT_PROTOCOL_E encryp_protocol;
    uint32_t channel;
    int32_t rssi;
    union {
        struct wifi_wep_credential wep_credential;
        struct wifi_wpa_credential wpa_credential;
    } credential;
    WIFI_CIPHER_E pairwise_cipher;
    WIFI_CIPHER_E group_cipher;
    uint8_t passwd[WPA_MAX_PSK_LEN + 1];
};

/**
 * @brief WiFi AP config information used when starting softAP
 */
struct wifi_ap_config {
    uint8_t ssid[WIFI_SSID_MAX_LEN + 1];
    uint8_t password[WPA_MAX_PSK_LEN + 1];
    WIFI_ENCRYPT_PROTOCOL_E security;
    uint8_t channel;
    uint8_t hidden;
};

/**
 * @brief WiFi connect result struct
 */
struct wifi_connect_result {
    WIFI_CONN_RESULT_E result;
    struct wifi_ssid ssid;
    struct wifi_bssid bssid_info;
    uint32_t channel;
    int32_t rssi;
    uint8_t bw;
    uint8_t wireless_mode;
};

/**
 * @brief WiFi disconnect info
 */
struct wifi_disconnect_info {
    WIFI_DISCON_REASON_E reason;
    struct wifi_ssid ssid;
};

/**
 * @brief WiFi scan request struct
 */
struct wifi_scan_req {
    struct wifi_ssid ssid;
};

/**
 * @brief WiFi private request information
 */
struct wifi_priv_req_info {
    WIFI_PRIV_REQ_E priv_req;
    uint32_t priv_param1;
    uint32_t priv_param2;
    uint32_t priv_param3;
    uint32_t priv_param4;
    uint32_t *result_data;
};

/**
 * @brief WiFi used heap information
 */
struct wifi_used_heap_info {
    int32_t used_heap_size;
    int32_t max_used_heap_size;
};

/**
 * @brief WiFi extra parameter
 */
typedef struct wifi_param_t {
    uint8_t crystal_freq;
    /* waiting for further expansion */
}wifi_param_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
void wifi_set_param(wifi_param_t* wifi_param_set);
/*============================ IMPLEMENTATION ================================*/
#endif
