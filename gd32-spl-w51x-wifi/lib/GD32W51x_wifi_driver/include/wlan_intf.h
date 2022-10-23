/*!
    \file    wlan_intf.h
    \brief   WLAN API for GD32W51x WiFi driver

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

#ifndef __WLAN_INTF_H_
#define __WLAN_INTF_H_

/*============================ INCLUDES ======================================*/
#include "wlan_intf_def.h"

/*============================ MACROS ========================================*/
#define DEFAULT_CHANNEL     1

/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/

typedef struct wlan_adapter ADAPTER, *PADAPTER;

typedef enum {
    WIFI_PS_MODE_NONE = 0,
    WIFI_PS_MODE_LPS_ONLY,
    WIFI_PS_MODE_LPS_AND_CPU_DEEPSLEEP,
} WIFI_PS_MODE_E;

/**
 * @brief WiFi indication interface structure which used to indicate information to upper layer
 */
struct wifi_indicate_intf {
    int (*indicate_disconnect)(struct wifi_disconnect_info *disc_info);
    int (*indicate_connect)(struct wifi_connect_result *connect_result);
    int (*indicate_scan_result)(WIFI_SCAN_RESULT_E *result);
    int (*indicate_softap_sta_del)(uint8_t *mac_addr);
};

/**
 * @brief WiFi operations entry structure which represents WiFi operation interface used by upper layer
 */
struct wifi_ops_entry {
    void *(*wifi_open_func)(uint8_t need_calibration);
    int32_t (*wifi_close_func)(void);
    int32_t (*wifi_scan_proc_func)(struct wifi_scan_req *scan_req_info);
    int32_t (*wifi_get_scan_list_func)(struct wifi_scan_info *scan_list, uint8_t *ap_num);
    int32_t (*wifi_connect_req_func)(struct wifi_ssid_config* connect_info);
    int32_t (*wifi_disconnect_req_func)(struct wifi_disconnect_req* disconnect_info);
    int32_t (*wifi_set_ipaddr_func)(uint8_t *ipaddr);
    int32_t (*wifi_set_mac_addr)(uint8_t *mac_addr);
    int32_t (*wifi_get_mac_addr)(uint8_t *mac_addr);
    int32_t (*wifi_priv_req_func)(struct wifi_priv_req_info *priv_info);
    int32_t (*wifi_set_idle_ps_delay)(uint32_t delay_time);
    int32_t (*wifi_set_ps_interval)(uint8_t listen_interval);
    int32_t (*wifi_set_channel_mask)(uint32_t ch_mask);
    int32_t (*wifi_set_channel)(uint8_t channel, uint8_t bw, uint8_t off);
    int32_t (*wifi_set_ps)(uint32_t ps_mode);
    int32_t (*wifi_get_ps)(uint32_t *ps_mode);
    int32_t (*wifi_get_bss_rssi)(void);
    int32_t (*wifi_softap_func)(struct wifi_ap_config *ap_config);
    int32_t (*wifi_show_trx_resource_func)(void);
    int32_t (*wifi_roaming_func)(void);
    int32_t (*wifi_set_promisc_mode)(uint32_t enable, void (*callback)(unsigned char*, unsigned short, signed char));
    int32_t (*wifi_set_promisc_filter)(uint8_t filter_type, uint8_t *filter_value);
    int32_t (*wifi_softap_get_assoc_info)(uint8_t *info);
    int32_t (*wifi_dump_driver_threads)(void);
    int32_t (*wifi_set_task_priority)(int32_t rx_prio, int32_t tx_prio, int32_t ev_prio, int32_t ps_prio);
    int32_t (*wifi_set_bw)(uint32_t bw);
    int32_t (*wifi_get_bw)(uint32_t *bw);
};

/*============================ GLOBAL VARIABLES ==============================*/
extern const struct wifi_ops_entry wifi_ops_entry;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
uint32_t wifi_indicate_intf_init(ADAPTER *Adapter);

void *wifi_open(uint8_t need_calibration);

int32_t wifi_close(void);

int32_t wifi_set_scan_entry(struct wifi_scan_req *scan_req_info);

int32_t wifi_get_scan_list_entry(struct wifi_scan_info *scan_list, uint8_t *ap_num);

int32_t wifi_connect_req_entry(struct wifi_ssid_config* connect_info);

int32_t wifi_disconnect_req_entry(struct wifi_disconnect_req* disconnect_info);

int32_t wifi_set_ipaddr_entry(uint8_t *ipaddr);

int32_t wifi_set_mac_addr(uint8_t *mac_addr);

int32_t wifi_get_heap_usage(struct wifi_used_heap_info *used_heap_info);

int32_t wifi_priv_req_entry(struct wifi_priv_req_info *priv_info);

int32_t wifi_set_idle_ps_delay(uint32_t delay_time);

int32_t wifi_set_ps_interval(uint8_t listen_interval);

int32_t wifi_set_channel_mask(uint32_t ch_mask);

int32_t wifi_set_ps(uint32_t ps_mode);

int32_t wifi_get_ps(uint32_t *ps_mode);

int32_t wifi_get_bss_rssi(void);

#ifdef CONFIG_SOFTAP_SUPPORT
int32_t wifi_softap_entry(struct wifi_ap_config *ap_conf);
#endif

void wifi_indicate_connect(ADAPTER *Adapter, WIFI_CONN_RESULT_E status);

void wifi_indicate_disconnect(ADAPTER *Adapter, WIFI_DISCON_REASON_E disc_reason);

void wifi_indicate_scan_complete(ADAPTER *Adapter);

int32_t wifi_set_bw(uint32_t bw);

int32_t wifi_get_bw(uint32_t *bw);

/* Import from WiFi Netlink */
extern int wifi_netlink_indicate_connect(struct wifi_connect_result *connect_result);

extern int wifi_netlink_scan_result_notify(WIFI_SCAN_RESULT_E *result);

extern int wifi_netlink_indicate_disconnect(struct wifi_disconnect_info *disc_info);

extern int wifi_netlink_indicate_softap_sta_del(uint8_t *mac_addr);

/*============================ IMPLEMENTATION ================================*/
static inline int is_zero_ether_addr(const uint8_t *a)
{
    return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

static inline int is_broadcast_ether_addr(const uint8_t *a)
{
    return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

#endif
