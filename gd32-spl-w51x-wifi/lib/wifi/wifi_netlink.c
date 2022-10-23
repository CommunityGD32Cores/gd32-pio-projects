/*!
    \file    wifi_netlink.c
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

#include <string.h>
#include <stdlib.h>

#undef MOUDLE
#define MOUDLE WIFI_NETLINK
#include "debug_print.h"

#include "app_cfg.h"
#include "uart.h"
#include "malloc.h"
#include "lwipopts.h"
#include "wifi_netif.h"

#define EXTERN
#include "wifi_netlink.h"

#include "wlan_intf_def.h"
#include "wlan_intf.h"
#include "wifi_management.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif

#ifdef PLATFORM_OS_RTTHREAD
#include <drivers/pm.h>
#endif

#ifdef CONFIG_WIFI_ROAMING_SUPPORT
static void *wifi_roaming_task_tcb = NULL;
static int32_t is_roaming = _FALSE;
static void terminate_roaming_task(void);
static int roaming_scan(struct wifi_scan_req *scan_req);
static void wifi_roaming_task_func(void *p_arg);
#endif

extern uint32_t softap_find_ipaddr_by_macaddr(uint8_t *mac_addr);

/*!
    \brief      initialize wifi netlink
    \param[in]  none
    \param[out] none
    \retval     wifi netlink information
*/
WIFI_NETLINK_INFO_T *wifi_netlink_init(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink;

    if (p_wifi_netlink)
        return p_wifi_netlink;

    wifi_netlink = (WIFI_NETLINK_INFO_T *)sys_malloc(sizeof(WIFI_NETLINK_INFO_T));

    if (NULL == wifi_netlink) {
        DEBUG_ERROR("wifi netlink: alloc wifi netlink failed\r\n");
    }

    sys_memset(wifi_netlink, 0, sizeof(WIFI_NETLINK_INFO_T));

    p_wifi_netlink = wifi_netlink;

    wifi_ops_entry.wifi_set_task_priority(WIFI_RX_TASK_PRORITY,
                                            WIFI_TX_TASK_PRORITY,
                                            WIFI_WLAN_TASK_PRORITY,
                                            WIFI_PS_TASK_PRORITY);
    sys_sema_init(&wifi_netlink->block_sema, 0);

    return wifi_netlink;
}

/*!
    \brief      get wifi netlink status
    \param[in]  none
    \param[out] none
    \retval     0: run success
*/
int wifi_netlink_status_get(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    if (!wifi_netlink->device_opened) {
        DEBUGPRINT("WIFI Status: Closed\r\n");
        DEBUGPRINT("==============================\r\n");
        DEBUGPRINT("\r\n");
        return 0;
    }

    if (wifi_netlink->ap_started) {
        uint8_t assoc_info[MAX_STATION_NUM * ETH_ALEN];
        uint32_t sta_ipaddr[MAX_STATION_NUM];
        uint32_t client_num, i;
        DEBUGPRINT("WIFI Status: AP Started\r\n");
        DEBUGPRINT("==============================\r\n");
        DEBUGPRINT("\rMODE:        AP\r\n");
        DEBUGPRINT("\rSSID:        %s\r\n", wifi_netlink->ap_conf.ssid);
        DEBUGPRINT("\rCHANNEL:     %d\r\n", wifi_netlink->ap_conf.channel);
        if (wifi_netlink->ap_conf.security == 3) {
            DEBUGPRINT("\rSECURITY:    WPA2\r\n");
            DEBUGPRINT("\rPASSWORD:    %s\r\n", wifi_netlink->ap_conf.password);
        } else if (wifi_netlink->ap_conf.security == 0) {
            DEBUGPRINT("\rSECURITY:    Open\r\n");
        }
        client_num = wifi_ops_entry.wifi_softap_get_assoc_info(assoc_info);
        for (i = 0; i < client_num; i++) {
            sta_ipaddr[i] = softap_find_ipaddr_by_macaddr(assoc_info + i * ETH_ALEN);
            DEBUGPRINT("CLIENT[%d]:   "MAC_FMT"   "IP_FMT"\r\n", i, MAC_ARG(assoc_info + i * ETH_ALEN), IP_ARG(sta_ipaddr[i]));
        }
    } else {
        if (wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
            DEBUGPRINT("WIFI Status: Connected\r\n");
            DEBUGPRINT("==============================\r\n");
            DEBUGPRINT("\rMODE:        STATION\r\n");
            DEBUGPRINT("\rSSID:        %s\r\n", wifi_netlink->connected_ap_info.ssid.ssid);
            DEBUGPRINT("\rCHANNEL:     %d\r\n", wifi_netlink->connected_ap_info.channel);
            switch (wifi_netlink->connected_ap_info.bw) {
            case CHANNEL_WIDTH_20:
                DEBUGPRINT("\rBW:          20M\r\n");
                break;
            case CHANNEL_WIDTH_40:
                DEBUGPRINT("\rBW:          40M\r\n");
                break;
            case CHANNEL_WIDTH_80:
                DEBUGPRINT("\rBW:          80M\r\n");
                break;
            case CHANNEL_WIDTH_160:
                DEBUGPRINT("\rBW:          160M\r\n");
                break;
            case CHANNEL_WIDTH_80_80:
                DEBUGPRINT("\rBW:          80+80M\r\n");
                break;
            default:
                DEBUGPRINT("\rBW:          UnKnown\r\n");
                break;
            }

            switch (wifi_netlink->connected_ap_info.wireless_mode) {
            case WIRELESS_11B:
                DEBUGPRINT("\rMODE:        B\r\n");
                break;
            case WIRELESS_11G:
            case WIRELESS_11BG:
            case WIRELESS_11ABG:
                DEBUGPRINT("\rMODE:        G\r\n");
                break;
            case WIRELESS_11_24N:
            case WIRELESS_11_5N:
            case WIRELESS_11GN:
            case WIRELESS_11AN:
            case WIRELESS_11BGN:
            case WIRELESS_11ABGN:
                DEBUGPRINT("\rMODE:        N\r\n");
                    break;
            case WIRELESS_11BGN_AC:
#ifndef CONFIG_80211AC_SUPPORT
                DEBUGPRINT("\rMODE:        N\r\n");
#else
                DEBUGPRINT("\rMODE:        AC\r\n");
#endif
                    break;
            case WIRELESS_11AC:
            case WIRELESS_11_AC_5G:
                DEBUGPRINT("\rMODE:        AC\r\n");
                break;
            default:
                DEBUGPRINT("\rMODE:        Unknown\r\n");
                break;
            }

            switch (wifi_netlink->connect_info.encryp_protocol) {
            case WIFI_ENCRYPT_PROTOCOL_OPENSYS:
                DEBUGPRINT("\rSECURITY:    Open   \r\n");
                break;
            case WIFI_ENCRYPT_PROTOCOL_WEP:
                DEBUGPRINT("\rSECURITY:    WEP   \r\n");
                break;
            case WIFI_ENCRYPT_PROTOCOL_WPA:
                DEBUGPRINT("\rSECURITY:    WPA   \r\n");
                break;
            case WIFI_ENCRYPT_PROTOCOL_WPA2:
                DEBUGPRINT("\rSECURITY:    WPA2   \r\n");
                break;
            case WIFI_ENCRYPT_PROTOCOL_WPA3_TRANSITION:
                DEBUGPRINT("\rSECURITY:    WPA2/WPA3\r\n");
                break;
            case WIFI_ENCRYPT_PROTOCOL_WPA3_ONLY:
                DEBUGPRINT("\rSECURITY:    WPA3\r\n");
                break;
            default:
                DEBUGPRINT("\rSECURITY:    Unknown   \r\n");
                break;
            }
            DEBUGPRINT("\rBSSID:       "MAC_FMT"\r\n", MAC_ARG(wifi_netlink->connected_ap_info.bssid_info.bssid));
            DEBUGPRINT("\rRSSI:        %d dbm\r\n", wifi_ops_entry.wifi_get_bss_rssi());

        } else {
            DEBUGPRINT("WIFI Status: Opened\r\n");
            DEBUGPRINT("\r\n==============================\r\n");
        }
    }

    DEBUGPRINT("\r\n");

    return 0;
}

/* same ssid ap info process */
/*!
    \brief      add ap list
    \param[in]  insert_node: pointer to the insert node
    \param[in]  scan_list_head: pointer to the scan list head
    \param[out] none
    \retval     0: replac node, 1: insert new node
*/
uint8_t ap_list_add(struct wifi_scan_info *insert_node, struct wifi_scan_info *scan_list_head)
{
    struct wifi_scan_info *p_scan_list = scan_list_head;

    while (p_scan_list != NULL) {
        if ((insert_node->ssid.ssid_len == p_scan_list->ssid.ssid_len) &&
            (!sys_memcmp(&insert_node->ssid.ssid, &p_scan_list->ssid.ssid, insert_node->ssid.ssid_len))) {
            if (insert_node->encryp_protocol != p_scan_list->encryp_protocol) {
                return 1;
            } else {
                if (insert_node->rssi > p_scan_list->rssi) {
                    // use insert_node ap info replacing the previous one except next_ptr
                    sys_memcpy(&p_scan_list->ssid, &insert_node->ssid, sizeof(struct wifi_scan_info) - FIELD_OFFSET(struct wifi_scan_info, ssid));
                }
                return 0;
            }
        }
        p_scan_list = p_scan_list->next_ptr;
    }

    return 1;
}

/*!
    \brief      compare AP RSSI
    \param[in]  insert_node: pointer to the insert node
    \param[in]  original_node: pointer to the original node
    \param[out] none
    \retval     0: insert node RSSI is lower, 1: insert node RSSI is higher
*/
uint8_t be_prior_to(struct wifi_scan_info *insert_node, struct wifi_scan_info *original_node)
{
/*
    if ((insert_node->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_OPENSYS) &&
        (original_node->encryp_protocol != WIFI_ENCRYPT_PROTOCOL_OPENSYS)) {
        return TRUE;
    }

    if ((insert_node->encryp_protocol != WIFI_ENCRYPT_PROTOCOL_OPENSYS) &&
        (original_node->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_OPENSYS)) {
        return FALSE;
    }
*/

    if (insert_node->rssi > original_node->rssi) {
        return 1;
    } else {
        return 0;
    }
}

/*!
    \brief      sorting scan list
    \param[in]  none
    \param[out] none
    \retval     none
*/
void scan_list_sorting(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_scan_info *p_scan_list, *p_next;
    struct wifi_scan_info *scan_list_head = NULL;
    struct wifi_scan_info *scan_list_node = wifi_netlink->scan_list;
    unsigned char i;

    wifi_netlink->valid_ap_num = 0;

    for (i = 0; i < wifi_netlink->scan_ap_num; i++) {
        if (scan_list_head == NULL) {
            scan_list_head = scan_list_node;
            wifi_netlink->valid_ap_num = 1;
        } else {
            if (ap_list_add(scan_list_node, scan_list_head)) {
                p_scan_list = scan_list_head;
                if (be_prior_to(scan_list_node, p_scan_list)) {
                    scan_list_node->next_ptr = p_scan_list;
                    scan_list_head = scan_list_node;
                } else {
                    while (p_scan_list->next_ptr != NULL) {
                        p_next = p_scan_list->next_ptr;
                        if (be_prior_to(scan_list_node, p_next)) {
                            scan_list_node->next_ptr = p_next;
                            p_scan_list->next_ptr = scan_list_node;
                            break;
                        }
                        p_scan_list = p_scan_list->next_ptr;
                    }
                    if (p_scan_list->next_ptr == NULL) {
                        p_scan_list->next_ptr = scan_list_node;
                        scan_list_node->next_ptr = NULL;
                    }
                }
                wifi_netlink->valid_ap_num++;
            }
        }
        scan_list_node++;
    }
    wifi_netlink->scan_list_head = scan_list_head;
}

/*!
    \brief      print bitrate
    \param[in]  bitrate: bitrate
    \param[out] none
    \retval     none
*/
void bitrate_print(unsigned long bitrate)
{
    double rate = bitrate;
    char scale;
    int divisor;

    if (rate >= GIGA) {
        scale = 'G';
        divisor = GIGA;
    } else {
        if (rate >= MEGA) {
            scale = 'M';
            divisor = MEGA;
        } else {
            scale = 'k';
            divisor = KILO;
        }
    }

    DEBUGPRINT("Rate: %g %cb/s\r\n", rate/divisor, scale);
}

/*!
    \brief      post netlink message
    \param[in]  receiver_tcb: pointer to the receiver task
    \param[in]  msg_type: message type refer to WIFI_MESSAGE_TYPE_E
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
static int netlink_message_post(void *receiver_tcb, WIFI_MESSAGE_TYPE_E msg_type)
{
    return sys_task_post(receiver_tcb, &msg_type, 0);
}

/*!
    \brief     set wifi netlink scan
    \param[in] none
    \param[out] none
    \retval      0 if succeeded, non-zero otherwise
*/
int wifi_netlink_scan_set(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_scan_req scan_req;
    uint8_t ret_val = 0;

#ifdef CONFIG_WIFI_ROAMING_SUPPORT
    if (wifi_netlink->link_status == WIFI_NETLINK_STATUS_ROAMING) {
        DEBUGPRINT("wifi netlink: Roaming is ongoing, stop scan.\r\n");
        return ret_val;
    }
#endif

    sys_memset(&scan_req.ssid.ssid, 0, WIFI_SSID_MAX_LEN);
    scan_req.ssid.ssid_len = 0;

    // wifi_netlink->saved_link_status = wifi_netlink->link_status;
    if (wifi_netlink->link_status == WIFI_NETLINK_STATUS_NO_LINK) {
        wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK_SCAN;
    } else if (wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
        wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED_SCAN;
    }

    wifi_netlink->scan_list_ready = 0;
    ret_val = wifi_ops_entry.wifi_scan_proc_func(&scan_req);

    return ret_val;
}

/*!
    \brief     get wifi netlink scan ist
    \param[in] iterator: pointer to the iter scan item function
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_scan_list_get(iter_scan_item iterator)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_scan_info *p_scan_list;

    wifi_netlink->scan_ap_num = 0;
    wifi_netlink->scan_list_head = NULL;
    sys_memset(wifi_netlink->scan_list, 0, sizeof(wifi_netlink->scan_list));

    wifi_ops_entry.wifi_get_scan_list_func(wifi_netlink->scan_list, &wifi_netlink->scan_ap_num);

    scan_list_sorting();

    if (iterator) {
        p_scan_list = wifi_netlink->scan_list_head;
        while (p_scan_list != NULL) {
            iterator(p_scan_list);
            p_scan_list = p_scan_list->next_ptr;
        }
    }

    //wifi_netlink->link_status = wifi_netlink->saved_link_status;

    DEBUG_INFO("wifi netlink: scan finished, scanned ap number: %d\r\n", wifi_netlink->valid_ap_num);
    return 0;
}

/*!
    \brief      wifi netlink connect request
    \param[in]  ssid: pointer to the SSID
    \param[in]  password: pointer to the password
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_connect_req(uint8_t *ssid, uint8_t *password)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_ssid_config *connect_info = &wifi_netlink->connect_info;
#if 0
    struct wifi_scan_info *p_scan_list = wifi_netlink->scan_list_head;
    unsigned char wep_key[13] = {0};
    uint8_t temp_buf[3] = {0};
    char *endptr = NULL;
    uint8_t  i;
    uint8_t found_ap = 0;
#endif

    if (ssid == NULL) {
        DEBUG_WARNING("wifi netlink: ssid empty!\r\n");
        return -1;
    }
    if (strlen((const char *)ssid) > WIFI_SSID_MAX_LEN) {
        DEBUG_WARNING("wifi netlink: ssid's length should not be greater than %d!\r\n", WIFI_SSID_MAX_LEN);
        return -1;
    }
    if (password &&
        ((strlen((const char *)password) > WPA_MAX_PSK_LEN) ||
        ((strlen((const char *)password) < WPA_MIN_PSK_LEN)))) {
        DEBUG_WARNING("wifi netlink: password's length should not be greater than %d or less than %d!\r\n",
                    WPA_MAX_PSK_LEN, WPA_MIN_PSK_LEN);
        return -1;
    }
    if (wifi_netlink->ap_started) {
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }
#if 0  /* Move to wifi interface layer. */
    for (i = 0; i < wifi_netlink->scan_ap_num; i++) {
        if ((p_scan_list->ssid.ssid_len == strlen((const char *)ssid)) &&
            (!sys_memcmp(p_scan_list->ssid.ssid, ssid, p_scan_list->ssid.ssid_len))) {
            found_ap = 1;
            break;

        } else {
            p_scan_list = p_scan_list->next_ptr;
        }
    }
    sys_memset(connect_info, 0, sizeof(*connect_info));
    if (found_ap) {
        sys_memcpy(&connect_info->ssid, &p_scan_list->ssid, sizeof(struct wifi_ssid));
        sys_memcpy(&connect_info->bssid_info, &p_scan_list->bssid_info, sizeof(struct wifi_bssid));
        connect_info->network_mode = p_scan_list->network_mode;
        connect_info->encryp_protocol = p_scan_list->encryp_protocol;
        connect_info->channel = p_scan_list->channel;
        connect_info->pairwise_cipher = p_scan_list->pairwise_cipher;
        connect_info->group_cipher = p_scan_list->group_cipher;
        if (password)
            sys_memcpy(connect_info->passwd, password, strlen((const char *)password) + 1);

        if (connect_info->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_OPENSYS) {
        } else if (connect_info->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_WPA ||
            connect_info->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_WPA2) {
            connect_info->credential.wpa_credential.credential_type = WIFI_WPA_CREDENTIAL_TYPE_PSK;
            if (password == NULL) {
                DEBUGPRINT("wifi netlink: please input the password when connect!\r\n");
                return -1;
            } else {
                sys_memcpy(connect_info->credential.wpa_credential.wpa_psk_info.psk, password, strlen((const char *)password) + 1);
            }
        } else if (connect_info->encryp_protocol == WIFI_ENCRYPT_PROTOCOL_WEP) {
            if (password == NULL) {
                DEBUGPRINT("wifi netlink: please input the password when connect!\r\n");
                return -1;
            }
            connect_info->credential.wep_credential.is_open_mode = 0;
            connect_info->credential.wep_credential.key_in_use = 0;  // [idx0 - idx3]

            if (strlen((const char *)password) == (WEP_64BITS_KEY_HEX_LEN << 1)) {
                connect_info->credential.wep_credential.key_type = WIFI_WEP_KEY_TYPE_64BITS;
                for (i=0; i < WEP_64BITS_KEY_HEX_LEN; i++) {
                    if ((password + i*2) != NULL) {
                        sys_memcpy(temp_buf, (password + i*2), 2);
                        wep_key[i] = (uint8_t)strtoul((const char *)temp_buf, &endptr, 16);
                    }
                }
                sys_memcpy(connect_info->credential.wep_credential.key.key_64bits_arr[0], wep_key, WEP_64BITS_KEY_HEX_LEN);
            } else if (strlen((const char *)password) == (WEP_128BITS_KEY_HEX_LEN << 1)) {
                connect_info->credential.wep_credential.key_type = WIFI_WEP_KEY_TYPE_128BITS;
                for (i=0; i < WEP_128BITS_KEY_HEX_LEN; i++) {
                    if ((password + i*2) != NULL) {
                        sys_memcpy(temp_buf, (password + i*2), 2);
                        wep_key[i] = (uint8_t)strtoul((const char *)temp_buf, &endptr, 16);
                    }
                }
                sys_memcpy(connect_info->credential.wep_credential.key.key_128bits_arr[0], wep_key, WEP_128BITS_KEY_HEX_LEN);
            } else {
                DEBUGPRINT("wifi netlink: WEP share key length should be %d or %d HEX!\r\n",
                            WEP_64BITS_KEY_HEX_LEN << 1, WEP_128BITS_KEY_HEX_LEN << 1);
                return -1;
            }
        } else {
            DEBUGPRINT("wifi netlink: not supported security mode!\r\n");
            return -1;
        }
    } else {
        sys_memcpy(connect_info->ssid.ssid, ssid, strlen((const char *)ssid) + 1);
        connect_info->ssid.ssid_len = strlen((const char *)ssid);
        if (password) {
            sys_memcpy(connect_info->passwd, password, strlen((const char *)password) + 1);
        }
    }
#else
    sys_memcpy(connect_info->ssid.ssid, ssid, strlen((const char *)ssid) + 1);
    connect_info->ssid.ssid_len = strlen((const char *)ssid);
    if (password) {
        sys_memcpy(connect_info->passwd, password, strlen((const char *)password) + 1);
    } else {
        sys_memset(connect_info->passwd, 0, 64);
    }
#endif
    wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKING;

    if (wifi_ops_entry.wifi_connect_req_func(connect_info) == _FALSE)
        return -1;

    return 0;
}

/*!
    \brief      wifi netlink disconnect request
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_disconnect_req(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_disconnect_req disconnect_info;

    if (wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
        sys_memcpy(&disconnect_info.ssid, &wifi_netlink->connected_ap_info.ssid, sizeof(struct wifi_ssid));

        wifi_ops_entry.wifi_disconnect_req_func(&disconnect_info);
#ifdef CONFIG_IPV6_SUPPORT
        wifi_netif_set_ip6addr_invalid();
#endif

        DEBUG_INFO("wifi netlink: disconnect with ap %s\r\n", &wifi_netlink->connected_ap_info.ssid.ssid);

        sys_memset(&wifi_netlink->connected_ap_info, 0, sizeof(struct wifi_connect_result));
        wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
    } else {
        DEBUG_INFO("wifi netlink: not connected!\r\n");
    }
#ifdef CONFIG_WIFI_ROAMING_SUPPORT
    terminate_roaming_task();
#endif
    return 0;
}

/*!
    \brief      open wifi netlink device
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_dev_open(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    if (!wifi_netlink->device_opened) {
        wifi_netif_open();

        wifi_netlink->device_opened = 1;
        sys_sema_init(&wifi_netlink->block_sema, 0);

        DEBUG_INFO("wifi netlink: device opened!\r\n");
    }

    return 0;
}

/*!
    \brief      close wifi netlink device
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_dev_close(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    if (wifi_netlink->device_opened) {
        wifi_netlink_disconnect_req();

        wifi_netif_close();
        wifi_netlink->device_opened = 0;
        if (wifi_netlink->block_sema)
            sys_sema_free(&wifi_netlink->block_sema);
        sys_memset(wifi_netlink, 0, sizeof(WIFI_NETLINK_INFO_T));
        DEBUG_INFO("wifi netlink: device closed!\r\n");
    }

    return 0;
}

/*!
    \brief      set wifi netlink ip addrress
    \param[in]  ipaddr: pointer to the ip addrress
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_ipaddr_set(uint8_t *ipaddr)
{
    wifi_ops_entry.wifi_set_ipaddr_func(ipaddr);

    DEBUG_INFO("wifi netlink: Got IP  "IP_FMT"\r\n", IP_ARG(*(uint32_t *)ipaddr));
#ifdef CONFIG_IPV6_SUPPORT
    ip6_addr_t *ip6_uniqe = (ip6_addr_t *)wifi_netif_get_ip6(1);
    DEBUG_INFO("wifi netlink: Got IP6 [%s]\r\n", ip6addr_ntoa(ip6_uniqe));
#endif

    p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED_CONFIGED;

    return 0;
}

/*!
    \brief      start wifi netlink ap
    \param[in]  ssid: pointer to the SSID
    \param[in]  password: pointer to the password
    \param[in]  channel: channel
    \param[in]  hidden: whether hidden
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_ap_start(char *ssid, char *password, uint8_t channel, uint8_t hidden)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    uint8_t result = 0;
    struct wifi_ap_config ap_conf;

    if (wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
        wifi_netlink_disconnect_req();
#ifdef CONFIG_WIFI_ROAMING_SUPPORT
    } else if (wifi_netlink->link_status == WIFI_NETLINK_STATUS_ROAMING) {
        terminate_roaming_task();
#endif
    }
    if (wifi_netlink->ap_started) {
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }

    if (wifi_netlink->device_opened == 0) {
        wifi_netlink_dev_open();
    }

    if ((NULL == ssid) || (strlen(ssid) > WIFI_SSID_MAX_LEN))
        return -1;
    sys_memcpy(ap_conf.ssid, ssid, (strlen(ssid) + 1));

    if (password)
        sys_memcpy(ap_conf.password, password, (strlen(password) + 1));
    else
        sys_memset(ap_conf.password, 0, sizeof(ap_conf.password));
    ap_conf.channel = channel;
    ap_conf.hidden = hidden;
    if (NULL != password) {
        ap_conf.security = WIFI_ENCRYPT_PROTOCOL_WPA2;
    } else {
        ap_conf.security = WIFI_ENCRYPT_PROTOCOL_OPENSYS;
    }

    // softap security mode: WPA2-AES only
    DEBUG_INFO("wifi netlink: starting softap ...\r\n");
    if (wifi_ops_entry.wifi_softap_func) {
        result = wifi_ops_entry.wifi_softap_func(&ap_conf);
    } else {
        DEBUG_WARNING("wifi netlink: Not support softap mode now.\r\n");
        DEBUG_WARNING("wifi netlink: Please define CONFIG_SOFTAP_SUPPORT in wlan_cfg.h and rebuild wlan library.\r\n");
        return 0;
    }

    if (result == 1) {
        // update wifi netlink structure for getting status
        wifi_netlink->ap_started = 1;
        sys_memcpy((uint8_t *)&wifi_netlink->ap_conf, &ap_conf, sizeof(struct wifi_ap_config));
        DEBUG_INFO("wifi netlink: softap %s started!\r\n", ssid);
    } else {
        DEBUG_INFO("wifi netlink: softap not started!\r\n");
    }

    return 0;
}

/*!
    \brief      set wifi netlink channel
    \param[in]  channel: channel
    \param[in]  bandwidth: band width
    \param[in]  ch_offset: chchannel offset
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_channel_set(uint32_t channel, uint32_t bandwidth, uint32_t ch_offset)
{
    if (!(channel > 0 && channel <= 14) &&
        !((channel == 36) || (channel == 40) || (channel == 44) || (channel == 48)) &&
        !((channel == 149) || (channel == 153) || (channel == 157) || (channel == 161))) {
        DEBUG_ERROR("wifi netlink: set channel, invalid channel!\r\n");
        return -1;
    }

    if (bandwidth != CHANNEL_WIDTH_20 && bandwidth != CHANNEL_WIDTH_40) {
        DEBUG_ERROR("wifi netlink: set channel, invalid bandwidth!\r\n");
        return -1;
    }

    if (ch_offset > HT_SEC_CHNL_OFFSET_BELOW) {
        DEBUG_ERROR("wifi netlink: set channel, invalid channel offset!\r\n");
        return -1;
    }

    if (bandwidth == CHANNEL_WIDTH_40 && ch_offset == HT_SEC_CHNL_OFFSET_NONE) {
        if ((channel > 9 && channel <= 13) || (channel == 48) || (channel == 161)) {
            ch_offset = HT_SEC_CHNL_OFFSET_BELOW;
        } else {
            ch_offset = HT_SEC_CHNL_OFFSET_ABOVE;
        }
    }

    wifi_ops_entry.wifi_set_channel(channel, bandwidth, ch_offset);
    DEBUG_INFO("set primary channel to %d, bandwidth is %d, channel offset is %d\r\n",
                channel, bandwidth, ch_offset);

    return 0;
}

/*!
    \brief      set wifi power mode
    \param[in]  ps_enable: 0 is disable, 1 is normal power saveing mode, 2 is deep sleep mode
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_ps_set(int ps_mode)
{
    wifi_ops_entry.wifi_set_ps(ps_mode);
#ifdef RT_USING_PM
    if (ps_mode == WIFI_PS_MODE_LPS_AND_CPU_DEEPSLEEP) {
        for (int i = PM_SLEEP_MODE_NONE; i < PM_SLEEP_MODE_DEEP; i++) {
            rt_pm_release_all(i);
        }
        rt_pm_request(PM_SLEEP_MODE_DEEP);
    } else {
        rt_pm_request(PM_SLEEP_MODE_NONE);
    }
#endif
    return 0;
}

/*!
    \brief      get wifi power save mode
    \param[in]  none
    \param[out] none
    \retval     the value of power save mode
*/
int wifi_netlink_ps_get(void)
{
    int ps_mode;

    wifi_ops_entry.wifi_get_ps((uint32_t*)&ps_mode);

    return ps_mode;
}

/*!
    \brief      get wifi netlink BSS RSSI
    \param[in]  none
    \param[out] none
    \retval     the RSSI value of BSS, 0 if get value failed
*/
int wifi_netlink_bss_rssi_get(void)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    if (wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
        wifi_netlink->connected_ap_info.rssi = wifi_ops_entry.wifi_get_bss_rssi();
        return wifi_netlink->connected_ap_info.rssi;
    }

    return 0;
}

/*!
    \brief      get wifi netlink ap channel
    \param[in]  bssid: pointer to the BSSID
    \param[out] none
    \retval     the channel of ap, 0 if get failed
*/
int wifi_netlink_ap_channel_get(uint8_t *bssid)
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    struct wifi_scan_info *p_scan_list = wifi_netlink->scan_list_head;

    while (p_scan_list != NULL) {
        if (sys_memcmp(p_scan_list->bssid_info.bssid, bssid, 6) == 0) {
            return p_scan_list->channel;
        }
        p_scan_list = p_scan_list->next_ptr;
    }

    return 0;
}

/*!
    \brief      get wifi netlink task stack
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_task_stack_get(void)
{
    wifi_ops_entry.wifi_dump_driver_threads();

    return 0;
}

/*!
    \brief      get wifi netlink link state
    \param[in]  none
    \param[out] none
    \retval     wifi netlink link state
*/
int wifi_netlink_link_state_get(void)
{
    return p_wifi_netlink->link_status;
}

/*!
    \brief      get wifi netlink linked ap information
    \param[in]  none
    \param[out] ssid: pointer to the SSID
    \param[out] passwd: pointer to the password
    \param[out] bssid: pointer to the BSSID
    \retval     0 if succeeded
*/
int wifi_netlink_linked_ap_info_get(uint8_t *ssid, uint8_t *passwd, uint8_t *bssid)
{
    if (ssid) {
        sys_memcpy(ssid, p_wifi_netlink->connected_ap_info.ssid.ssid, p_wifi_netlink->connected_ap_info.ssid.ssid_len);
    }
    if (passwd) {
        sys_memcpy(passwd, p_wifi_netlink->connect_info.passwd, sizeof(p_wifi_netlink->connect_info.passwd));
    }
    if (bssid) {
        sys_memcpy(bssid, p_wifi_netlink->connected_ap_info.bssid_info.bssid, ETH_ALEN);
    }

    return 0;
}

/*!
    \brief      send wifi netlink raw
    \param[in]  buf: pointer to the data
    \param[in]  len: send data length
    \param[out] none
    \retval     1 if succeed, 0 otherwise
*/
int wifi_netlink_raw_send(uint8_t *buf, uint32_t len)
{
    struct wifi_priv_req_info priv_req_info;

    priv_req_info.priv_req = WIFI_PRIV_TEST_TX;
    priv_req_info.priv_param1 = (uint32_t)buf;
    priv_req_info.priv_param2 = len;
    priv_req_info.priv_param3 = 1;
    priv_req_info.priv_param4 = 0;
    priv_req_info.result_data = NULL;
    return wifi_ops_entry.wifi_priv_req_func(&priv_req_info);
}

/*!
    \brief      wifi netlink promisc callback
    \param[in]  buf: pointer to the data
    \param[in]  len: data length
    \param[in]  rssi: RSSI
    \param[out] none
    \retval     none
*/
static void wifi_netlink_promisc_callback(unsigned char *buf, unsigned short len, signed char rssi)
{
    if (p_wifi_netlink->promisc_callback) {
        p_wifi_netlink->promisc_callback(buf, len, rssi);
    }

    if (p_wifi_netlink->promisc_mgmt_callback) {
        if ((GetFrameType(buf) == WIFI_MGT_TYPE) &&
            ((GetFrameSubType(buf) >> 5) & p_wifi_netlink->promisc_mgmt_filter)) {
            p_wifi_netlink->promisc_mgmt_callback(buf, (int)len, rssi, 0);
        }
    }
}

/*!
    \brief      set wifi netlink promisc mode
    \param[in]  enable: 1 is enable, 0 is disable
    \param[in]  callback: pointer to the callback function
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_promisc_mode_set(uint32_t enable,
    void (*callback)(unsigned char*, unsigned short, signed char))
{
    WIFI_NETLINK_INFO_T *wifi_netlink = p_wifi_netlink;
    if (enable) {
        wifi_netlink->promisc_callback = callback;
        wifi_ops_entry.wifi_set_promisc_mode(enable, wifi_netlink_promisc_callback);
    } else {
        wifi_ops_entry.wifi_set_promisc_mode(0, NULL);
        wifi_netlink->promisc_callback = NULL;
        wifi_netlink->promisc_mgmt_callback = NULL;
    }

    return 0;
}

/*!
    \brief      set wifi netlink promisc management callback
    \param[in]  filter_mask: filter mask
    \param[in]  callback: pointer to the callback function
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_promisc_mgmt_cb_set(uint32_t filter_mask,
    void (*callback)(unsigned char*, int, signed char, int))
{
    p_wifi_netlink->promisc_mgmt_callback = callback;
    p_wifi_netlink->promisc_mgmt_filter = filter_mask;

    return 0;
}

/*!
    \brief      set wifi netlink promisc filter
    \param[in]  filter_type: filter type
    \param[in]  filter_value: pointer to the filter value
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_promisc_filter_set(uint8_t filter_type, uint8_t *filter_value)
{
    wifi_ops_entry.wifi_set_promisc_filter(filter_type, filter_value);

    return 0;
}

/*!
    \brief      set wifi netlink auto connect
    \param[in]  auto_conn_enable: 1 is enable, 0 is disable
    \param[out] none
    \retval     function run state
*/
int wifi_netlink_auto_conn_set(uint8_t auto_conn_enable)
{
#ifdef USE_MBL_API
    return mbl_sys_status_set(SYS_AUTO_CONN, LEN_SYS_AUTO_CONN, &auto_conn_enable);
#else
    return 0;
#endif
}

/*!
    \brief      get wifi netlink auto connect
    \param[in]  none
    \param[out] none
    \retval     the state value of auto connect enable
*/
uint8_t wifi_netlink_auto_conn_get(void)
{
#ifdef USE_MBL_API
    uint8_t auto_conn_enable;
    int ret = mbl_sys_status_get(SYS_AUTO_CONN, LEN_SYS_AUTO_CONN, &auto_conn_enable);
    if (ret != SYS_STATUS_FOUND_OK)
        auto_conn_enable = 0;
    //DEBUG_INFO("Get auto connect = %d\r\n", auto_conn_enable);
    return auto_conn_enable;
#else
    return 0;
#endif
}

/*!
    \brief      store wifi netlink joined ap
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded, -1 is SSID length error, -2 is password length error
*/
int wifi_netlink_joined_ap_store(void)
{
#ifdef USE_MBL_API
    uint8_t *ssid = p_wifi_netlink->connected_ap_info.ssid.ssid;
    uint8_t *pwd = p_wifi_netlink->connect_info.passwd;
    ip_addr_t *ip = wifi_netif_get_ip();
    uint8_t buf[LEN_SYS_SSID_X];
    uint8_t ssid_len = strlen((char *)ssid);
    uint8_t pwd_len = strlen((char *)pwd);

    if (ssid_len <= LEN_SYS_SSID_X) {
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, ssid, ssid_len);
        mbl_sys_status_set(SYS_SSID_1, LEN_SYS_SSID_X, buf);
    } else if ((ssid_len > LEN_SYS_SSID_X) && (ssid_len < 2 * LEN_SYS_SSID_X)) {
        mbl_sys_status_set(SYS_SSID_1, LEN_SYS_SSID_X, (uint8_t *)ssid);
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, (ssid + LEN_SYS_SSID_X), (ssid_len - LEN_SYS_SSID_X));
        mbl_sys_status_set(SYS_SSID_2, LEN_SYS_SSID_X, buf);
    } else {
        return -1;
    }
    if (pwd_len <= LEN_SYS_PASSWD_X) {
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, pwd, pwd_len);
        mbl_sys_status_set(SYS_PASSWD_1, LEN_SYS_PASSWD_X, buf);
    } else if ((pwd_len > LEN_SYS_PASSWD_X) && (pwd_len <= 2 * LEN_SYS_PASSWD_X)) {
        mbl_sys_status_set(SYS_PASSWD_1, LEN_SYS_PASSWD_X, (uint8_t *)pwd);
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, (pwd + LEN_SYS_PASSWD_X), (pwd_len - LEN_SYS_PASSWD_X));
        mbl_sys_status_set(SYS_PASSWD_2, LEN_SYS_PASSWD_X, buf);
    } else if ((pwd_len > 2 * LEN_SYS_PASSWD_X) && (pwd_len <= 3 * LEN_SYS_PASSWD_X)) {
        mbl_sys_status_set(SYS_PASSWD_1, LEN_SYS_PASSWD_X, (uint8_t *)pwd);
        mbl_sys_status_set(SYS_PASSWD_2, LEN_SYS_PASSWD_X, (uint8_t *)(pwd + LEN_SYS_PASSWD_X));
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, (pwd + 2 * LEN_SYS_PASSWD_X), (pwd_len - 2 * LEN_SYS_PASSWD_X));
        mbl_sys_status_set(SYS_PASSWD_3, LEN_SYS_PASSWD_X, buf);
    } else if ((pwd_len > 3 * LEN_SYS_PASSWD_X) && (pwd_len <= 4 * LEN_SYS_PASSWD_X)){
        mbl_sys_status_set(SYS_PASSWD_1, LEN_SYS_PASSWD_X, (uint8_t *)pwd);
        mbl_sys_status_set(SYS_PASSWD_2, LEN_SYS_PASSWD_X, (uint8_t *)(pwd + LEN_SYS_PASSWD_X));
        mbl_sys_status_set(SYS_PASSWD_3, LEN_SYS_PASSWD_X, (uint8_t *)(pwd + 2 * LEN_SYS_PASSWD_X));
        sys_memset(buf, 0, sizeof(buf));
        sys_memcpy(buf, (pwd + 3 * LEN_SYS_PASSWD_X), (pwd_len - 3 * LEN_SYS_PASSWD_X));
        mbl_sys_status_set(SYS_PASSWD_4, LEN_SYS_PASSWD_X, buf);
    } else {
        return -2;
    }
#ifdef CONFIG_IPV6_SUPPORT
    if (ip->type == IPADDR_TYPE_V6)
        mbl_sys_status_set(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&(ip->u_addr.ip6.addr));
    else
        mbl_sys_status_set(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&(ip->u_addr.ip4.addr));
#else
    mbl_sys_status_set(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&(ip->addr));
#endif

    return 0;
#else
    return 0;
#endif
}

/*!
    \brief      load wifi netlink joined ap
    \param[in]  none
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_joined_ap_load(void)
{
#ifdef USE_MBL_API
    uint8_t buf[LEN_SYS_SSID_X];
    uint8_t *ssid_get = p_wifi_netlink->connect_info.ssid.ssid;
    uint8_t *pwd_get = p_wifi_netlink->connect_info.passwd;
    ip_addr_t ip_get;
    int ret;

    /* Get SSID */
    sys_memset(ssid_get, 0, sizeof(p_wifi_netlink->connect_info.ssid.ssid));
    ret = mbl_sys_status_get(SYS_SSID_1, LEN_SYS_SSID_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy(ssid_get, buf, LEN_SYS_SSID_X);
    } else {
        return -1;
    }
    ret = mbl_sys_status_get(SYS_SSID_2, LEN_SYS_SSID_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy((ssid_get + LEN_SYS_SSID_X), buf, LEN_SYS_SSID_X);
    }
    p_wifi_netlink->connect_info.ssid.ssid_len = strlen((char *)ssid_get);
    DEBUG_INFO("Get ssid = %s len= %d\r\n", ssid_get, p_wifi_netlink->connect_info.ssid.ssid_len);

    /* Get Password */
    sys_memset(pwd_get, 0, sizeof(p_wifi_netlink->connect_info.passwd));
    ret = mbl_sys_status_get(SYS_PASSWD_1, LEN_SYS_PASSWD_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy(pwd_get, buf, LEN_SYS_PASSWD_X);
    }
    ret = mbl_sys_status_get(SYS_PASSWD_2, LEN_SYS_PASSWD_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy((pwd_get + LEN_SYS_PASSWD_X), buf, LEN_SYS_PASSWD_X);
    }
    ret = mbl_sys_status_get(SYS_PASSWD_3, LEN_SYS_PASSWD_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy((pwd_get + 2 * LEN_SYS_PASSWD_X), buf, LEN_SYS_PASSWD_X);
    }
    ret = mbl_sys_status_get(SYS_PASSWD_4, LEN_SYS_PASSWD_X, buf);
    if (ret == SYS_STATUS_FOUND_OK) {
        sys_memcpy((pwd_get + 3 * LEN_SYS_PASSWD_X), buf, LEN_SYS_PASSWD_X);
    }
    DEBUG_INFO("Get pwd = %s\r\n", (char *)pwd_get);

    /* TODO: shorten dhcp time */
#ifdef CONFIG_IPV6_SUPPORT
    if (ip_get.type == IPADDR_TYPE_V6) {
        IP6_ADDR(&ip_get.u_addr.ip6, 0, 0, 0, 0);
        mbl_sys_status_get(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&ip_get.u_addr.ip6.addr);
        DEBUG_INFO("Get ipaddr = %s\r\n", ip6addr_ntoa(&ip_get.u_addr.ip6));
    } else {
        ip_get.u_addr.ip4.addr = 0;
        mbl_sys_status_get(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&ip_get.u_addr.ip4.addr);
        DEBUG_INFO("Get ipaddr = "IP_FMT"\r\n", IP_ARG(ip_get.u_addr.ip4.addr));
    }
#else
    ip_get.addr = 0;
    mbl_sys_status_get(SYS_IP_ADDR, LEN_SYS_IP_ADDR, (uint8_t *)&ip_get.addr);
    DEBUG_INFO("Get ipaddr = "IP_FMT"\r\n", IP_ARG(ip_get.addr));
#endif

    return 0;
#else
    return 0;
#endif
}

/*!
    \brief      notify wifi netlink scan result
    \param[in]  result: pointer to the WIFI_SCAN_RESULT_E struction
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_scan_result_notify(WIFI_SCAN_RESULT_E *result)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    WIFI_MESSAGE_TYPE_E message;

    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_NO_LINK_SCAN) {
        p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
    }
    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_LINKED_SCAN) {
        if (wifi_netif_is_ip_got()) {
            p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED_CONFIGED;
        } else {
            p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED;
        }
    }

    if (*result == WIFI_SCAN_RESULT_SUCC) {
        p_wifi_netlink->scan_list_ready = 1;
        message = WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC;
    } else if (*result == WIFI_SCAN_RESULT_FAIL) {
        message = WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL;
    } else {
        DEBUGPRINT("wifi netlink: unknown scan result\r\n");
        return -1;
    }
    return wifi_management_forward_netlink_message(message);

#if 0
    int ret = 0;
    WIFI_SCAN_RESULT_E scan_result = *result;
    wifi_management_event_t event;

    if (scan_result == WIFI_SCAN_RESULT_SUCC) {
        p_wifi_netlink->scan_list_ready = 1;
        event = WIFI_MGMT_EVENT_SCAN_DONE;
    } else if (scan_result == WIFI_SCAN_RESULT_FAIL) {
        event = WIFI_MGMT_EVENT_SCAN_FAIL;
    } else {
        DEBUGPRINT("wifi netlink: unknown scan result\r\n");
        return -1;
    }


    ret = eloop_event_send(event);
    if (ret != 0) {
        DEBUGPRINT("wifi netlink: notify scan result, os msg pool is empty, or task msg queue is full\r\n");
    }
    return ret;
#endif
#else
    WIFI_SCAN_RESULT_E scan_result = *result;
    WIFI_MESSAGE_TYPE_E msg_type;
    int ret = 0;

    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_NO_LINK_SCAN) {
        p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
    }
    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_LINKED_SCAN) {
        if (wifi_netif_is_ip_got()) {
            p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED_CONFIGED;
        } else {
            p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED;
        }
    }

    if (scan_result == WIFI_SCAN_RESULT_SUCC) {
        p_wifi_netlink->scan_list_ready = 1;
        msg_type = WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC;
    } else if (scan_result == WIFI_SCAN_RESULT_FAIL) {
        msg_type = WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL;
    } else {
        msg_type = WIFI_MESSAGE_NUM;
    }

#ifdef CONFIG_WIFI_ROAMING_SUPPORT
    if ((p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_ROAMING) && wifi_roaming_task_tcb) {
        ret = netlink_message_post(wifi_roaming_task_tcb, msg_type);
        if (ret != 0) {
            DEBUG_ERROR("wifi netlink: notify scan result, os msg pool is empty, or task msg queue is full\r\n");
        }
    } else {
        ret = netlink_message_post(&console_task_tcb, msg_type);
        if (ret != 0) {
            DEBUG_ERROR("wifi netlink: notify scan result, os msg pool is empty, or task msg queue is full\r\n");
        }
    }
#else
    ret = netlink_message_post(&console_task_tcb, msg_type);
    if (ret != 0) {
        DEBUG_ERROR("wifi netlink: notify scan result, os msg pool is empty, or task msg queue is full\r\n");
    }
#endif  /* CONFIG_WIFI_ROAMING_SUPPORT */
    return ret;
#endif  /* CONFIG_WIFI_MANAGEMENT_TASK */
}

/*!
    \brief      connect wifi netlink indicate
    \param[in]  connect_result: pointer to the wifi_connect_result struction
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_indicate_connect(struct wifi_connect_result *connect_result)
{
    WIFI_MESSAGE_TYPE_E msg_type = WIFI_MESSAGE_NUM;

    if (!p_wifi_netlink) {
        DEBUG_ERROR("wifi netlink: indicate connect error, wifi_netlink not initialized!\r\n");
        return -1;
    }

    DEBUG_INFO("wifi netlink: indicate connect, link_status is %d.\r\n", p_wifi_netlink->link_status);
    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_LINKING) {
        switch (connect_result->result) {
            case WIFI_CONN_RESULT_SUCC:
                sys_memcpy(&p_wifi_netlink->connected_ap_info, connect_result, sizeof(struct wifi_connect_result));
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED;
                msg_type = WIFI_MESSAGE_INDICATE_CONN_SUCCESS;
                DEBUG_INFO("wifi netlink: connected to ap: %s\r\n", &connect_result->ssid.ssid);
                break;

            case WIFI_CONN_RESULT_AP_NOT_FOUND:
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
                msg_type = WIFI_MESSAGE_INDICATE_CONN_NO_AP;
                DEBUG_INFO("wifi netlink: indicate connect failed, couldn't find AP!\r\n");
                break;

            case WIFI_CONN_RESULT_AUTH_FAIL:
            case WIFI_CONN_RESULT_ASSOC_FAIL:
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
                msg_type = WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL;
                DEBUG_INFO("wifi netlink: indicate connect failed, assoctiate failed!\r\n");
                break;

            case WIFI_CONN_RESULT_HANDSHAKE_FAIL:
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
                msg_type = WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL;
                DEBUG_INFO("wifi netlink: indicate connect failed, wpa/wpa2 handshake failed !\r\n");
                break;

            default :
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
                msg_type = WIFI_MESSAGE_INDICATE_CONN_FAIL;
                DEBUG_INFO("wifi netlink: indicate connect failed, connect failed!\r\n");
                break;
        }

#ifndef CONFIG_WIFI_MANAGEMENT_TASK
        if (netlink_message_post(&console_task_tcb, msg_type) != 0) {
            DEBUG_ERROR("wifi netlink: indicate connect, os msg pool is empty, or task msg queue is full\r\n");
            return -1;
        }
#ifdef CONFIG_WIFI_ROAMING_SUPPORT
    } else if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_ROAMING) {
        switch (connect_result->result) {
            case WIFI_CONN_RESULT_SUCC:
                sys_memcpy(&p_wifi_netlink->connected_ap_info, connect_result, sizeof(struct wifi_connect_result));
                msg_type = WIFI_MESSAGE_INDICATE_CONN_SUCCESS;
                DEBUG_INFO("wifi netlink: roaming connected to ap: %s\r\n", &connect_result->ssid.ssid);
                break;

            case WIFI_CONN_RESULT_AP_NOT_FOUND:
                msg_type = WIFI_MESSAGE_INDICATE_CONN_NO_AP;
                DEBUG_INFO("wifi netlink: roaming indicate connect failed, couldn't find AP!\r\n");
                break;

            case WIFI_CONN_RESULT_AUTH_FAIL:
            case WIFI_CONN_RESULT_ASSOC_FAIL:
                msg_type = WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL;
                DEBUG_INFO("wifi netlink: roaming indicate connect failed, assoctiate failed!\r\n");
                break;

            case WIFI_CONN_RESULT_HANDSHAKE_FAIL:
                msg_type = WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL;
                DEBUG_INFO("wifi netlink: roaming indicate connect failed, wpa/wpa2 handshake failed !\r\n");
                break;

            case WIFI_CONN_RESULT_UNSPECIFIED:
            default :
                msg_type = WIFI_MESSAGE_INDICATE_CONN_FAIL;
                DEBUG_INFO("wifi netlink: roaming indicate connect failed, connect failed!\r\n");
                break;
        }

        if (wifi_roaming_task_tcb) {
            if (netlink_message_post(wifi_roaming_task_tcb, msg_type) != 0) {
                DEBUG_INFO("wifi netlink: indicate connect, os msg pool is empty, or task msg queue is full\r\n");
                return -1;
            }
        }
#endif
#endif
    }

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    return wifi_management_forward_netlink_message(msg_type);
#else
    return 0;
#endif
}

/*!
    \brief      disconnect wifi netlink indicate
    \param[in]  disc_info: pointer to the wifi_disconnect_info struction
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_indicate_disconnect(struct wifi_disconnect_info *disc_info)
{
    WIFI_MESSAGE_TYPE_E msg_type;
    struct wifi_connect_result *ap_info = &(p_wifi_netlink->connected_ap_info);

    if (!p_wifi_netlink) {
        DEBUG_ERROR("wifi netlink: indicate disconnect error, wifi_netlink not initialized!\r\n");
        return -1;
    }

    p_wifi_netlink->discon_reason = disc_info->reason;
    switch (disc_info->reason) {
        case WIFI_DISCON_RECV_DEAUTH:
        case WIFI_DISCON_RECV_DISASSOC:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH;
            break;
        case WIFI_DISCON_NO_BEACON:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_NO_BEACON;
            break;
        case WIFI_DISCON_AP_CHANGED:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_AP_CHANGED;
            break;
        case WIFI_DISCON_REKEY_FAIL:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_REKEY_FAIL;
            break;
        case WIFI_DISCON_MIC_FAIL:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_MIC_FAIL;
            break;
        case WIFI_DISCON_FROM_UI:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_FROM_UI;
            break;
        default:
            msg_type = WIFI_MESSAGE_INDICATE_DISCON_UNSPECIFIED;
            break;
    }
    DEBUG_INFO("wifi netlink: disconnect reason %d, msg type %d\r\n", disc_info->reason, msg_type);

    switch (p_wifi_netlink->link_status) {
        case WIFI_NETLINK_STATUS_NO_LINK:
        case WIFI_NETLINK_STATUS_NO_LINK_SCAN:
            break;

        case WIFI_NETLINK_STATUS_LINKING:
            p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
            if (netlink_message_post(&console_task_tcb, msg_type) != 0) {
                DEBUG_ERROR("wifi netlink: indicate disconnect, os msg pool is empty, or task msg queue is full\r\n");
                return -1;
            }
#endif
            break;

#if defined(CONFIG_WIFI_ROAMING_SUPPORT) && !defined(CONFIG_WIFI_MANAGEMENT_TASK)
        case WIFI_NETLINK_STATUS_LINKED:
        case WIFI_NETLINK_STATUS_LINKED_SCAN:
        case WIFI_NETLINK_STATUS_LINKED_CONFIGED:
            if (disc_info->reason != WIFI_DISCON_FROM_UI) {
                if ((disc_info->ssid.ssid_len == ap_info->ssid.ssid_len) &&
                    (!sys_memcmp(&disc_info->ssid.ssid, ap_info->ssid.ssid, disc_info->ssid.ssid_len))) {
                    DEBUG_INFO("wifi netlink: start roaming...\r\n");
                    p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_ROAMING;
                    wifi_netif_stop_dhcp();
                    wifi_roaming_task_tcb = sys_task_create(NULL, (const uint8_t *)"wifi_roaming_task", NULL, WIFI_ROAMING_TASK_STK_SIZE,
                                    WIFI_ROAMING_QUEUE_SIZE, WIFI_ROAMING_TASK_PRIO, (task_func_t)wifi_roaming_task_func, NULL);
                } else {
                    DEBUG_INFO("wifi netlink: indicate disconnect, ssid not match, ignored!\r\n");
                    DEBUG_INFO("disconnect ssid %s len %d connected ssid %s len %d\r\n", &disc_info->ssid.ssid, disc_info->ssid.ssid_len,
                                ap_info->ssid.ssid, ap_info->ssid.ssid_len);
                }
            } else {
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
            }
            break;
        case WIFI_NETLINK_STATUS_ROAMING:
            DEBUG_INFO("wifi netlink: roaming state, receive message %d\r\n", msg_type);
            if (wifi_roaming_task_tcb) {
                if (netlink_message_post(wifi_roaming_task_tcb, msg_type) != 0) {
                    DEBUG_ERROR("wifi netlink: indicate disconnect, post netlink message failed.\r\n");
                    return -1;
                }
            }
            break;
#else
        case WIFI_NETLINK_STATUS_LINKED:
        case WIFI_NETLINK_STATUS_LINKED_SCAN:
        case WIFI_NETLINK_STATUS_LINKED_CONFIGED:
            if ((disc_info->ssid.ssid_len == ap_info->ssid.ssid_len) &&
                (!sys_memcmp(&disc_info->ssid.ssid, ap_info->ssid.ssid, disc_info->ssid.ssid_len))) {
                sys_memset(&p_wifi_netlink->connected_ap_info, 0, sizeof(struct wifi_connect_result));
                p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
                wifi_netif_stop_dhcp();
                DEBUG_INFO("wifi netlink: indicate disconnect, disconnected with ap: %s\r\n", &disc_info->ssid.ssid);
            } else {
                DEBUG_INFO("wifi netlink: indicate disconnect, ssid not match, ignored!\r\n");
                DEBUG_INFO("disconnect ssid %s len %d connected ssid %s len %d\r\n", &disc_info->ssid.ssid, disc_info->ssid.ssid_len,
                            ap_info->ssid.ssid, ap_info->ssid.ssid_len);
            }
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
            if (netlink_message_post(&console_task_tcb, msg_type) != 0) {
                DEBUG_INFO("wifi netlink: indicate disconnect, os msg pool is empty, or task msg queue is full\r\n");
                return -1;
            }
#endif
            break;
#endif
        default:
            break;
    }

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    return wifi_management_forward_netlink_message(msg_type);
#else
    return 0;
#endif
}

/*!
    \brief      get wifi bandwith
    \param[in]  none
    \param[out] bw : get supported bandwidth
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_bw_get(uint32_t *bw)
{
    return wifi_ops_entry.wifi_get_bw(bw);
}

/*!
    \brief      set wifi bandwith
    \param[in]  bw : set supported bandwidth
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
int wifi_netlink_bw_set(uint32_t bw)
{
    return wifi_ops_entry.wifi_set_bw(bw);
}


#ifdef CONFIG_WIFI_ROAMING_SUPPORT
/*!
    \brief      terminate roaming task
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void terminate_roaming_task(void)
{
    while (is_roaming == _TRUE) {
        if (wifi_roaming_task_tcb) {
            netlink_message_post(wifi_roaming_task_tcb, WIFI_MESSAGE_TASK_TERMINATE);
            DEBUG_INFO("wifi netlink: roaming task terminating...\r\n");
        }
        sys_ms_sleep(100);
    }
}
/*!
    \brief      roaming scan
    \param[in]  scan_req: pointer to the wifi_scan_req struction
    \param[out] none
    \retval     0 if succeeded, -1 otherwise
*/
static int roaming_scan(struct wifi_scan_req *scan_req)
{
    int waiting_terminated = 0;
    WIFI_MESSAGE_TYPE_E msg_type;

    if (wifi_ops_entry.wifi_scan_proc_func(scan_req) == 1) {
        do {
            if (sys_task_wait(WIFI_SCAN_WAITING_TIME, &msg_type) == OS_OK) {
                if (msg_type == WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC
                    || msg_type == WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL) {
                    if (waiting_terminated)
                        return -1;
                    else
                        break;
                } else if (msg_type == WIFI_MESSAGE_TASK_TERMINATE) {
                    waiting_terminated = 1;
                }
            } else { /* Waiting timeout */
                return -1;
            }
        } while (1);
    }
    return 0;
}

/*!
    \brief      wifi roaming task function
    \param[in]  p_arg: pointer to the argument
    \param[out] none
    \retval     none
*/
static void wifi_roaming_task_func(void *p_arg)
{
    struct wifi_ssid_config *connect_info = &(p_wifi_netlink->connect_info);
    WIFI_MESSAGE_TYPE_E msg_type;
    int retry_count = 0;
    int dhcp_retry = 0;
    int status = -1;
    struct wifi_disconnect_req disconnect_info;
    struct wifi_scan_req scan_req;

    is_roaming = _TRUE;
    DEBUG_INFO("wifi netlink: roaming ssid %s\r\n", connect_info->ssid.ssid);
    // DEBUGPRINT("wifi netlink: roaming password %s\r\n", (char *)connect_info->passwd);

    sys_memcpy(&scan_req.ssid.ssid, connect_info->ssid.ssid, connect_info->ssid.ssid_len);
    scan_req.ssid.ssid_len = connect_info->ssid.ssid_len;

    /* Force scanning since AP may change channel or other options. */
    if (p_wifi_netlink->discon_reason == WIFI_DISCON_AP_CHANGED
        || p_wifi_netlink->discon_reason == WIFI_DISCON_NO_BEACON
        || p_wifi_netlink->discon_reason == WIFI_DISCON_RECV_DEAUTH) {
        if (roaming_scan(&scan_req) == -1)
            goto Exit;
    }

    while ((retry_count < ROAMING_RETRY_LIMIT) && (status != 0)) {
        DEBUG_INFO("Connect Retry[%d]: last status is %d.\r\n", retry_count, status);
        status = -1;
        sys_task_msg_flush(NULL);

        /* Force scanning if the first linking retry failed. */
        if (retry_count > 1) {
            if (roaming_scan(&scan_req) == -1)
                goto Exit;
        }

        wifi_ops_entry.wifi_connect_req_func(connect_info);

        do {
            if (sys_task_wait(WIFI_CONNECT_WAITING_TIME, &msg_type) == OS_OK) {
                switch (msg_type) {
                case WIFI_MESSAGE_INDICATE_CONN_SUCCESS:
                    DEBUG_INFO("wifi netlink: roaming connect successfully! Start DHCP.\r\n");
                    while (dhcp_retry++ < DHCP_RETRY_LIMIT) {
                        wifi_netif_start_dhcp();
                        if (TRUE == wifi_netif_polling_dhcp()) {
                            wifi_netlink_ipaddr_set((uint8_t *)wifi_netif_get_ip());
                            DEBUG_INFO("wifi netlink: ip configured, IP: %s\r\n", ip_ntoa(wifi_netif_get_ip()));
                            status = 0;
                            break;
                        } else {
                            DEBUG_INFO("wifi netlink: get ip timeout!\r\n");
                            wifi_netif_stop_dhcp();
                        }
                    }
                    if (status != 0) {
                        sys_memcpy(&disconnect_info.ssid,  connect_info->ssid.ssid, sizeof(disconnect_info.ssid));
                        wifi_ops_entry.wifi_disconnect_req_func(&disconnect_info);
                        status = 7;
                    }
                    dhcp_retry = 0;
                    break;
                case WIFI_MESSAGE_INDICATE_CONN_NO_AP:
                case WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL:
                case WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL:
                case WIFI_MESSAGE_INDICATE_CONN_FAIL:
                    DEBUG_INFO("wifi netlink: roaming connect failed, event is %d!\r\n", msg_type);
                    status = 2;
                    break;
                case WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH:
                    DEBUG_INFO("wifi netlink: roaming connect failed, receive Deauth or Disassoc.\r\n");
                    status = 3;
                    break;
                case WIFI_MESSAGE_TASK_TERMINATE:
                case WIFI_MESSAGE_INDICATE_DISCON_FROM_UI:
                    DEBUG_INFO("wifi netlink: roaming task terminated.\r\n");
                    sys_memcpy(&disconnect_info.ssid,  connect_info->ssid.ssid, sizeof(disconnect_info.ssid));
                    wifi_ops_entry.wifi_disconnect_req_func(&disconnect_info);
                    retry_count = ROAMING_RETRY_LIMIT;
                    status = 4;
                    break;
                case WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC:
                case WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL:
                default:
                    break;
                }
            } else {
                DEBUG_INFO("wifi netlink: romaing connect timeout or error\r\n");
                status = 6;
                break;
            }
        } while (status < 0);
        retry_count++;
    }

Exit:
    if (status != 0) {
        sys_memset(&p_wifi_netlink->connected_ap_info, 0, sizeof(struct wifi_connect_result));
        p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
        wifi_netif_stop_dhcp();
        DEBUG_INFO("wifi netlink: roaming failed, status is %d.\r\n", status);
    } else {
        p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKED;
        DEBUG_INFO("wifi netlink: roaming succeeded.\r\n");
    }

    is_roaming = _FALSE;
    wifi_roaming_task_tcb = NULL;
    sys_task_delete(NULL);
}
#endif

/*!
    \brief      callbak function for softap sta delete
    \param[in]  mac_addr: the mac addr of delete sta
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_indicate_softap_sta_del(uint8_t *mac_addr)
{
#ifdef CONFIG_SOFTAP_CALLBACK_ENABLED
    uint32_t ip_addr;
    ip_addr = softap_find_ipaddr_by_macaddr(mac_addr);
    /* TODO */
    printf("this is callback for softap sta delete\r\n");
    printf("the mac addr of delete sta is "MAC_FMT"\r\n", MAC_ARG(mac_addr));
    printf("the ip addr of delete sta is "IP_FMT"\r\n", IP_ARG(ip_addr));
#endif
    return 0;
}

/*!
    \brief      callbak function for softap sta add
    \param[in]  mac_addr: the mac addr of add sta
    \param[in]  ip_addr: the ip addr(ipv4) of add sta
    \param[out] none
    \retval     0 if succeeded
*/
int wifi_netlink_indicate_softap_sta_add(uint8_t *mac_addr, uint32_t ip_addr)
{
#ifdef CONFIG_SOFTAP_CALLBACK_ENABLED
    /* TODO */
    printf("this is callback for softap sta add\r\n");
    printf("the mac addr of add sta is "MAC_FMT"\r\n", MAC_ARG(mac_addr));
    printf("the ip addr of add sta is "IP_FMT"\r\n", IP_ARG(ip_addr));
#endif
    return 0;
}