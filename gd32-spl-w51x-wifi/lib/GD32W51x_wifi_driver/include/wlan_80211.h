/*!
    \file    wlan_80211.h
    \brief   IEEE802.11 definition for GD32W51x WiFi driver

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

#ifndef _WLAN_80211_H_
#define _WLAN_80211_H_

#include "wlan_cfg.h"

#define ETH_HLEN                    14
#define LLC_HEADER_SIZE             6

#define ETH_P_IP                    0x0800
#define ETH_P_ARP                   0x0806
#define ETH_P_RARP                  0x8035
#define ETH_P_AARP                  0x80F3
#define ETH_P_IPX                   0x8137
#define ETH_P_PAE                   0x888E

#define WLAN_IEEE_OUI_LEN           3
#define WLAN_HDR_A3_LEN             24
#define WLAN_HDR_A4_LEN             30
#define WLAN_HDR_A3_QOS_LEN         26
#define WLAN_HDR_A4_QOS_LEN         32
#define WLAN_SSID_MAXLEN            32
#define WLAN_DATA_MAXLEN            2312
#define IW_ESSID_MAX_SIZE           32
#define WEP_KEYS                    4
#define MAX_WPA_IE_LEN              128
#define MAX_TID_NUM                 16

#define WPA_CIPHER_NONE             BIT(0)
#define WPA_CIPHER_WEP40            BIT(1)
#define WPA_CIPHER_WEP104           BIT(2)
#define WPA_CIPHER_TKIP             BIT(3)
#define WPA_CIPHER_CCMP             BIT(4)
#define WPA_SELECTOR_LEN            4
#define RSN_SELECTOR_LEN            4

#define RSN_AKM_NONE                BIT(0)
#define RSN_AKM_8021X               BIT(1)
#define RSN_AKM_PSK                 BIT(2)
#define RSN_AKM_8021X_FT            BIT(3)
#define RSN_AKM_PSK_FT              BIT(4)
#define RSN_AKM_8021X_SHA256        BIT(5)
#define RSN_AKM_PSK_SHA256          BIT(6)
#define RSN_AKM_TDLS                BIT(7)
#define RSN_AKM_SAE                 BIT(8)
#define RSN_AKM_SAE_FT              BIT(9)
#define RSN_AKM_RESERVED            BIT(10)

#define WLAN_MIN_ETHFRM_LEN         60
#define WLAN_MAX_ETHFRM_LEN         1514

#define MIN_FRAG_THRESHOLD          256U
#define MAX_FRAG_THRESHOLD          2346U

#define MAX_IE_SZ                   512

#define IEEE80211_LENGTH_RATES      8
#define IEEE80211_LENGTH_RATES_EX   16
#define NUM_SUPP_RATES              (12+1)
#define NUM_24G_CHANNELS            (14)
#define NUM_5G_CHANNELS             (8)

/* QoS ACK type */
#define NORMAL_ACK                  0
#define NO_ACK                      1
#define NON_EXPLICIT_ACK            2
#define BLOCK_ACK                   3

/* Authentication algorithms */
#define WLAN_AUTH_OPEN              0
#define WLAN_AUTH_SHARED_KEY        1
#define WLAN_AUTH_CHALLENGE_LEN     128

/* WLAN capability */
#define WLAN_CAPABILITY_BSS             (1<<0)
#define WLAN_CAPABILITY_IBSS            (1<<1)
#define WLAN_CAPABILITY_CF_POLLABLE     (1<<2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST (1<<3)
#define WLAN_CAPABILITY_PRIVACY         (1<<4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE  (1<<5)
#define WLAN_CAPABILITY_PBCC            (1<<6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY (1<<7)
#define WLAN_CAPABILITY_SHORT_SLOT      (1<<10)

/* Fixed field */
#define AUTH_ALGM_NUM               2
#define AUTH_SEQ_NUM                2
#define BEACON_ITERVAL              2
#define CAPABILITY_INFO             2
#define CURRENT_APADDR              6
#define LISTEN_INTERVAL             2
#define REASON_CODE                 2
#define ASSOC_ID                    2
#define STATUS_CODE                 2
#define TIMESTAMP                   8

/* Information Element IDs*/
#define EID_SSID                    0
#define EID_SUPP_RATES              1
#define EID_DS_PARAMS               3
#define EID_TIM                     5
#define EID_IBSS_PARAMS             6
#define EID_CHALLENGE               16
#define EID_ERP_INFO                42
#define EID_RSN                     48
#define EID_EXT_SUPP_RATES          50
#define EID_MME                     76
#define EID_GENERIC                 221
#define EID_WPA                     221
#define EID_VENDOR_SPECIFIC         221
#define EID_HT_CAPABILITY           45
#define EID_HT_OPERATION            61
#define EID_20_40_BSS_COEX          72
#define EID_20_40_BSS_INTOL_CHAN    73
#define EID_OBSS_SCAN_PARAMS        74
#define EID_EXT_CAPABILITY          127
#ifdef CONFIG_80211AC_SUPPORT
#define EID_VHT_CAPABILITY          191
#define EID_VHT_OPERATION           192
#define EID_OPMODE_NOTIFICATION     199
#endif
#define WLAN_EID_EXTENSION          255

/* Element ID Extension (EID 255) values */
#define WLAN_EID_EXT_ASSOC_DELAY_INFO           1
#define WLAN_EID_EXT_FILS_REQ_PARAMS            2
#define WLAN_EID_EXT_FILS_KEY_CONFIRM           3
#define WLAN_EID_EXT_FILS_SESSION               4
#define WLAN_EID_EXT_FILS_HLP_CONTAINER         5
#define WLAN_EID_EXT_FILS_IP_ADDR_ASSIGN        6
#define WLAN_EID_EXT_KEY_DELIVERY               7
#define WLAN_EID_EXT_FILS_WRAPPED_DATA          8
#define WLAN_EID_EXT_FTM_SYNC_INFO              9
#define WLAN_EID_EXT_EXTENDED_REQUEST           10
#define WLAN_EID_EXT_ESTIMATED_SERVICE_PARAMS   11
#define WLAN_EID_EXT_FILS_PUBLIC_KEY            12
#define WLAN_EID_EXT_FILS_NONCE                 13
#define WLAN_EID_EXT_FUTURE_CHANNEL_GUIDANCE    14
#define WLAN_EID_EXT_OWE_DH_PARAM               32
#define WLAN_EID_EXT_PASSWORD_IDENTIFIER        33
#define WLAN_EID_EXT_HE_CAPABILITIES            35
#define WLAN_EID_EXT_HE_OPERATION               36
#define WLAN_EID_EXT_HE_MU_EDCA_PARAMS          38
#define WLAN_EID_EXT_SPATIAL_REUSE              39
#define WLAN_EID_EXT_OCV_OCI                    54

#define ASOCREQ_IE_OFFSET                       4
#define ASOCRSP_IE_OFFSET                       6
#define REASOCREQ_IE_OFFSET                     10
#define REASOCRSP_IE_OFFSET                     6
#define PROBEREQ_IE_OFFSET                      0
#define PROBERSP_IE_OFFSET                      12
#define AUTH_IE_OFFSET                          6
#define DEAUTH_IE_OFFSET                        0
#define BEACON_IE_OFFSET                        12

#define FIXED_IE_LENGTH                         BEACON_IE_OFFSET
#define WMM_IE_LENGTH                           7
#define WMM_PARA_IE_LENGTH                      24


#define IEEE80211_CCK_RATE_NUM                  4
#define IEEE80211_OFDM_RATE_NUM                 8

#define IEEE80211_CCK_RATE_1MB                  0x02
#define IEEE80211_CCK_RATE_2MB                  0x04
#define IEEE80211_CCK_RATE_5MB                  0x0B
#define IEEE80211_CCK_RATE_11MB                 0x16
#define IEEE80211_OFDM_RATE_6MB                 0x0C
#define IEEE80211_OFDM_RATE_9MB                 0x12
#define IEEE80211_OFDM_RATE_12MB                0x18
#define IEEE80211_OFDM_RATE_18MB                0x24
#define IEEE80211_OFDM_RATE_24MB                0x30
#define IEEE80211_OFDM_RATE_36MB                0x48
#define IEEE80211_OFDM_RATE_48MB                0x60
#define IEEE80211_OFDM_RATE_54MB                0x6C
#define IEEE80211_BASIC_RATE_MASK               0x80

#define IEEE80211_CCK_RATE_1MB_MASK             (1<<0)
#define IEEE80211_CCK_RATE_2MB_MASK             (1<<1)
#define IEEE80211_CCK_RATE_5MB_MASK             (1<<2)
#define IEEE80211_CCK_RATE_11MB_MASK            (1<<3)
#define IEEE80211_OFDM_RATE_6MB_MASK            (1<<4)
#define IEEE80211_OFDM_RATE_9MB_MASK            (1<<5)
#define IEEE80211_OFDM_RATE_12MB_MASK           (1<<6)
#define IEEE80211_OFDM_RATE_18MB_MASK           (1<<7)
#define IEEE80211_OFDM_RATE_24MB_MASK           (1<<8)
#define IEEE80211_OFDM_RATE_36MB_MASK           (1<<9)
#define IEEE80211_OFDM_RATE_48MB_MASK           (1<<10)
#define IEEE80211_OFDM_RATE_54MB_MASK           (1<<11)

#define IEEE80211_CCK_RATES_MASK                0x0000000F
#define IEEE80211_CCK_BASIC_RATES_MASK          (IEEE80211_CCK_RATE_1MB_MASK |  IEEE80211_CCK_RATE_2MB_MASK)

#define IEEE80211_OFDM_RATES_MASK               0x00000FF0
#define IEEE80211_OFDM_BASIC_RATES_MASK         (IEEE80211_OFDM_RATE_6MB_MASK | \
                                                IEEE80211_OFDM_RATE_12MB_MASK | \
                                                IEEE80211_OFDM_RATE_24MB_MASK)

#define RSN_CAPABILITY_PREAUTH                  BIT0
#define RSN_CAPABILITY_NO_PAIRWISE              BIT1
#define RSN_CAPABILITY_PTKSA_RC_SHIFT           2
#define RSN_CAPABILITY_PTKSA_RC_MASK            0x000C
#define RSN_CAPABILITY_GTKSA_RC_SHIFT           4
#define RSN_CAPABILITY_GTKSA_RC_MASK            0x0030
#define RSN_CAPABILITY_MFPR                     BIT6
#define RSN_CAPABILITY_MFPC                     BIT7
#define RSN_CAPABILITY_JOINT_MB_RSNA            BIT8
#define RSN_CAPABILITY_PEERKEY_EN               BIT9
#define RSN_CAPABILITY_SPP_AMSDU_CAP            BIT10
#define RSN_CAPABILITY_SPP_AMSDU_REQ            BIT11
#define RSN_CAPABILITY_PBAC                     BIT12
#define RSN_CAPABILITY_EXT_KEY_ID               BIT13

/* Extended Capabilities field */
#define WLAN_EXT_CAPAB_20_40_COEX               BIT0
#define WLAN_EXT_CAPAB_GLK                      BIT1
#define WLAN_EXT_CAPAB_EXT_CHAN_SWITCH          BIT2
#define WLAN_EXT_CAPAB_GLK_GCR                  BIT3
#define WLAN_EXT_CAPAB_PSMP                     BIT4
#define WLAN_EXT_CAPAB_S_PSMP                   BIT6
#define WLAN_EXT_CAPAB_EVENT                    BIT7

/* IEEE Std 802.11-2012, 8.4.2.62 20/40 BSS Coexistence element */
#define WLAN_20_40_BSS_COEX_INFO_REQ            BIT0
#define WLAN_20_40_BSS_COEX_40MHZ_INTOL         BIT1
#define WLAN_20_40_BSS_COEX_20MHZ_WIDTH_REQ     BIT2
#define WLAN_20_40_BSS_COEX_OBSS_EXEMPT_REQ     BIT3
#define WLAN_20_40_BSS_COEX_OBSS_EXEMPT_GRNT    BIT4

/**
* @brief rate defined in ieee80211
*/
enum MGN_RATE{
    MGN_1M      = 0x02,
    MGN_2M      = 0x04,
    MGN_5_5M    = 0x0B,
    MGN_6M      = 0x0C,
    MGN_9M      = 0x12,
    MGN_11M     = 0x16,
    MGN_12M     = 0x18,
    MGN_18M     = 0x24,
    MGN_24M     = 0x30,
    MGN_36M     = 0x48,
    MGN_48M     = 0x60,
    MGN_54M     = 0x6C,
    MGN_MCS32   = 0x7F,
    MGN_MCS0,
    MGN_MCS1,
    MGN_MCS2,
    MGN_MCS3,
    MGN_MCS4,
    MGN_MCS5,
    MGN_MCS6,
    MGN_MCS7,
    MGN_MCS8,
    MGN_MCS9,
    MGN_MCS10,
    MGN_MCS11,
    MGN_MCS12,
    MGN_MCS13,
    MGN_MCS14,
    MGN_MCS15,
    MGN_VHT1SS_MCS0,
    MGN_VHT1SS_MCS1,
    MGN_VHT1SS_MCS2,
    MGN_VHT1SS_MCS3,
    MGN_VHT1SS_MCS4,
    MGN_VHT1SS_MCS5,
    MGN_VHT1SS_MCS6,
    MGN_VHT1SS_MCS7,
    MGN_VHT1SS_MCS8,
    MGN_VHT1SS_MCS9,
    MGN_VHT2SS_MCS0,
    MGN_VHT2SS_MCS1,
    MGN_VHT2SS_MCS2,
    MGN_VHT2SS_MCS3,
    MGN_VHT2SS_MCS4,
    MGN_VHT2SS_MCS5,
    MGN_VHT2SS_MCS6,
    MGN_VHT2SS_MCS7,
    MGN_VHT2SS_MCS8,
    MGN_VHT2SS_MCS9,
    MGN_UNKNOWN
};

#define IS_HT_RATE(_rate)   (_rate >= MGN_MCS0 && _rate <= MGN_MCS15)
#define IS_VHT_RATE(_rate)  (_rate >= MGN_VHT1SS_MCS0 && _rate <= MGN_VHT2SS_MCS9)
#define IS_CCK_RATE(_rate)  (MGN_1M == _rate || _rate == MGN_2M || _rate == MGN_5_5M || _rate == MGN_11M )
#define IS_OFDM_RATE(_rate) (MGN_6M <= _rate && _rate <= MGN_54M  && _rate != MGN_11M)

/* Frame control field constants */
#define IEEE80211_FCTL_VERS             0x0002
#define IEEE80211_FCTL_FTYPE            0x000c
#define IEEE80211_FCTL_STYPE            0x00f0
#define IEEE80211_FCTL_TODS             0x0100
#define IEEE80211_FCTL_FROMDS           0x0200
#define IEEE80211_FCTL_MOREFRAGS        0x0400
#define IEEE80211_FCTL_RETRY            0x0800
#define IEEE80211_FCTL_PM               0x1000
#define IEEE80211_FCTL_MOREDATA         0x2000
#define IEEE80211_FCTL_WEP              0x4000
#define IEEE80211_FCTL_ORDER            0x8000

#define IEEE80211_FTYPE_MGMT            0x0000
#define IEEE80211_FTYPE_CTL             0x0004
#define IEEE80211_FTYPE_DATA            0x0008

/* Management */
#define IEEE80211_STYPE_ASSOC_REQ       0x0000
#define IEEE80211_STYPE_ASSOC_RESP      0x0010
#define IEEE80211_STYPE_REASSOC_REQ     0x0020
#define IEEE80211_STYPE_REASSOC_RESP    0x0030
#define IEEE80211_STYPE_PROBE_REQ       0x0040
#define IEEE80211_STYPE_PROBE_RESP      0x0050
#define IEEE80211_STYPE_BEACON          0x0080
#define IEEE80211_STYPE_ATIM            0x0090
#define IEEE80211_STYPE_DISASSOC        0x00A0
#define IEEE80211_STYPE_AUTH            0x00B0
#define IEEE80211_STYPE_DEAUTH          0x00C0
#define IEEE80211_STYPE_ACTION          0x00D0

/* Control */
#define IEEE80211_STYPE_PSPOLL          0x00A0
#define IEEE80211_STYPE_RTS             0x00B0
#define IEEE80211_STYPE_CTS             0x00C0
#define IEEE80211_STYPE_ACK             0x00D0
#define IEEE80211_STYPE_CFEND           0x00E0
#define IEEE80211_STYPE_CFENDACK        0x00F0

/* Data */
#define IEEE80211_STYPE_DATA            0x0000
#define IEEE80211_STYPE_DATA_CFACK      0x0010
#define IEEE80211_STYPE_DATA_CFPOLL     0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL  0x0030
#define IEEE80211_STYPE_NULLFUNC        0x0040
#define IEEE80211_STYPE_CFACK           0x0050
#define IEEE80211_STYPE_CFPOLL          0x0060
#define IEEE80211_STYPE_CFACKPOLL       0x0070
#define IEEE80211_QOS_DATAGRP           0x0080

#define IEEE80211_SCTL_FRAG             0x000F
#define IEEE80211_SCTL_SEQ              0xFFF0

#define WLAN_FC_GET_TYPE(fc)            ((fc) & IEEE80211_FCTL_FTYPE)
#define WLAN_FC_GET_STYPE(fc)           ((fc) & IEEE80211_FCTL_STYPE)
#define WLAN_QC_GET_TID(qc)             ((qc) & 0x0f)
#define WLAN_GET_SEQ_FRAG(seq)          ((seq) & IEEE80211_SCTL_FRAG)
#define WLAN_GET_SEQ_SEQ(seq)           ((seq) & IEEE80211_SCTL_SEQ)

/* Queue select value in Tx desc */
#define QSLT_BK                         0x2
#define QSLT_BE                         0x0
#define QSLT_VI                         0x5
#define QSLT_VO                         0x7
#define QSLT_BEACON                     0x10
#define QSLT_HIGH                       0x11
#define QSLT_MGNT                       0x12
#define QSLT_CMD                        0x13

#define TRY_AP_TIMES                    3

/**
* @brief management action category
*/
typedef enum ieee80211_category {
    WLAN_CATEGORY_SPECTRUM_MGMT     = 0,
    WLAN_CATEGORY_QOS               = 1,
    WLAN_CATEGORY_DLS               = 2,
    WLAN_CATEGORY_BACK              = 3,
    WLAN_CATEGORY_PUBLIC            = 4,
    WLAN_CATEGORY_RADIO_MEASUREMENT = 5,
    WLAN_CATEGORY_FT                = 6,
    WLAN_CATEGORY_HT                = 7,
    WLAN_CATEGORY_SA_QUERY          = 8,
    WLAN_CATEGORY_WMM               = 17,
    WLAN_CATEGORY_VHT               = 21
} IEEE80211_CATEGORY;

/**
* @brief Public action
*/
typedef enum ieee80211_public_action {
    WLAN_PA_20_40_BSS_COEX              = 0,
    WLAN_PA_DSE_ENABLEMENT              = 1,
    WLAN_PA_DSE_DEENABLEMENT            = 2,
    WLAN_PA_DSE_REG_LOCATION_ANNOUNCE   = 3,
    WLAN_PA_EXT_CHANNEL_SWITCH_ANNOUNCE = 4,
    WLAN_PA_DSE_MEASUREMENT_REQ         = 5,
    WLAN_PA_DSE_MEASUREMENT_RESP        = 6,
    WLAN_PA_MEASUREMENT_PILOT           = 7,
    WLAN_PA_DSE_POWER_CONSTRAINT        = 8,
    WLAN_PA_VENDOR_SPECIFIC             = 9,
    WLAN_PA_GAS_INITIAL_REQ             = 10,
    WLAN_PA_GAS_INITIAL_RESP            = 11,
    WLAN_PA_GAS_COMEBACK_REQ            = 12,
    WLAN_PA_GAS_COMEBACK_RESP           = 13,
    WLAN_TDLS_DISCOVERY_RESPONSE        = 14,
    WLAN_PA_LOCATION_TRACK_NOTIFICATION = 15,
    WLAN_PA_QAB_REQUEST_FRAME           = 16,
    WLAN_PA_QAB_RESPONSE_FRAME          = 17,
    WLAN_PA_QMF_POLICY                  = 18,
    WLAN_PA_QMF_POLICY_CHANGE           = 19,
    WLAN_PA_QLOAD_REQUEST               = 20,
    WLAN_PA_QLOAD_REPORT                = 21,
    WLAN_PA_HCCA_TXOP_ADVERTISEMENT     = 22,
    WLAN_PA_HCCA_TXOP_RESPONSE          = 23,
    WLAN_PA_PUBLIC_KEY                  = 24,
    WLAN_PA_CHANNEL_AVAILABILITY_QUERY  = 25,
    WLAN_PA_CHANNEL_SCHEDULE_MANAGEMENT = 26,
    WLAN_PA_CONTACT_VERIFICATION_SIGNAL = 27,
    WLAN_PA_GDD_ENABLEMENT_REQ          = 28,
    WLAN_PA_GDD_ENABLEMENT_RESP         = 29,
    WLAN_PA_NETWORK_CHANNEL_CONTROL     = 30,
    WLAN_PA_WHITE_SPACE_MAP_ANNOUNCEMENT = 31,
    WLAN_PA_FTM_REQUEST                 = 32,
    WLAN_PA_FTM                         = 33,
    WLAN_PA_FILS_DISCOVERY              = 34
} IEEE80211_PUBLIC_ACTION;

/**
* @brief Block ACK action
*/
typedef enum ieee80211_back_action {
    WLAN_ACTION_ADDBA_REQ   = 0,
    WLAN_ACTION_ADDBA_RESP  = 1,
    WLAN_ACTION_DELBA       = 2
} IEEE80211_BACK_ACTION;

/**
* @brief SA Query action
*/
typedef enum ieee80211_sa_query_action {
    WLAN_ACTION_SA_QUERY_REQ = 0,
    WLAN_ACTION_SA_QUERY_RESP = 1
} IEEE80211_SA_QUERY_ACTION;

#ifdef CONFIG_80211AC_SUPPORT
/**
* @brief VHT management action
*/
typedef enum ieee80211_vht_action{
    WLAN_ACTION_VHT_COMPRESSED_BEAMFORMING  = 0,
    WLAN_ACTION_VHT_GROUPID_MANAGEMENT      = 1,
    WLAN_ACTION_VHT_OPMODE_NOTIFICATION     = 2,
} IEEE80211_VHT_ACTION;
#endif

/**
* @brief WiFi packet frame type
*/
typedef enum {
    WIFI_MGT_TYPE  = (0),
    WIFI_CTRL_TYPE = (BIT(2)),
    WIFI_DATA_TYPE = (BIT(3))
} WIFI_FRAME_TYPE;

/**
* @brief WiFi media status
*/
typedef enum {
    WLAN_MEDIA_DISCONNECT  = 0,
    WLAN_MEDIA_CONNECT     = 1
} WLAN_MEDIA_STATUS;

/**
* @brief WiFi packet frame subtype
*/
typedef enum {

    WIFI_ASSOCREQ       = (0 | WIFI_MGT_TYPE),
    WIFI_ASSOCRSP       = (BIT(4) | WIFI_MGT_TYPE),
    WIFI_REASSOCREQ     = (BIT(5) | WIFI_MGT_TYPE),
    WIFI_REASSOCRSP     = (BIT(5) | BIT(4) | WIFI_MGT_TYPE),
    WIFI_PROBEREQ       = (BIT(6) | WIFI_MGT_TYPE),
    WIFI_PROBERSP       = (BIT(6) | BIT(4) | WIFI_MGT_TYPE),
    WIFI_BEACON         = (BIT(7) | WIFI_MGT_TYPE),
    WIFI_ATIM           = (BIT(7) | BIT(4) | WIFI_MGT_TYPE),
    WIFI_DISASSOC       = (BIT(7) | BIT(5) | WIFI_MGT_TYPE),
    WIFI_AUTH           = (BIT(7) | BIT(5) | BIT(4) | WIFI_MGT_TYPE),
    WIFI_DEAUTH         = (BIT(7) | BIT(6) | WIFI_MGT_TYPE),
    WIFI_ACTION         = (BIT(7) | BIT(6) | BIT(4) | WIFI_MGT_TYPE),

    WIFI_BAR            = (BIT(7) | WIFI_CTRL_TYPE),
    WIFI_PSPOLL         = (BIT(7) | BIT(5) | WIFI_CTRL_TYPE),
    WIFI_RTS            = (BIT(7) | BIT(5) | BIT(4) | WIFI_CTRL_TYPE),
    WIFI_CTS            = (BIT(7) | BIT(6) | WIFI_CTRL_TYPE),
    WIFI_ACK            = (BIT(7) | BIT(6) | BIT(4) | WIFI_CTRL_TYPE),
    WIFI_CFEND          = (BIT(7) | BIT(6) | BIT(5) | WIFI_CTRL_TYPE),
    WIFI_CFEND_CFACK    = (BIT(7) | BIT(6) | BIT(5) | BIT(4) | WIFI_CTRL_TYPE),

    WIFI_DATA           = (0 | WIFI_DATA_TYPE),
    WIFI_DATA_CFACK     = (BIT(4) | WIFI_DATA_TYPE),
    WIFI_DATA_CFPOLL    = (BIT(5) | WIFI_DATA_TYPE),
    WIFI_DATA_CFACKPOLL = (BIT(5) | BIT(4) | WIFI_DATA_TYPE),
    WIFI_DATA_NULL      = (BIT(6) | WIFI_DATA_TYPE),
    WIFI_QOS_DATA_TYPE  = (BIT(7)|WIFI_DATA_TYPE),
    WIFI_QOSDATA_NULL   = (BIT(7) | BIT(6) | WIFI_DATA_TYPE),
    WIFI_CF_ACK         = (BIT(6) | BIT(4) | WIFI_DATA_TYPE),
    WIFI_CF_POLL        = (BIT(6) | BIT(5) | WIFI_DATA_TYPE),
    WIFI_CF_ACKPOLL     = (BIT(6) | BIT(5) | BIT(4) | WIFI_DATA_TYPE)
} WIFI_FRAME_SUBTYPE;

/**
* @brief WiFi reason code
*/
typedef enum {
    REASON_RESERVED                     = 0,
    REASON_UNSPECIFIED                  = 1,
    REASON_AUTH_NO_LONGER_VALID         = 2,
    REASON_DEAUTH_STA_LEAVING           = 3,
    REASON_INACTIVITY                   = 4,
    REASON_UNABLE_HANDLE                = 5,
    REASON_CLASS_2                      = 6,
    REASON_CLASS_3                      = 7,
    REASON_DISAOC_STA_LEAVING           = 8,
    REASON_ASOC_NOT_AUTH                = 9,

    // WPA reason
    REASON_INVALID_IE                   = 13,
    REASON_MIC_FAILURE                  = 14,
    REASON_4WAY_HNDSHK_TIMEOUT          = 15,
    REASON_GROUP_KEY_UPDATE_TIMEOUT     = 16,
    REASON_DIFF_IE                      = 17,
    REASON_MLTCST_CIPHER_NOT_VALID      = 18,
    REASON_UNICST_CIPHER_NOT_VALID      = 19,
    REASON_AKMP_NOT_VALID               = 20,
    REASON_UNSUPPORT_RSNE_VER           = 21,
    REASON_INVALID_RSNE_CAP_            = 22,
    REASON_IEEE_802DOT1X_AUTH_FAIL      = 23,
    REASON_PMK_NOT_AVAILABLE            = 24
} WIFI_REASON_CODE;

/**
* @brief WiFi status code
*/
typedef enum {
    STATUS_SUCCESSFUL                   = 0,
    STATUS_FAILURE                      = 1,
    STATUS_CAP_FAIL                     = 10,
    STATUS_NO_ASOC                      = 11,
    STATUS_OTHER                        = 12,
    STATUS_NO_SUPP_ALG                  = 13,
    STATUS_OUT_OF_AUTH_SEQ              = 14,
    STATUS_CHALLENGE_FAIL               = 15,
    STATUS_AUTH_TIMEOUT                 = 16,
    STATUS_UNABLE_HANDLE_STA            = 17,
    STATUS_RATE_FAIL                    = 18,
    STATUS_REFUSED_TEMPORARILY          = 30,
    STATUS_ANTI_CLOGGING_TOKEN_REQ      = 76,
    STATUS_FINITE_CYCLIC_GROUP_NOT_SUPP = 77,
    STATUS_MAX
} WIFI_STATUS_CODE;

/**
* @brief WiFi channel plan
*/
typedef enum {
    CHPL_FCC    = 0,
    CHPL_IC     = 1,
    CHPL_ETSI   = 2,
    CHPL_SPAIN  = 3,
    CHPL_FRANCE = 4,
    CHPL_MKK    = 5,
    CHPL_MKK1   = 6,
    CHPL_ISRAEL = 7,
    CHPL_TELEC  = 8,
    CHPL_GLOBAL = 9,
    CHPL_WORLD  = 10
} CHANNEL_PLAN;

/**
* @brief WiFi wireless mode
*/
typedef enum {
    WIRELESS_INVALID  = 0,
    WIRELESS_11B      = BIT(0),
    WIRELESS_11G      = BIT(1),
    WIRELESS_11A      = BIT(2),
    WIRELESS_11_24N   = BIT(3),
    WIRELESS_11_5N    = BIT(4),
    WIRELESS_11AC     = BIT(5),
    WIRELESS_AUTO     = BIT(6),

    //Combination
    WIRELESS_11BG     = (WIRELESS_11B|WIRELESS_11G),
    WIRELESS_11ABG    = (WIRELESS_11B|WIRELESS_11G|WIRELESS_11A),
    WIRELESS_11GN     = (WIRELESS_11G|WIRELESS_11_24N),
    WIRELESS_11AN     = (WIRELESS_11A|WIRELESS_11_5N),
    WIRELESS_11BGN    = (WIRELESS_11B|WIRELESS_11G|WIRELESS_11_24N),
    WIRELESS_11AGN    = (WIRELESS_11A|WIRELESS_11G|WIRELESS_11_24N|WIRELESS_11_5N),
    WIRELESS_11ABGN   = (WIRELESS_11A|WIRELESS_11B|WIRELESS_11G|WIRELESS_11_24N|WIRELESS_11_5N),
    WIRELESS_11BGN_AC = (WIRELESS_11B|WIRELESS_11G|WIRELESS_11_24N|WIRELESS_11AC),
    WIRELESS_11_AC_5G = (WIRELESS_11A|WIRELESS_11_5N|WIRELESS_11AC),
} WIRELESS_MODE;

/**
* @brief EAP packet type
*/
typedef enum eap_type {
    EAP_PACKET = 0,
    EAPOL_START,
    EAPOL_LOGOFF,
    EAPOL_KEY,
    EAPOL_ENCAP_ASF_ALERT
} EAP_TYPE;

/**
* @brief snap header format
*/
typedef struct ieee80211_snap_hdr {
    uint8_t dsap;
    uint8_t ssap;
    uint8_t ctrl;
    uint8_t oui[WLAN_IEEE_OUI_LEN];
} __PACKED ieee80211_snap_hdr_t;

/**
* @brief wlan header format
*/
typedef struct wlan_80211_hdr {
    uint16_t frame_ctl;
    uint16_t duration_id;
    uint8_t addr1[ETH_ALEN];
    uint8_t addr2[ETH_ALEN];
    uint8_t addr3[ETH_ALEN];
    uint16_t seq_ctl;
    uint8_t addr4[ETH_ALEN];
} __PACKED wlan_80211_hdr_t;

/**
* @brief wlan 3-address header format
*/
typedef struct wlan_80211_hdr_3addr {
    uint16_t frame_ctl;
    uint16_t duration_id;
    uint8_t addr1[ETH_ALEN];
    uint8_t addr2[ETH_ALEN];
    uint8_t addr3[ETH_ALEN];
    uint16_t seq_ctl;
} __PACKED wlan_80211_hdr_3addr_t;

/**
 * @brief wlan qos header format
 */
typedef struct wlan_80211_hdr_qos {
    struct wlan_80211_hdr wlan_hdr;
    uint16_t qc;
} __PACKED wlan_80211_hdr_qos_t;

/**
 * @brief wlan 3-address qos header format
 */
typedef struct  wlan_80211_hdr_3addr_qos {
    struct wlan_80211_hdr_3addr wlan_hdr;
    uint16_t qc;
} __PACKED wlan_80211_hdr_3addr_qos_t;

/**
 * @brief EAPOL frame header format
 */
typedef struct eapol {
    uint8_t snap[6];
    uint16_t ethertype;
    uint8_t version;
    uint8_t type;
    uint16_t length;
} __PACKED eapol_t;

#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)

#define _TO_DS_     BIT(8)
#define _FROM_DS_   BIT(9)
#define _MORE_FRAG_ BIT(10)
#define _RETRY_     BIT(11)
#define _PWRMGT_    BIT(12)
#define _MORE_DATA_ BIT(13)
#define _PRIVACY_   BIT(14)
#define _ORDER_     BIT(15)

#ifdef CONFIG_BIG_ENDIAN

#define SetToDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_TO_DS_); \
    } while(0)

#define GetToDs(pbuf) ((((*(uint16_t *)(pbuf)) & be16_to_cpu(_TO_DS_)) != 0) ? 1 : 0)

#define ClearToDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_TO_DS_)); \
    } while(0)

#define SetFrDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_FROM_DS_); \
    } while(0)

#define GetFrDs(pbuf)  ((((*(uint16_t *)(pbuf)) & be16_to_cpu(_FROM_DS_)) != 0) ? 1 : 0)

#define ClearFrDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_FROM_DS_)); \
    } while(0)

#define GetToFrDs(pframe) ((GetToDs(pframe) << 1) | GetFrDs(pframe))


#define SetMFrag(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_MORE_FRAG_); \
    } while(0)

#define GetMFrag(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_MORE_FRAG_)) != 0)

#define ClearMFrag(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_MORE_FRAG_)); \
    } while(0)

#define SetRetry(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_RETRY_); \
    } while(0)

#define GetRetry(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_RETRY_)) != 0)

#define ClearRetry(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_RETRY_)); \
    } while(0)

#define SetPwrMgt(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_PWRMGT_); \
    } while(0)

#define GetPwrMgt(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_PWRMGT_)) != 0)

#define ClearPwrMgt(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_PWRMGT_)); \
    } while(0)

#define SetMData(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_MORE_DATA_); \
    } while(0)

#define GetMData(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_MORE_DATA_)) != 0)

#define ClearMData(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_MORE_DATA_)); \
    } while(0)

#define SetPrivacy(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(_PRIVACY_); \
    } while(0)

#define GetPrivacy(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_PRIVACY_)) != 0)

#define ClearPrivacy(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_be16(_PRIVACY_)); \
    } while(0)


#define GetOrder(pbuf) (((*(uint16_t *)(pbuf)) & be16_to_cpu(_ORDER_)) != 0)

#define GetFrameType(pbuf) (be16_to_cpu(*(uint16_t *)(pbuf)) & (BIT(3) | BIT(2)))

#define SetFrameType(pbuf,type) \
    do {  \
        *(uint16_t *)(pbuf) &= __constant_cpu_to_be16(~(BIT(3) | BIT(2))); \
        *(uint16_t *)(pbuf) |= __constant_cpu_to_be16(type); \
    } while(0)

#define GetFrameSubType(pbuf) (cpu_to_be16(*(uint16_t *)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))

#define SetFrameSubType(pbuf,type) \
do {    \
    *(uint16_t *)(pbuf) &= cpu_to_be16((uint16_t)(~(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))); \
    *(uint16_t *)(pbuf) |= cpu_to_be16((uint16_t)type); \
} while(0)

#define GetSequence(pbuf) (cpu_to_be16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) >> 4)

#define GetFragNum(pbuf) (cpu_to_be16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & 0x0f)

#define GetTupleCache(pbuf) (cpu_to_be16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)))

#define SetFragNum(pbuf, num) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & be16_to_cpu((uint16_t)(~0x000f))) | \
            cpu_to_be16((uint16_t)(0x000f) & (num));     \
    } while(0)


#define SetSeqNum(pbuf, num) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & be16_to_cpu((uint16_t)~0xfff0)) | \
            be16_to_cpu((uint16_t)(0xfff0 & (num << 4))); \
    } while(0)


#define SetDuration(pbuf, dur) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 2) = cpu_to_be16(0xffff & (dur)); \
    } while(0)


#define SetPriority(pbuf, tid) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16(tid & 0xf); \
    } while(0)

#define GetPriority(pbuf) ((be16_to_cpu(*(uint16_t *)(pbuf))) & 0xf)

#define SetAckpolicy(pbuf, ack) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16( (ack & 3) << 5); \
    } while(0)

#define GetAckpolicy(pbuf) (((be16_to_cpu(*(uint16_t *)pbuf)) >> 5) & 0x3)

#define GetAMsdu(pbuf) (((be16_to_cpu(*(uint16_t *)pbuf)) >> 7) & 0x1)

#define SetAMsdu(pbuf, amsdu) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_be16( (amsdu & 1) << 7); \
    } while(0)

#define GetAid(pbuf) (cpu_to_be16(*(uint16_t *)((SIZE_PTR)(pbuf) + 2)) & 0x3fff)

#define GetTid(pbuf) (cpu_to_be16(*(uint16_t *)((SIZE_PTR)(pbuf) + (((GetToDs(pbuf)<<1)|GetFrDs(pbuf))==3?30:24))) & 0x000f)

#else

#define SetToDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_TO_DS_); \
    } while(0)

#define GetToDs(pbuf) ((((*(uint16_t *)(pbuf)) & le16_to_cpu(_TO_DS_)) != 0) ? 1 : 0)

#define ClearToDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_TO_DS_)); \
    } while(0)

#define SetFrDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_FROM_DS_); \
    } while(0)

#define GetFrDs(pbuf)  ((((*(uint16_t *)(pbuf)) & le16_to_cpu(_FROM_DS_)) != 0) ? 1 : 0)

#define ClearFrDs(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_FROM_DS_)); \
    } while(0)

#define GetToFrDs(pframe) ((GetToDs(pframe) << 1) | GetFrDs(pframe))


#define SetMFrag(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_MORE_FRAG_); \
    } while(0)

#define GetMFrag(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_MORE_FRAG_)) != 0)

#define ClearMFrag(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_MORE_FRAG_)); \
    } while(0)

#define SetRetry(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_RETRY_); \
    } while(0)

#define GetRetry(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_RETRY_)) != 0)

#define ClearRetry(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_RETRY_)); \
    } while(0)

#define SetPwrMgt(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_PWRMGT_); \
    } while(0)

#define GetPwrMgt(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_PWRMGT_)) != 0)

#define ClearPwrMgt(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_PWRMGT_)); \
    } while(0)

#define SetMData(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_MORE_DATA_); \
    } while(0)

#define GetMData(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_MORE_DATA_)) != 0)

#define ClearMData(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_MORE_DATA_)); \
    } while(0)

#define SetPrivacy(pbuf) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(_PRIVACY_); \
    } while(0)

#define GetPrivacy(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_PRIVACY_)) != 0)

#define ClearPrivacy(pbuf) \
    do { \
        *(uint16_t *)(pbuf) &= (~ cpu_to_le16(_PRIVACY_)); \
    } while(0)


#define GetOrder(pbuf) (((*(uint16_t *)(pbuf)) & le16_to_cpu(_ORDER_)) != 0)

#define GetFrameType(pbuf) (le16_to_cpu(*(uint16_t *)(pbuf)) & (BIT(3) | BIT(2)))

#define SetFrameType(pbuf,type) \
    do {  \
        *(uint16_t *)(pbuf) &= __constant_cpu_to_le16(~(BIT(3) | BIT(2))); \
        *(uint16_t *)(pbuf) |= __constant_cpu_to_le16(type); \
    } while(0)

#define GetFrameSubType(pbuf) (cpu_to_le16(*(uint16_t *)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))

#define SetFrameSubType(pbuf,type) \
do {    \
    *(uint16_t *)(pbuf) &= cpu_to_le16((uint16_t)(~(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))); \
    *(uint16_t *)(pbuf) |= cpu_to_le16((uint16_t)type); \
} while(0)

#define GetSequence(pbuf) (cpu_to_le16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) >> 4)

#define GetFragNum(pbuf) (cpu_to_le16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & 0x0f)

#define GetTupleCache(pbuf) (cpu_to_le16(*(uint16_t *)((SIZE_PTR)(pbuf) + 22)))

#define SetFragNum(pbuf, num) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & le16_to_cpu((uint16_t)(~0x000f))) | \
            cpu_to_le16((uint16_t)(0x000f) & (num));     \
    } while(0)


#define SetSeqNum(pbuf, num) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 22) = \
            ((*(uint16_t *)((SIZE_PTR)(pbuf) + 22)) & le16_to_cpu((uint16_t)~0xfff0)) | \
            le16_to_cpu((uint16_t)(0xfff0 & (num << 4))); \
    } while(0)


#define SetDuration(pbuf, dur) \
    do {    \
        *(uint16_t *)((SIZE_PTR)(pbuf) + 2) = cpu_to_le16(0xffff & (dur)); \
    } while(0)


#define SetPriority(pbuf, tid) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16(tid & 0xf); \
    } while(0)

#define GetPriority(pbuf) ((le16_to_cpu(*(uint16_t *)(pbuf))) & 0xf)

#define SetEOSP(pbuf, eosp) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16( (eosp & 1) << 4); \
    } while(0)

#define SetAckpolicy(pbuf, ack) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16( (ack & 3) << 5); \
    } while(0)

#define GetAckpolicy(pbuf) (((le16_to_cpu(*(uint16_t *)pbuf)) >> 5) & 0x3)

#define GetAMsdu(pbuf) (((le16_to_cpu(*(uint16_t *)pbuf)) >> 7) & 0x1)

#define SetAMsdu(pbuf, amsdu) \
    do { \
        *(uint16_t *)(pbuf) |= cpu_to_le16( (amsdu & 1) << 7); \
    } while(0)

#define GetAid(pbuf) (cpu_to_le16(*(uint16_t *)((SIZE_PTR)(pbuf) + 2)) & 0x3fff)

#define GetTid(pbuf) (cpu_to_le16(*(uint16_t *)((SIZE_PTR)(pbuf) + (((GetToDs(pbuf)<<1)|GetFrDs(pbuf))==3?30:24))) & 0x000f)

#endif

#define GetAddr1Ptr(pbuf) ((uint8_t *)((SIZE_PTR)(pbuf) + 4))

#define GetAddr2Ptr(pbuf) ((uint8_t *)((SIZE_PTR)(pbuf) + 10))

#define GetAddr3Ptr(pbuf) ((uint8_t *)((SIZE_PTR)(pbuf) + 16))

#define GetAddr4Ptr(pbuf) ((uint8_t *)((SIZE_PTR)(pbuf) + 24))

#define MacAddr_isBcst(addr) is_broadcast_ether_addr(addr)

/**
 * @brief This function check whether the MAC address is multicast address or not
 * @param da MAC address
 * @return check result
 */
__inline static int32_t IS_MCAST(uint8_t *da)
{
    if ((*da) & 0x01)
        return _TRUE;
    else
        return _FALSE;
}

/**
 * @brief This function get destination MAC address from the frame
 * @param pframe pointer to the frame
 * @return pointer to the DA
 */
__inline static uint8_t *get_da(uint8_t *pframe)
{
    uint8_t *da;
    uint32_t to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds) {
        case 0x00:
            da = GetAddr1Ptr(pframe);
            break;
        case 0x01:
            da = GetAddr1Ptr(pframe);
            break;
        case 0x02:
            da = GetAddr3Ptr(pframe);
            break;
        default:
            da = GetAddr3Ptr(pframe);
            break;
    }

    return da;
}

/**
 * @brief This function get source MAC address from the frame
 * @param pframe pointer to the frame
 * @return pointer to the SA
 */
__inline static uint8_t *get_sa(uint8_t *pframe)
{
    uint8_t *sa;
    uint32_t to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds) {
        case 0x00:
            sa = GetAddr2Ptr(pframe);
            break;
        case 0x01:
            sa = GetAddr3Ptr(pframe);
            break;
        case 0x02:
            sa = GetAddr2Ptr(pframe);
            break;
        default:
            sa = GetAddr4Ptr(pframe);
            break;
    }

    return sa;
}

/**
 * @brief This function get BSSID from the frame
 * @param pframe pointer to the frame
 * @return pointer to the BSSID
 */
__inline static uint8_t *get_hdr_bssid(uint8_t *pframe)
{
    uint8_t *bssid;
    uint32_t to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);

    switch (to_fr_ds) {
        case 0x00:
            bssid = GetAddr3Ptr(pframe);
            break;
        case 0x01:
            bssid = GetAddr2Ptr(pframe);
            break;
        case 0x02:
            bssid = GetAddr1Ptr(pframe);
            break;
        default:
            bssid = GetAddr2Ptr(pframe);
            break;
    }

    return bssid;
}

/**
 * @brief This function check whether the frame is control frame or not
 * @param pframe pointer to the frame
 * @return check result
 */
__inline static int32_t IsFrameTypeCtrl(uint8_t *pframe)
{
    if (WIFI_CTRL_TYPE == GetFrameType(pframe))
        return _TRUE;
    else
        return _FALSE;
}

/**
 * @brief ieee80211 HT capability element format
 */
typedef struct ieee80211_ht_cap {
    uint16_t HT_caps_info;
    uint8_t AMPDU_para;
    uint8_t MCS_rate[16];
    uint16_t HT_ext_caps;
    uint32_t Beamforming_caps;
    uint8_t ASEL_caps;
} __PACKED ieee80211_ht_cap_t;

/**
 * @brief ieee80211 HT capability element format as union
 */
struct HT_caps_element {
    union {
        struct ieee80211_ht_cap HT_cap_element;
        uint8_t HT_cap[26];
    } u;
};

/**
 * @brief ieee80211 HT operation element format
 */
typedef struct HT_operation_element {
    uint8_t primary_channel;
    uint8_t infos[5];
    uint8_t MCS_rate[16];
} __PACKED HT_operation_element_t;

/**
 * @brief ieee80211 overlapping BSS scan parameters element format
 */
struct obss_scan_parameters {
	uint16_t scan_passive_dwell;
	uint16_t scan_active_dwell;
	uint16_t width_trigger_scan_interval;
	uint16_t scan_passive_total_per_channel;
	uint16_t scan_active_total_per_channel;
	uint16_t channel_transition_delay_factor;
	uint16_t scan_activity_threshold;
} __PACKED;

/**
 * @brief ieee80211 20/40 BSS coexistence element format
 */
struct ieee80211_2040_bss_coex_ie {
	uint8_t element_id;
	uint8_t length;
	uint8_t coex_param;
} __PACKED;

/**
 * @brief ieee80211 20/40 BSS intolerant channel report element format
 */
struct ieee80211_2040_intol_chan_report {
	uint8_t element_id;
	uint8_t length;
	uint8_t op_class;
	uint8_t variable[NUM_24G_CHANNELS];
} __PACKED;

/**
 * @brief ieee80211 AC parameter format
 */
typedef struct AC_param {
    uint8_t ACI_AIFSN;
    uint8_t CW;
    uint16_t TXOP_limit;
} __PACKED AC_param_t;

/**
 * @brief ieee80211 WMM parameter element format
 */
typedef struct WMM_para_element {
    uint8_t QoS_info;
    uint8_t reserved;
    struct AC_param ac_param[4];
} __PACKED WMM_para_element_t;

/**
 * @brief Management MIC element (IEEE 802.11w)
 */
typedef struct ieee80211_mme {
    uint8_t element_id;
    uint8_t length;
    uint16_t key_id;
    uint8_t ipn[6];
    uint8_t mic[8];
} __PACKED ieee80211_mme_t;

/**
 * @brief ieee80211 ADDBA request format
 */
typedef struct ADDBA_request {
    uint8_t dialog_token;
    uint16_t BA_para_set;
    uint16_t BA_timeout_value;
    uint16_t BA_starting_seqctrl;
} __PACKED ADDBA_request_t;

/* 802.11n HT capabilities masks */
#define IEEE80211_HT_CAP_SUP_WIDTH           0x0002
#define IEEE80211_HT_CAP_SM_PS               0x000C
#define IEEE80211_HT_CAP_GRN_FLD             0x0010
#define IEEE80211_HT_CAP_SGI_20              0x0020
#define IEEE80211_HT_CAP_SGI_40              0x0040
#define IEEE80211_HT_CAP_TX_STBC             0x0080
#define IEEE80211_HT_CAP_RX_STBC             0x0300
#define IEEE80211_HT_CAP_DELAY_BA            0x0400
#define IEEE80211_HT_CAP_MAX_AMSDU           0x0800
#define IEEE80211_HT_CAP_DSSSCCK40           0x1000
#define IEEE80211_HT_CAP_40M_INTOLERANT      0x4000
#define IEEE80211_HT_CAP_AMPDU_FACTOR        0x03
#define IEEE80211_HT_CAP_AMPDU_DENSITY       0x1C
#define IEEE80211_HT_CAP_MAX_STREAMS         4
#define IEEE80211_SUPP_MCS_SET_LEN           10
#define IEEE80211_HT_CAP_MCS_TX_DEFINED      0x01
#define IEEE80211_HT_CAP_MCS_TX_RX_DIFF      0x02
#define IEEE80211_HT_CAP_MCS_TX_STREAMS      0x0C
#define IEEE80211_HT_CAP_MCS_TX_UEQM         0x10

/* 802.11n HT IE masks */
#define IEEE80211_HT_IE_SEC_CHAN_OFFSET      0x03
#define IEEE80211_HT_IE_SEC_CHAN_NONE        0x00
#define IEEE80211_HT_IE_SEC_CHAN_ABOVE       0x01
#define IEEE80211_HT_IE_SEC_CHAN_BELOW       0x03
#define IEEE80211_HT_IE_CHAN_WIDTH           0x04
#define IEEE80211_HT_IE_HT_PROTECTION        0x0003
#define IEEE80211_HT_IE_NON_GF_STA_PRSNT     0x0004
#define IEEE80211_HT_IE_NON_HT_STA_PRSNT     0x0010

/* Block-ack parameters */
#define IEEE80211_ADDBA_PARAM_POLICY_MASK    0x0002
#define IEEE80211_ADDBA_PARAM_TID_MASK       0x003C
#define IEEE80211_DELBA_PARAM_TID_MASK       0xF000
#define IEEE80211_DELBA_PARAM_INITIATOR_MASK 0x0800

#define HT_MIMO_PS_STATIC                    BIT(0)
#define HT_MIMO_PS_DYNAMIC                   BIT(1)

/**
 * @brief ieee80211 preamble mode
 */
typedef enum {
    PREAMBLE_LONG   = 1,
    PREAMBLE_AUTO   = 2,
    PREAMBLE_SHORT  = 3
} PREAMBLE_MODE;

/**
 * @brief ieee80211 band
 */
typedef enum {
    IEEE80211_BAND_2G   = 0,
    IEEE80211_BAND_5G   = 1,
    IEEE80211_BAND_MAX  = 2
} IEEE80211_BAND;

/**
 * @brief ieee80211 5g subband
 */
typedef enum {
    IEEE80211_5G_SUBBAND_INVALID = 0,
    IEEE80211_5G_SUBBAND_1       = 1,
    IEEE80211_5G_SUBBAND_2       = 2,
    IEEE80211_5G_SUBBAND_MAX
} IEEE80211_5G_SUBBAND;

/**
 * @brief ieee80211 channel width in HT capabilities
 */
typedef enum {
    CHANNEL_WIDTH_20    = 0,
    CHANNEL_WIDTH_40    = 1,
    CHANNEL_WIDTH_80    = 2,
    CHANNEL_WIDTH_160   = 3,
    CHANNEL_WIDTH_80_80 = 4,
    CHANNEL_WIDTH_MAX   = 5,
} CHANNEL_WIDTH;

/**
 * @brief ieee80211 extention channel offset in HT operation
 */
typedef enum {
    HT_SEC_CHNL_OFFSET_NONE  = 0,
    HT_SEC_CHNL_OFFSET_ABOVE = 1,
    HT_SEC_CHNL_OFFSET_RESV  = 2,
    HT_SEC_CHNL_OFFSET_BELOW = 3,
    HT_SEC_CHNL_OFFSET_MAX  = 4
} HT_SEC_CHNL_OFFSET;

/**
 * @brief ieee80211 Maximum RX AMPDU size in HT capability
 */
typedef enum {
    HT_MAX_RX_AMPDU_SZ_16K = 1,
    HT_MAX_RX_AMPDU_SZ_32K  = 2,
    HT_MAX_RX_AMPDU_SZ_64K = 3,
} HT_MAX_RX_AMPDU_SZ;

/**
 * @brief WiFi RF type
 */
typedef enum {
    RF_1T1R     = 0,
    RF_1T2R     = 1,
    RF_2T2R     = 2,
    RF_MAX_TYPE = 3
} WLAN_RF_TYPE;

/**
 * @brief ieee80211 network type
 */
typedef enum {
    IEEE80211_FH,
    IEEE80211_DS,
    IEEE80211_OFDM5,
    IEEE80211_OFDM24,
    IEEE80211_NetworkTypeMax
} IEEE80211_NETWORK_TYPE;

/**
 * @brief ieee80211 network mode
 */
typedef enum {
    IEEE80211_IBSS,
    IEEE80211_Infrastructure,
    IEEE80211_AutoUnknown,
    IEEE80211_InfrastructureMax,
    IEEE80211_APMode
} IEEE80211_NETWORK_MODE;

/**
 * @brief ieee80211 authentication mode
 */
typedef enum {
    IEEE80211_AuthModeOpen,
    IEEE80211_AuthModeShared,
    IEEE80211_AuthModeWPA,
    IEEE80211_AuthModeWPAPSK,
    IEEE80211_AuthModeWPANone,
    IEEE80211_AuthModeWPA2,
    IEEE80211_AuthModeWPA2PSK,
    IEEE80211_AuthModeSAE,
    IEEE80211_AuthModeMax
} IEEE80211_AUTH_MODE;

/**
 * @brief ieee80211 encryption algorithm
 */
typedef enum {
    IEEE80211_ENC_NONE   = 0,
    IEEE80211_ENC_WEP40  = 1,
    IEEE80211_ENC_TKIP   = 2,
    IEEE80211_ENC_WRAP   = 3,
    IEEE80211_ENC_CCMP   = 4,
    IEEE80211_ENC_WEP104 = 5,
    IEEE80211_BIP_CMAC_128  = 6,
    IEEE80211_ENC_WAPI   = 14,
    IEEE80211_ENC_MAX
} IEEE80211_ENC_ALGO;

/**
 * @brief ieee80211 key type
 */
typedef enum{
    KEY_TYPE_GROUP = 0,
    KEY_TYPE_PAIRWISE = 1,
    KEY_TYPE_IGTK = 2
} IEEE80211_KEY_TYPE;

/**
 * @brief ieee80211 event code
 */
typedef enum{
    IEEE80211_EVENT_NO_EVENT                = 1,
    IEEE80211_EVENT_REQUEST                 = 2,
    IEEE80211_EVENT_ASSOCIATION_IND         = 3,
    IEEE80211_EVENT_ASSOCIATION_RSP         = 4,
    IEEE80211_EVENT_AUTHENTICATION_IND      = 5,
    IEEE80211_EVENT_REAUTHENTICATION_IND    = 6,
    IEEE80211_EVENT_DEAUTHENTICATION_IND    = 7,
    IEEE80211_EVENT_DISASSOCIATION_IND      = 8,
    IEEE80211_EVENT_DISCONNECT_REQ          = 9,
    IEEE80211_EVENT_SET_802DOT11            = 10,
    IEEE80211_EVENT_SET_KEY                 = 11,
    IEEE80211_EVENT_SET_PORT                = 12,
    IEEE80211_EVENT_DELETE_KEY              = 13,
    IEEE80211_EVENT_SET_RSNIE               = 14,
    IEEE80211_EVENT_GKEY_TSC                = 15,
    IEEE80211_EVENT_MIC_FAILURE             = 16,
    IEEE80211_EVENT_ASSOCIATION_INFO        = 17,
    IEEE80211_EVENT_INIT_QUEUE              = 18,
    IEEE80211_EVENT_EAPOLSTART              = 19,

    IEEE80211_EVENT_ACC_SET_EXPIREDTIME     = 31,
    IEEE80211_EVENT_ACC_QUERY_STATS         = 32,
    IEEE80211_EVENT_ACC_QUERY_STATS_ALL     = 33,
    IEEE80211_EVENT_REASSOCIATION_IND       = 34,
    IEEE80211_EVENT_REASSOCIATION_RSP       = 35,
    IEEE80211_EVENT_STA_QUERY_BSSID         = 36,
    IEEE80211_EVENT_STA_QUERY_SSID          = 37,

    IEEE80211_EVENT_EAP_PACKET              = 41,
    IEEE80211_EVENT_EAPOLSTART_PREAUTH      = 45,
    IEEE80211_EVENT_EAP_PACKET_PREAUTH      = 46,
    IEEE80211_EVENT_WPA2_MULTICAST_CIPHER   = 47,
    IEEE80211_EVENT_WPA_MULTICAST_CIPHER    = 48,

    IEEE80211_EVENT_MAX
} IEEE80211_EVENT;

/**
 * @brief ieee80211 virtual carrier sense status
 */
typedef enum {
    DISABLE_VCS,
    ENABLE_VCS,
    AUTO_VCS
} VIRTUAL_CARRIER_SENSE;

/**
 * @brief ieee80211 virtual carrier sense type
 */
typedef enum {
    NONE_VCS,
    RTS_CTS,
    CTS_TO_SELF
} VCS_TYPE;


typedef uint64_t IEEE80211_KEY_RSC;

/**
 * @brief ieee80211 SSID structure
 */
struct ieee80211_ssid {
    uint32_t SsidLength;
    uint8_t Ssid[33];
};

/**
 * @brief ieee80211 management fixed fields
 */
struct ieee80211_fixed_fields {
    uint8_t Timestamp[8];
    uint16_t BeaconInterval;
    uint16_t Capabilities;
};

/**
 * @brief ieee80211 IE header structure
 */
struct ieee80211_ie_hdr {
    uint8_t ElementID;
    uint8_t Length;
    uint8_t data[1];
};

/**
 * @brief ieee80211 key structure
 */
struct ieee80211_key {
    uint32_t Length;
    uint32_t KeyIndex;
    uint32_t KeyLength;
    uint8_t BSSID[6];
    IEEE80211_KEY_RSC KeyRSC;
    uint8_t KeyMaterial[32];
};

/**
 * @brief ieee80211 remove key structure
 */
struct ieee80211_remove_key {
    uint32_t Length;
    uint32_t KeyIndex;
    uint8_t BSSID[6];
};

/**
 * @brief ieee80211 WEP key structure
 */
struct ieee80211_wep {
    uint32_t KeyIndex;
    uint32_t KeyLength;
    uint8_t KeyMaterial[16];
};

/**
 * @brief set key request structure
 */
struct set_key_request {
    uint8_t EventId;
    uint8_t MoreEvent;
    uint32_t KeyIndex;
    uint32_t KeyLen;
    uint8_t KeyType;
    uint8_t EncType;
    uint8_t MACAddr[6];
    uint8_t KeyRSC[8];
    uint8_t KeyMaterial[64];
};

/**
 * @brief delete key request structure
 */
struct delete_key_request {
    uint8_t EventId;
    uint8_t MoreEvent;
    uint8_t MACAddr[6];
    uint32_t KeyIndex;
    uint8_t KeyType;
};

/**
 * @brief key request id structure
 */
struct request_id {
    uint8_t EventId;
};

/**
 * @brief ieee80211 configuration structure
 */
struct ieee80211_configuration {
    uint16_t beacon_period;
    uint16_t ATIM_window;
    uint16_t channel_config;
};

/**
 * @brief wlan PHY information structure
 */
struct wlan_phy_info {
    uint8_t SignalStrength;
    uint8_t SignalQuality;
};

/**
 * @brief ieee80211 BSS information structure which represent a BSS
 */
struct ieee80211_bss_info {
    uint16_t Length;
    uint8_t  MacAddress[6];
    struct ieee80211_ssid Ssid;
    int32_t  Rssi;
    struct ieee80211_configuration  Configuration;
    IEEE80211_NETWORK_TYPE  network_type;
    IEEE80211_NETWORK_MODE  network_mode;
    //uint8_t ieee80211_rates[IEEE80211_LENGTH_RATES_EX];
    uint16_t ieee80211_rates_map;
    uint16_t mcs_rate;
    //struct wlan_phy_info phy_info;

    uint8_t rsne_error;
    uint8_t Privacy;
    uint8_t is_8021x;
    uint8_t encryp_protocol;
    uint8_t privacy_algorithm;
    uint8_t group_cipher;
    uint8_t pairwise_cipher;
    uint16_t akm_suite;
    uint16_t rsn_cap;

    uint8_t ht_cap;
    uint8_t bw_40MHz;
    uint8_t bss_20_40_coex_supp;
    uint8_t short_GI;
    uint8_t beacon_flag;

    uint8_t hidden_ssid;
    uint8_t erp_protection;

#ifdef CONFIG_80211AC_SUPPORT
    uint16_t vht_cap;
    uint8_t bw_160MHz;
    uint8_t vht_highest_rate;
    uint16_t vht_data_rate;
#endif

    uint32_t IELength;
    uint8_t *IEs;  // it must be the last one of the struct
};

/**
 * @brief This function get size of given BSS information
 * @param bss pointer to the BSS information
 * @return the size
 */
__inline static uint16_t get_bss_info_size(struct ieee80211_bss_info *bss)
{
    return (uint16_t)(sizeof(struct ieee80211_bss_info) + bss->IELength);
}

/**
 * @brief This function get BSSID of given BSS information
 * @param bss pointer to the BSS information
 * @return the BSSID
 */
__inline static uint8_t *get_my_bssid(struct ieee80211_bss_info *bss)
{
    return (bss->MacAddress);
}

/**
 * @brief ieee80211 network structure which includes BSS information
 */
struct wlan_network {
    _list list;
    uint8_t wireless_mode;
    uint8_t fixed;
    int8_t join_res;
    uint8_t aid;
    uint32_t last_scanned;
    uint8_t ap_vendor;
    uint8_t ap_manufacturer;
    struct ieee80211_bss_info bss_info;  // it must be the last one of the struct
};

/**
 * @brief ethernet header format
 */
struct ethhdr {
    uint8_t h_dest[ETH_ALEN];
    uint8_t h_source[ETH_ALEN];
    uint16_t h_proto;
};

/**
 * @brief IP header format
 */
struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t  ihl:4;
    uint32_t version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
    uint32_t version:4;
    uint32_t ihl:4;
#else
#error "Please define BIG or LITTLE endian"
#endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

#define MAC_ARG(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define RND4(x)   (((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)
#define RND8(x)   (((x >> 3) + (((x & 7) == 0) ?  0: 1)) << 3)
#define RND128(x) (((x >> 7) + (((x & 127) == 0) ?  0: 1)) << 7)
#define RND256(x) (((x >> 8) + (((x & 255) == 0) ?  0: 1)) << 8)
#define RND512(x) (((x >> 9) + (((x & 511) == 0) ?  0: 1)) << 9)

/**
 * @brief This function transform the key from char to number
 * @param ch input char
 * @return the number
 */
__inline static uint8_t key_char2num(uint8_t ch)
{
    if((ch>='0')&&(ch<='9'))
        return ch - '0';
    else if ((ch>='a')&&(ch<='f'))
        return ch - 'a' + 10;
    else if ((ch>='A')&&(ch<='F'))
        return ch - 'A' + 10;
    else
        return 0xff;
}

/**
 * @brief This function transform the key from two char to number
 * @param hch input high char
 * @param lch input low char
 * @return the number
 */
__inline static uint8_t key_2char2num(uint8_t hch, uint8_t lch)
{
    return ((key_char2num(hch) << 4) | key_char2num(lch));
}

#endif
