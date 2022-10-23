/*!
    \file    wifi_netlink.h
    \brief   WiFi netlink layer for GD32W51x WiFi SDK

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

#ifndef _WIFI_NETLINK_H
#define _WIFI_NETLINK_H

/*============================ INCLUDES ======================================*/
#include "app_cfg.h"
#include "wlan_intf_def.h"
#include "wlan_endian.h"
#include "wlan_80211.h"
#include "wrapper_os.h"

/*============================ MACROS ========================================*/
/*============================ MACRO FUNCTIONS ===============================*/

#define ROAMING_RETRY_LIMIT     10
#define DHCP_RETRY_LIMIT        3

#define IP_MODE_STATIC          1
#define IP_MODE_DHCP            0

/* Some usefull constants */
#define KILO    1e3
#define MEGA    1e6
#define GIGA    1e9

#undef TRUE
#define TRUE    1

#undef FALSE
#define FALSE   0

#define WIFI_SCAN_WAITING_TIME        5000
#define WIFI_CONNECT_WAITING_TIME     20000

#define WIFI_RX_TASK_PRORITY          (TASK_PRIO_WIFI_BASE)
#define WIFI_TX_TASK_PRORITY          (TASK_PRIO_WIFI_BASE + TASK_PRIO_HIGHER(1))
#define WIFI_WLAN_TASK_PRORITY        (TASK_PRIO_WIFI_BASE + TASK_PRIO_HIGHER(1))
#define WIFI_PS_TASK_PRORITY          (TASK_PRIO_WIFI_BASE + TASK_PRIO_HIGHER(2))

/*============================ TYPES =========================================*/
typedef enum {
    WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC = 1,
    WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL,
    WIFI_MESSAGE_INDICATE_CONN_SUCCESS,
    WIFI_MESSAGE_INDICATE_CONN_NO_AP,
    WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL,
    WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL,
    WIFI_MESSAGE_INDICATE_CONN_FAIL,
    WIFI_MESSAGE_INDICATE_DISCON_REKEY_FAIL,
    WIFI_MESSAGE_INDICATE_DISCON_MIC_FAIL,
    WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH,
    WIFI_MESSAGE_INDICATE_DISCON_NO_BEACON,
    WIFI_MESSAGE_INDICATE_DISCON_AP_CHANGED,
    WIFI_MESSAGE_INDICATE_DISCON_FROM_UI,
    WIFI_MESSAGE_INDICATE_DISCON_UNSPECIFIED,
    WIFI_MESSAGE_SHELL_COMMAND,
    WIFI_MESSAGE_TASK_TERMINATE,
    WIFI_MESSAGE_NUM
} WIFI_MESSAGE_TYPE_E;

typedef enum {
    WIFI_NETLINK_STATUS_NO_LINK = 0,
    WIFI_NETLINK_STATUS_NO_LINK_SCAN,
    WIFI_NETLINK_STATUS_LINKING,
    WIFI_NETLINK_STATUS_ROAMING,
    WIFI_NETLINK_STATUS_LINKED,
    WIFI_NETLINK_STATUS_LINKED_SCAN,
    WIFI_NETLINK_STATUS_LINKED_CONFIGED,
    WIFI_NETLINK_STATUS_NUM
} WIFI_NETLINK_STATUS_E;

#if 0
typedef struct wifi_message {
    WIFI_MESSAGE_TYPE_E msg_type;
    void *msg_data;
} WIFI_MESSAGE_T;
#endif

typedef struct wifi_netlink_info {
    uint8_t device_opened;
    uint8_t ap_started;
    WIFI_NETLINK_STATUS_E link_status;
    // WIFI_NETLINK_STATUS_E saved_link_status;

    uint8_t scan_ap_num;
    uint8_t valid_ap_num;
    uint8_t scan_list_ready;
    struct wifi_scan_info scan_list[SUPPORT_MAX_AP_NUM];
    struct wifi_scan_info *scan_list_head;

    struct wifi_ssid_config connect_info;
    struct wifi_connect_result connected_ap_info;
    WIFI_DISCON_REASON_E discon_reason;

    struct wifi_ap_config ap_conf;

    void (*promisc_callback)(unsigned char *, unsigned short, signed char);
    void (*promisc_mgmt_callback)(unsigned char*, int, signed char, int);
    uint32_t promisc_mgmt_filter;

    uint8_t scan_blocked;
    uint8_t conn_blocked;
    os_sema_t block_sema;
} WIFI_NETLINK_INFO_T;

/**
 * @brief factory function to iterate scan items retrieved from the scan result in the driver
 *
 * @param wifi_netlink pointer to the netlink shared resource
 * @param scan_item pointer to the scan item
 *
 */
typedef void (*iter_scan_item)(struct wifi_scan_info *scan_item);

/*============================ GLOBAL VARIABLES ==============================*/
#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN WIFI_NETLINK_INFO_T *p_wifi_netlink;

#undef EXTERN

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/**
 * @brief Allocate the resource for netlink layer
 *
 * @note Should be called only once at boot-up time, we don't provide
 * deinit function to free the resource.
 *
 * @return address to allocated memory, NULL pointer if there is an error
 */
WIFI_NETLINK_INFO_T *wifi_netlink_init(void);
int wifi_netlink_dev_open(void);
int wifi_netlink_dev_close(void);
int wifi_netlink_scan_set(void);
int wifi_netlink_scan_list_get(iter_scan_item iterator);
int wifi_netlink_connect_req(uint8_t *ssid, uint8_t *password);
int wifi_netlink_disconnect_req(void);
int wifi_netlink_status_get(void);
int wifi_netlink_ipaddr_set(uint8_t *ipaddr);
int wifi_netlink_ap_start(char *ssid, char *password, uint8_t channel, uint8_t hidden);
int wifi_netlink_channel_set(uint32_t channel, uint32_t bandwidth, uint32_t ch_offset);
int wifi_netlink_ps_set(int ps_mode);
int wifi_netlink_ps_get(void);
int wifi_netlink_bss_rssi_get(void);
int wifi_netlink_ap_channel_get(uint8_t *bssid);
int wifi_netlink_task_stack_get(void);
int wifi_netlink_link_state_get(void);
int wifi_netlink_linked_ap_info_get(uint8_t *ssid, uint8_t *passwd, uint8_t *bssid);
int wifi_netlink_raw_send(uint8_t *buf, uint32_t len);
int wifi_netlink_promisc_mode_set(uint32_t enable,
    void (*callback)(unsigned char*, unsigned short, signed char));
int wifi_netlink_promisc_mgmt_cb_set(uint32_t filter_mask,
    void (*callback)(unsigned char*, int, signed char, int));
int wifi_netlink_promisc_filter_set(uint8_t filter_type, uint8_t *filter_value);
int wifi_netlink_auto_conn_set(uint8_t auto_conn_enable);
uint8_t wifi_netlink_auto_conn_get(void);
int wifi_netlink_joined_ap_store(void);
int wifi_netlink_joined_ap_load(void);
int wifi_netlink_bw_set(uint32_t bw);
int wifi_netlink_bw_get(uint32_t *bw);


/*============================ IMPLEMENTATION ================================*/

#endif
