/*!
    \file    cmd_shell.c
    \brief   Command shell for GD32W51x WiFi SDK

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

/*============================ INCLUDES ======================================*/
#include <string.h>
#include <stdlib.h>

#include "app_cfg.h"
#include "app_type.h"
#include "bsp_inc.h"
#include "osal_types.h"
#include "wlan_debug.h"
#include "wrapper_os.h"
#include "debug_print.h"
#include "malloc.h"

#include "lwip/netifapi.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/stats.h"

#include "wifi_netlink.h"
#include "dhcpd.h"
#include "cmd_shell.h"
#include "wifi_netif.h"
#include "wifi_management.h"
#include "uart.h"
#include "console.h"

#ifdef CONFIG_TELNET_SERVER
#include "telnet_main.h"
#endif

#ifdef CONFIG_ALICLOUD_SUPPORT
#include "alicloud_entry.h"
#endif
#if defined(CONFIG_CONSOLE_ENABLE)
#ifdef CONFIG_RF_TEST_SUPPORT
#include "cmd_rf_test.h"
#endif
#ifdef CONFIG_INTERNAL_DEBUG
#include "cmd_inner_wifi.c"
#include "cmd_inner_misc.c"
#endif
#ifdef CONFIG_ATCMD
#include "atcmd.c"
#endif
#ifdef CONFIG_FATFS_SUPPORT
#include "fatfs.h"
#endif
/*============================ MACROS ========================================*/
/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static uint8_t exit_cmd_mode = 0;
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
static uint8_t ap_dhcpd_started = 0;
#endif

/*============================ PROTOTYPES ====================================*/
extern void cmd_ping(int argc, char **argv);
extern void cmd_tcp(int argc, char **argv);
extern void cmd_udp(int argc, char **argv);
extern err_t igmp_joingroup(const ip4_addr_t *ifaddr, const ip4_addr_t *groupaddr);
#ifdef CONFIG_IPERF_TEST
extern void cmd_iperf3(int argc, char **argv);
#endif
#ifdef CONFIG_SSL_TEST
extern void cmd_ssl_client(int argc, char **argv);
extern void cmd_ssl_selftest(int argc, char **argv);
#endif
static void cmd_help(int argc, char **argv);
extern void dump_mem_block_list(void);

/*============================ IMPLEMENTATION ================================*/
/*!
    \brief      open wifi
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_open(int argc, char **argv)
{
    if (!p_wifi_netlink->device_opened) {
        wifi_pmu_config();
        wifi_rcu_config();
        wifi_setting_config();
        wifi_netlink_dev_open();
    } else {
        DEBUGPRINT("wifi device had been opened!\r\n");
    }
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    if (eloop_event_send(ELOOP_EVENT_WAKEUP) != OS_OK) {
        wifi_management_start();
    }
#endif
}

/*!
    \brief      close wifi
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_close(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    eloop_event_send(ELOOP_EVENT_TERMINATE);
#endif
    if (p_wifi_netlink->device_opened) {
        wifi_netlink_dev_close();
        /* wlan related clock disable */
        wifi_clock_gate();
        /* set PMU to power off wlan */
        wifi_power_state_set(PMU_WIFI_SLEEP);
    } else {
        DEBUGPRINT("wifi device had been closed!\r\n");
    }
}

/*!
    \brief      show wifi scan information
    \param[in]  scan_item: the pointer of WiFi scan information used in getting scan result
    \param[out] none
    \retval     none
*/
static void scan_info_print(struct wifi_scan_info *scan_item)
{
    DEBUGPRINT("----------------------------------------------------\r\n");
    DEBUGPRINT("SSID:        %s\r\n", &scan_item->ssid.ssid);
    DEBUGPRINT("Channel:     %d\r\n", scan_item->channel);
    switch (scan_item->encryp_protocol) {
    case WIFI_ENCRYPT_PROTOCOL_OPENSYS:
        DEBUGPRINT("Security:    Open\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WEP:
        DEBUGPRINT("Security:    WEP\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA:
        DEBUGPRINT("Security:    WPA\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA2:
        DEBUGPRINT("Security:    WPA2\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA3_TRANSITION:
        DEBUGPRINT("Security:    WPA2/WPA3\r\n");
        break;
    case WIFI_ENCRYPT_PROTOCOL_WPA3_ONLY:
        DEBUGPRINT("Security:    WPA3\r\n");
        break;
    default:
        DEBUGPRINT("Security:    Unknown\r\n");
        break;
    }

    if (scan_item->network_mode == WIFI_NETWORK_MODE_INFRA) {
        DEBUGPRINT("Network:     Infrastructure\r\n");
    } else if (scan_item->network_mode == WIFI_NETWORK_MODE_ADHOC) {
        DEBUGPRINT("Network:     Adhoc\r\n");
    } else {
        DEBUGPRINT("Network:     Unknown\r\n");
    }

    // bitrate_print(scan_item->rate);
    DEBUGPRINT("Rate:        %d Mbps\r\n", scan_item->rate);
    DEBUGPRINT("RSSI:        %d dbm\r\n", scan_item->rssi);
    DEBUGPRINT("BSSID:       "MAC_FMT"\r\n", MAC_ARG(scan_item->bssid_info.bssid));

    DEBUGPRINT("\r\n");
}

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
/*!
    \brief      callback function after scan done
    \param[in]  eloop_data: the pointer of related data after the loop has stopped
    \param[in]  user_ctx: the pointer of user parameter
    \param[out] none
    \retval     none
*/
void cmd_cb_scan_done(void *eloop_data, void *user_ctx)
{
    DEBUGPRINT("[Scanned AP list]\r\n");

    wifi_netlink_scan_list_get(scan_info_print);

    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
}

/*!
    \brief      callback function after scan failed
    \param[in]  eloop_data: the pointer of related data after the loop has stopped
    \param[in]  user_ctx: the pointer of user parameter
    \param[out] none
    \retval     none
*/
void cmd_cb_scan_fail(void *eloop_data, void *user_ctx)
{
    DEBUGPRINT("WIFI_SCAN: failed\r\n");
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
}
#endif

/*!
    \brief      start wifi scan and show scan result
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_scan(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_DONE, cmd_cb_scan_done, NULL, NULL);
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_FAIL, cmd_cb_scan_fail, NULL, NULL);

    if (wifi_management_scan(FALSE) != 0) {
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
        DEBUGPRINT("start wifi_scan failed\r\n");
    }
#else
    WIFI_MESSAGE_TYPE_E msg_type;
    int result = 0;

    sys_task_msg_flush(NULL);
    if (wifi_netlink_scan_set() != 1) {
        DEBUGPRINT("\rWIFI_SCAN: failed!\r\n");
        return;
    }

    do {
        result = sys_task_wait(WIFI_SCAN_WAITING_TIME, &msg_type);
        if (result == OS_OK) {
            if (msg_type == WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC) {
                DEBUGPRINT("[Scanned AP list]\r\n");
                wifi_netlink_scan_list_get(scan_info_print);
                break;
            } else if (msg_type == WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL) {
                break;
            } else {
                /* shell command or other messages, do nothing */
            }
        } else { /* Waiting timeout */
            break;
        }
    } while (1);
#endif  /* CONFIG_WIFI_MANAGEMENT_TASK */
}

/*!
    \brief      set wifi ip address
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ip_set(int argc, char **argv)
{
    ip4_addr_t ip, netmask, gw;

    if (argc > 4 || argc < 2) {
        DEBUGPRINT("wifi_set_ip: invalid input \r\n");
        goto usage;
    }
    if (argc == 2) {
        if (strcmp(argv[1], "dhcp") == 0) {
            wifi_netif_set_ip_mode(IP_MODE_DHCP);
            return;
        } else {
            goto usage;
        }
    } else if (argc == 4) {
        ip4addr_aton((const char *)argv[1], &ip);
        ip4addr_aton((const char *)argv[2], &gw);
        ip4addr_aton((const char *)argv[3], &netmask);
        DEBUGPRINT("wifi_set_ip: set ip addr:%s, gate_way:%s, net_mask:%s \r\n", (char *)argv[1], (char *)argv[2],(char *)argv[3]);
        wifi_netif_set_ip(&ip, &netmask, &gw);
        wifi_netif_set_ip_mode(IP_MODE_STATIC);
        return;
    } else {
        goto usage;
    }

usage:
    DEBUGPRINT("Usage: wifi_set_ip dhcp |<ip_addr> <gate_way> <net_mask>\r\n");
    DEBUGPRINT("\tdhcp: get ip from dhcp\r\n");
    DEBUGPRINT("\tip_addr: ipv4 addr needded to set. eg: 192.168.0.123\r\n");
    DEBUGPRINT("\tgate_way: eg: 192.168.0.1\r\n");
    DEBUGPRINT("\tnet_mask: eg: 255.255.255.0\r\n");
}

/*!
    \brief      wifi connect to ap
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_connect(int argc, char **argv)
{
    int status = 0;
    uint8_t *ssid;
    uint8_t *password;
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
    WIFI_MESSAGE_TYPE_E msg_type;
#endif

    if(argc == 2) {
        ssid = (uint8_t *)argv[1];
        password = NULL;
    } else if(argc == 3) {
        ssid = (uint8_t *)argv[1];
        password = (uint8_t *)argv[2];
    } else {
        DEBUGPRINT("\rUsage: wifi_connect SSID [PASSWORD]\r\n");
        return;
    }

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    status = wifi_management_connect(ssid, password, FALSE);
    if (status != 0)
        DEBUGPRINT("start wifi_connect failed\r\n");
#else
    if ((wifi_netlink_link_state_get() >= WIFI_NETLINK_STATUS_LINKED)
        || (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_ROAMING)) {
        wifi_netlink_disconnect_req();
        wifi_netif_stop_dhcp();
    }
    if (ap_dhcpd_started) {
        stop_dhcpd_daemon();
        ap_dhcpd_started = 0;
    }

    //connect req
    sys_task_msg_flush(NULL);
    status = wifi_netlink_connect_req(ssid, password);
    if (status != 0)
        return;

    do{
        if (sys_task_wait(WIFI_CONNECT_WAITING_TIME, &msg_type) == OS_OK) {
            switch (msg_type) {
            case WIFI_MESSAGE_INDICATE_CONN_SUCCESS:
                wifi_netif_start_dhcp();
                if (TRUE == wifi_netif_polling_dhcp()) {
                    wifi_netlink_ipaddr_set((uint8_t *)wifi_netif_get_ip());
                    DEBUGPRINT("WIFI_CONNECT: ip configured, IP: %s\r\n", ip_ntoa(wifi_netif_get_ip()));
                } else {
                    DEBUGPRINT("WIFI_CONNECT: get ip timeout!\r\n");
                    wifi_netif_stop_dhcp();
                    wifi_netlink_disconnect_req();
                }
                status = 1;
                break;
            case WIFI_MESSAGE_INDICATE_CONN_NO_AP:
            case WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL:
            case WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL:
            case WIFI_MESSAGE_INDICATE_CONN_FAIL:
                DEBUGPRINT("WIFI_CONNECT: connect failed, msg_type is %d!\r\n", msg_type);
                status = 1;
                break;
            case WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH:
                DEBUGPRINT("WIFI_CONNECT: connect failed, receive Deauth or Disassoc.\r\n");
                status = 1;
                break;
            case WIFI_MESSAGE_INDICATE_DISCON_AP_CHANGED:
                DEBUGPRINT("WIFI_CONNECT: connect failed, AP has changed, please scan again!\r\n");
                status = 1;
                break;
            case WIFI_MESSAGE_SHELL_COMMAND:
            default:
                break;
            }
        }else{
            DEBUGPRINT("WIFI_CONNECT: connect timeout or error\r\n");
            break;
        }
    } while(!status);
#endif
}

/*!
    \brief      wifi disconnect from ap
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_disconnect(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    if (wifi_management_disconnect() != 0)
        DEBUGPRINT("start wifi_disconnect failed\r\n");
#else
    //disconnect req
    wifi_netlink_disconnect_req();

    wifi_netif_stop_dhcp();
#endif
}

/*!
    \brief      show wifi status
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_status(int argc, char **argv)
{
    uint8_t *mac = wifi_netif_get_hwaddr();
    uint8_t *ip = (uint8_t *)wifi_netif_get_ip();
    uint8_t *gw = (uint8_t *)wifi_netif_get_gw();

    wifi_netlink_status_get();

    DEBUGPRINT("\rNetwork Interface Status\r\n");
    DEBUGPRINT("==============================\r\n");
    DEBUGPRINT("\rMAC:         [%02x:%02x:%02x:%02x:%02x:%02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    DEBUGPRINT("\rIP:          [%d.%d.%d.%d]\r\n", ip[0], ip[1], ip[2], ip[3]);
    DEBUGPRINT("\rGW:          [%d.%d.%d.%d]\r\n", gw[0], gw[1], gw[2], gw[3]);
#ifdef CONFIG_IPV6_SUPPORT
    if (!p_wifi_netlink->ap_started) {
        ip6_addr_t *ip6_local = (ip6_addr_t *)wifi_netif_get_ip6(0);
        ip6_addr_t *ip6_uniqe = (ip6_addr_t *)wifi_netif_get_ip6(1);
        DEBUGPRINT("\rIP6_local:   [%s]\r\n", ip6addr_ntoa(ip6_local));
        DEBUGPRINT("\rIP6_uniqe:   [%s]\r\n", ip6addr_ntoa(ip6_uniqe));
    }
#endif
}

/*!
    \brief      show wifi signal strength
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_rssi(int argc, char **argv)
{
    int rssi;

    rssi = wifi_netlink_bss_rssi_get();
    if (rssi < 0) {
        DEBUGPRINT("wifi rssi: current AP RSSI is %d dbm\r\n", rssi);
    } else {
        DEBUGPRINT("wifi rssi: wifi is not connected!\r\n");
    }
}

/*!
    \brief      set wifi channel
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_channel_set(int argc, char **argv)
{
    char *endptr = NULL;
    uint32_t channel = 0;
    uint32_t bandwidth = CHANNEL_WIDTH_20;
    uint32_t ch_offset = HT_SEC_CHNL_OFFSET_NONE;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc >= 4) {
        ch_offset = (uint32_t)strtoul((const char *)argv[3], &endptr, 10);
        if (*endptr != '\0' || !((ch_offset == HT_SEC_CHNL_OFFSET_ABOVE) || (ch_offset == HT_SEC_CHNL_OFFSET_BELOW))) {
            DEBUGPRINT("wifi_set_channel: invalid channel offset\r\n");
            goto usage;
        }
    }

    if (argc >= 3) {
        bandwidth = (uint32_t)strtoul((const char *)argv[2], &endptr, 10);
        if (*endptr != '\0') {
            DEBUGPRINT("wifi_set_channel: invalid channel bandwidth\r\n");
            goto usage;
        }
    }

    if (argc >= 2) {
        channel = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        if (*endptr == '\0') {
            if (bandwidth == 1) {
                if ((channel <= 4 && ch_offset == HT_SEC_CHNL_OFFSET_BELOW) || (channel <= 14 && channel >= 11 && ch_offset == HT_SEC_CHNL_OFFSET_ABOVE)) {
                    DEBUGPRINT("wifi_set_channel: channel 1-4 can't set 2nd channel below, channel 11-14 can't set 2nd channel above\r\n");
                    goto usage;
                }
            }
            wifi_netlink_channel_set(channel, bandwidth, ch_offset);
            return;
        } else {
            DEBUGPRINT("wifi_set_channel: invalid channel\r\n");
        }
    }
usage:
    DEBUGPRINT("Usage: wifi_set_channel <channel> [bandwidth] [offset]\r\n");
    DEBUGPRINT("\tchannel: 1 - 14\r\n");
    DEBUGPRINT("\tbandwidth: 0: 20M, 1: 40M, default 0\r\n");
    DEBUGPRINT("\toffset: 1: 2nd channel above, 3: 2nd channel below (only use for 40M, 20M will ignore)\r\n");
}

/*!
    \brief      set wifi mac address
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_mac_addr(int argc, char **argv)
{
    uint8_t macaddr[9];    /* reserve 3 more bytes to buffer the overflow */
    uint32_t num;

    if (argc < 2) {
        DEBUGPRINT("Device MAC address: "MAC_FMT"\r\n", MAC_ARG(wifi_netif_get_hwaddr()));
        return;
    }

    num = sscanf(argv[1], "%02x:%02x:%02x:%02x:%02x:%02x", (uint32_t *)&macaddr[0], (uint32_t *)&macaddr[1],
                    (uint32_t *)&macaddr[2], (uint32_t *)&macaddr[3], (uint32_t *)&macaddr[4], (uint32_t *)&macaddr[5]);
    if (num == 6) {
        if (wifi_netif_set_hwaddr(macaddr)) {
            DEBUGPRINT("Set device MAC address: "MAC_FMT"\r\n", MAC_ARG(macaddr));
        }
    } else {
        DEBUGPRINT("Usage: wifi_mac_addr <MAC address>\r\n");
        DEBUGPRINT("\tMAC address should be like 11:22:33:aa:bb:cc\r\n");
    }
}

/*!
    \brief      configure wifi power save mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ps(int argc, char **argv)
{
    char *endptr = NULL;
    uint32_t ps_enable = 1;

    if (argc >= 2) {
        ps_enable = (uint32_t)strtoul((const char *)argv[1], &endptr, 0);
        if (*endptr != '\0' || ps_enable > 2) {
            goto usage;
        }
    } else {
        DEBUGPRINT("Current ps mode: %d\r\n", wifi_netlink_ps_get());
        goto usage;
    }

    wifi_netlink_ps_set(ps_enable);
    if (ps_enable) {
        DEBUGPRINT("wifi_ps: power save enabled!\r\n");
    } else {
        DEBUGPRINT("wifi_ps: power save disabled!\r\n");
    }
    return;

usage:
    DEBUGPRINT("Usage: wifi_ps <0, 1 or 2>\r\n");
    DEBUGPRINT("\t0: Wifi not sleep and CPU not sleep\r\n");
    DEBUGPRINT("\t1: WiFi in sleep and CPU not sleep\r\n");
    DEBUGPRINT("\t2: WiFi in sleep and CPU in deep sleep\r\n");
}

/*!
    \brief      configure wifi as AP
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ap(int argc, char **argv)
{
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
#endif
    char *ssid = NULL;
    char *passwd = NULL;
    uint8_t channel = 0;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc == 4) {
        ssid = argv[1];
        passwd = argv[2];
        if (strlen(ssid) > WIFI_SSID_MAX_LEN
                || strlen(passwd) > WPA_MAX_PSK_LEN
                || strlen(passwd) < WPA_MIN_PSK_LEN)
            goto usage;
        channel = atoi(argv[3]) & 0xFF;
        if (channel > 13 || channel < 1)
            goto usage;
    } else {
        goto usage;
    }

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_ap_start(ssid, passwd, channel, 0);
#else
    wifi_netif_stop_dhcp();

    wifi_netlink_ap_start(ssid, passwd, channel, 0);

    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
#endif
    return;

usage:
    DEBUGPRINT("Usage: wifi_ap <SSID> <PASSWORD> <CHANNEL>\r\n");
    DEBUGPRINT("<SSID>: The length should be less than 32.\r\n");
    DEBUGPRINT("<PASSWORD>: The length should be between 8 and 63.\r\n");
    DEBUGPRINT("<CHANNEL>: 1~13.\r\n");
}

/*!
    \brief      advanced settings for WiFi in AP mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ap_adv(int argc, char **argv)
{

    char *ssid = NULL;
    char *passwd = NULL;
#ifndef CONFIG_WIFI_MANAGEMENT_TASK
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    int ssid_len = 0, psk_len = 0;
#endif
    uint8_t channel = 1, is_hidden = 0;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc == 2) {
        ssid = argv[1];
        if (strlen(ssid) > WIFI_SSID_MAX_LEN)
            goto usage;
    } else if (argc == 3) {
        ssid = argv[1];
        passwd = argv[2];
        if (strlen(ssid) > WIFI_SSID_MAX_LEN
                || strlen(passwd) > WPA_MAX_PSK_LEN
                || strlen(passwd) < WPA_MIN_PSK_LEN)
            goto usage;
    } else if (argc == 4) {
        ssid = argv[1];
        passwd = argv[2];
        channel = atoi(argv[3]);
        if (strlen(ssid) > WIFI_SSID_MAX_LEN
                || strlen(passwd) > WPA_MAX_PSK_LEN
                || strlen(passwd) < WPA_MIN_PSK_LEN
                || channel > 13 || channel < 1)
            goto usage;
    } else if (argc == 5) {
        ssid = argv[1];
        passwd = argv[2];
        channel = atoi(argv[3]);
        is_hidden = atoi(argv[4]);
        if (strlen(ssid) > WIFI_SSID_MAX_LEN
                || strlen(passwd) > WPA_MAX_PSK_LEN
                || strlen(passwd) < WPA_MIN_PSK_LEN
                || channel > 13 || channel < 1
                || (1 != is_hidden && 0 != is_hidden))
            goto usage;
    } else {
        goto usage;
    }
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_ap_start(ssid, passwd, channel, is_hidden);
#else
    wifi_netif_stop_dhcp();

    wifi_netlink_ap_start(ssid, passwd, channel, is_hidden);

    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
#endif
    return;
usage:
    DEBUGPRINT("Usage: wifi_ap_adv <SSID> [PASSWORD] [CHANNEL] [HIDDEN]\r\n");
    DEBUGPRINT("<SSID>: len <= 32\r\n");
    DEBUGPRINT("[PASSWORD]: len >= 8 && len <= 63\r\n");
    DEBUGPRINT("[CHANNEL]: 1~13\r\n");
    DEBUGPRINT("[HIDDEN]: 0 or 1\r\n");
}

/*!
    \brief      stop wifi ap mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_wifi_ap_stop(int argc, char **argv)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_sta_start();
#else
    if (p_wifi_netlink->ap_started){
        /* Stop DHCPD */
        if (ap_dhcpd_started) {
            stop_dhcpd_daemon();
            ap_dhcpd_started = 0;
        }
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }
#endif
}

/*!
    \brief      show memory usage
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void cmd_mem_status(int argc, char **argv)
{
    uint8_t i;

    DEBUGPRINT("\rHeap Information\r\n");
    DEBUGPRINT("==================================\r\n");
    DEBUGPRINT("\rheap status: total free heap size = %d\r\n", sys_free_heap_size());
    DEBUGPRINT("\rheap status: total min free heap size = %d\r\n", sys_min_free_heap_size());

    DEBUGPRINT("Task Stack Information\r\n");
    DEBUGPRINT("==================================\r\n");
    DEBUGPRINT("\ros idle task: free stack = %d\r\n", sys_stack_free_get(sys_idle_task_handle_get()));
    DEBUGPRINT("\ros timer task: free stack = %d\r\n", sys_stack_free_get(sys_timer_task_handle_get()));
    DEBUGPRINT("\rconsole task: free stack = %d\r\n", sys_stack_free_get(&console_task_tcb));
    for (i = 0; i < LWIP_TASK_MAX; i++) {
        DEBUGPRINT("\rlwip task %d: free stack = %d\r\n", i, sys_stack_free_get(&lwip_task_tcb[i]));
    }

    wifi_netlink_task_stack_get();
#ifdef CONFIG_GAGENT_TEST
    DEBUGPRINT("\rgagent test task: free stack = %d\r\n", sys_stack_free_get(&gagent_task_tcb));
#endif
    DEBUGPRINT("\r\n");

    DEBUGPRINT("\r\n\rMemory List\r\n");
    DEBUGPRINT("==================================\r\n");
    dump_mem_block_list();
}

#ifdef CONFIG_JOIN_GROUP_SUPPORT
static void join_group_task_func(void *param)
{
    ip4_addr_t group_ip;
    char *ip_addr = (char *)param;

    if (inet_aton(ip_addr, (struct in_addr*)&group_ip) == 0) {
        DEBUGPRINT("\rCan not join group because of group IP error\r\n");
        goto exit;
    }

    if (wifi_netif_is_ip_got()) {
        igmp_joingroup((const ip4_addr_t *)wifi_netif_get_ip(), (const ip4_addr_t *)&group_ip);
    } else {
        DEBUGPRINT("\rCan not join group because DHCP IP not got\r\n");
    }

exit:
    sys_task_delete(NULL);
}

/*!
    \brief      join group
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_group_join(int argc, char **argv)
{
    void *handle;

    if (argc == 2) {
        handle = sys_task_create(NULL, (const uint8_t *)"join_group_task", NULL,
                        JOIN_GROUP_STACK_SIZE, 0, JOIN_GROUP_TASK_PRIO,
                        (task_func_t)join_group_task_func, (void *)argv[1]);
        if (handle == NULL) {
            DEBUGPRINT("ERROR: create join group task failed.\r\n");
            return;
        }
    } else {
        DEBUGPRINT("\rUsage: join_group MulticastIP\r\n");
    }
}
#endif

/*!
    \brief      restart system
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_reboot(int argc, char **argv)
{
    if (p_wifi_netlink->device_opened) {
        /* clock: wlan disable, wlan run disable */
        wifi_clock_gate();
        /* set PMU to power off wlan */
        wifi_power_state_set(PMU_WIFI_SLEEP);
    }

    NVIC_SystemReset();
}

/*!
    \brief      iperf2 test
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_iperf2(int argc, char **argv)
{
    int i;
    if(argc >= 2){
        if (strcmp(argv[1], "-h") == 0) {
            goto Usage;
        }
    } else {
        goto Exit;
    }

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            cmd_udp(argc, argv);
            return;
        }
    }
    cmd_tcp(argc, argv);
    return;
Exit:
    DEBUGPRINT("\r\nIperf2: command format error!\r\n");
Usage:
    DEBUGPRINT("\rUsage:\r\n");
    DEBUGPRINT("    iperf <-s|-c hostip|exit|-h> [options]\r\n");
    DEBUGPRINT("\rClient/Server:\r\n");
    DEBUGPRINT("    -u #      use UDP rather than TCP\r\n");
    DEBUGPRINT("    -i #      seconds between periodic bandwidth reports\r\n");
    DEBUGPRINT("    -l #      length of buffer to read or write (default 1460 Bytes)\r\n");
    DEBUGPRINT("    -p #      server port to listen on/connect to (default 5001)\r\n");
    //DEBUGPRINT("    -N        set TCP no delay, disabling Nagle's Algorithm\r\n");
    DEBUGPRINT("\rServer specific:\r\n");
    DEBUGPRINT("    -s        run in server mode\r\n");
    DEBUGPRINT("\rClient specific:\r\n");
    DEBUGPRINT("    -b #      bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)\r\n");
    DEBUGPRINT("    -S #      set the IP 'type of service'\r\n");
    DEBUGPRINT("    -c <host> run in client mode, connecting to <host>\r\n");
    //DEBUGPRINT("    -d        Do a bidirectional test simultaneously\r\n");
    //DEBUGPRINT("    -n #      number of bytes to transmit for (default not use)\r\n");
    DEBUGPRINT("    -t #      time in seconds to transmit for (default 10 secs)\r\n");
}

/*!
    \brief      exit command test mode
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_exit(int argc, char **argv)
{
    DEBUGPRINT("\rLEAVE COMMAND TEST MODE\r\n");
    exit_cmd_mode = 1;
}

#ifdef CONFIG_FATFS_SUPPORT
/*!
    \brief      fatfs related operation
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
void cmd_fatfs (int argc, char **argv)
{

    if(argc >= 2 && argc <= 4){
        if (cmd_fatfs_exec(argc, argv) == 0)
            return;
    }
    DEBUGPRINT("\r\nUsage:\r\n");
    DEBUGPRINT("    fatfs create <path | path/filename>(path should end with \\ or /)\r\n");
    DEBUGPRINT("    fatfs append <path/filename> <string>\r\n");
    DEBUGPRINT("    fatfs read   <path/filename> [length]\r\n");
    DEBUGPRINT("    fatfs delete <path | path/filename>\r\n");
    DEBUGPRINT("    fatfs show   [dir]\r\n");
    DEBUGPRINT("    Example: fatfs creat a/b/c/d/ | fatfs creat a/b/c/d.txt\r\n");
}
#endif

/*!
    \brief      set bw to support 20MHz only or 20/40MHz
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_bw_set(int argc, char **argv)
{
    char *endptr = NULL;
    uint32_t cmd_bw = CHANNEL_WIDTH_20, cur_bw = 0, connected_bw = 0;
    int status = 0;

    if (!p_wifi_netlink->device_opened) {
        DEBUGPRINT("wifi device is closed! please use 'wifi_open' to open device\r\n");
        return;
    }

    if (argc >= 3) {
        DEBUGPRINT("wifi_set_bw: command format error!\r\n");
        goto Usage;
    }

    // get current supported bw
    status = wifi_netlink_bw_get(&cur_bw);
    if (status != 0) {
        DEBUGPRINT("ERROR: get current supported bandwidth failed\r\n");
        goto Usage;
    }

    if(argc == 2) {
        cmd_bw = (uint32_t)strtoul((const char *)argv[1], &endptr, 10);
        if (*endptr != '\0' || (cmd_bw != CHANNEL_WIDTH_20 && cmd_bw != CHANNEL_WIDTH_40)) {
            DEBUGPRINT("wifi_set_bw: invalid bandwidth\r\n");
            goto Usage;
        }

        if (cmd_bw == cur_bw) {
            DEBUGPRINT("No need to modify the supported bandwidth.\r\n");
            return;
        }

        if (wifi_netlink_link_state_get() >= WIFI_NETLINK_STATUS_LINKED) {
            connected_bw = p_wifi_netlink->connected_ap_info.bw;
            if ((cmd_bw == connected_bw) && (connected_bw != cur_bw)) {
                status = wifi_netlink_bw_set(cmd_bw);
                if (status != 0)
                    DEBUGPRINT("ERROR: wifi_set_bw failed\r\n");
                return;
            }

            // reconnect
            wifi_management_disconnect();
            status = wifi_netlink_bw_set(cmd_bw);
            if (status != 0) {
                DEBUGPRINT("ERROR: wifi_set_bw failed\r\n");
                return;
            }
            if (WIFI_ENCRYPT_PROTOCOL_OPENSYS == p_wifi_netlink->connect_info.encryp_protocol) {
                status = wifi_management_connect(p_wifi_netlink->connect_info.ssid.ssid,
                                            NULL, FALSE);
            } else {
                status = wifi_management_connect(p_wifi_netlink->connect_info.ssid.ssid,
                                            p_wifi_netlink->connect_info.passwd, FALSE);
            }
            if (status != 0)
                DEBUGPRINT("start wifi_connect failed\r\n");
        } else {
            status = wifi_netlink_bw_set(cmd_bw);
            if (status != 0)
                DEBUGPRINT("ERROR: wifi_set_bw failed\r\n");
        }
        return;
    } else {
        switch (cur_bw) {
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
    }

Usage:
    DEBUGPRINT("\rUsage:\r\n");
    DEBUGPRINT("    wifi_set_bw [0/1]\r\n");
    DEBUGPRINT("    0 #      support 20MHz only\r\n");
    DEBUGPRINT("    1 #      support 20/40MHz\r\n");
}

static const cmd_entry cmd_table[] = {
#ifdef CONFIG_BASECMD
    {"wifi_open", cmd_wifi_open},
    {"wifi_close", cmd_wifi_close},
    {"wifi_scan", cmd_wifi_scan},
    {"wifi_set_ip", cmd_wifi_ip_set},
    {"wifi_connect", cmd_wifi_connect},
    {"wifi_disconnect", cmd_wifi_disconnect},
    {"wifi_status", cmd_wifi_status},
    {"wifi_rssi", cmd_wifi_rssi},
    {"wifi_set_channel", cmd_wifi_channel_set},
    {"wifi_mac_addr", cmd_wifi_mac_addr},
    {"wifi_ps", cmd_wifi_ps},
    {"wifi_ap", cmd_wifi_ap},
    {"wifi_ap_adv", cmd_wifi_ap_adv},
    {"wifi_stop_ap", cmd_wifi_ap_stop},
    {"mem_status", cmd_mem_status},
    {"ping", cmd_ping},
#ifdef CONFIG_JOIN_GROUP_SUPPORT
    {"join_group", cmd_group_join},
#endif
#ifdef CONFIG_TELNET_SERVER
    {"telnet", cmd_telnet_server},
#endif
#ifdef CONFIG_IPERF_TEST
    {"iperf", cmd_iperf2},
    {"iperf3", cmd_iperf3},
#endif
#ifdef CONFIG_SSL_TEST
    {"ssl_client", cmd_ssl_client},
    {"ssl_selftest", cmd_ssl_selftest},
#endif
#ifdef CONFIG_ALICLOUD_SUPPORT
    {"ali_cloud", cmd_alicloud_linkkit},
#endif
    {"exit", cmd_exit},
#if defined(CONFIG_ATCMD)
    {"AT", at_entry},
#endif
#ifdef CONFIG_FATFS_SUPPORT
   {"fatfs", cmd_fatfs},
#endif
    {"wifi_set_bw", cmd_bw_set},
#endif
    {"reboot", cmd_reboot},
    {"help", cmd_help}
};

/*!
    \brief      show help command list
    \param[in]  argc: number of parameters
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     none
*/
static void cmd_help(int argc, char **argv)
{
    int i;

    DEBUGPRINT("\rCOMMAND LIST:\r\n");
    DEBUGPRINT("==============================\r\n");

    for (i = 0; i < ARRAY_SIZE(cmd_table); i++)
        DEBUGPRINT("    %s\r\n", cmd_table[i].command);

#ifdef CONFIG_RF_TEST_SUPPORT
    mp_cmd_help();
#endif
#ifdef CONFIG_INTERNAL_DEBUG
    inner_cmd_help();
#endif
}

/*!
    \brief      parse command
    \param[in]  buf: the pointer to the buffer that holds commands
    \param[in]  argv: the pointer to the array that holds the parameters
    \param[out] none
    \retval     number of commands
*/
static int cmd_parse(char *buf, char **argv)
{
    int argc = 0;

    while((argc < MAX_ARGC) && (*buf != '\0')) {
        argv[argc] = buf;
        argc ++;
        buf ++;

        while((*buf != ' ') && (*buf != '\0'))
            buf ++;

        while(*buf == ' ') {
            *buf = '\0';
            buf ++;
        }
        // Don't replace space
        if(argc == 1){
            if(strcmp(argv[0], "iwpriv") == 0){
                if(*buf != '\0'){
                    argv[1] = buf;
                    argc ++;
                }
                break;
            }
        }
    }

    return argc;
}

/*!
    \brief      command handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void command_handler(void)
{
    int i, argc;
    char *argv[MAX_ARGC];
    extern char uart_buf[];
    char local_uart_buf[UART_BUFFER_SIZE];
    WIFI_MESSAGE_TYPE_E msg_type;

    exit_cmd_mode = 0;

    while (1) {
        while (1) {
            if (sys_task_wait(0, &msg_type) == OS_OK) {
                if (msg_type == WIFI_MESSAGE_SHELL_COMMAND) {
                    sys_memcpy(local_uart_buf, uart_buf, UART_BUFFER_SIZE);
                    uart_buf[0] = '\0';
                    DEBUGPRINT("\r\n");
                    break;
                }
            } else {
                DEBUGPRINT("command_handler: sys_task_wait error\n");
            }
        }

#ifdef CONFIG_ATCMD
        if (atcmd_mode_get()) {
            if ((argc = atcmd_parse(local_uart_buf, argv)) > 0) {
                int found = 0;
                for (i = 0; i < ARRAY_SIZE(atcmd_table); i++) {
                    if ((strlen(argv[0]) == strlen(atcmd_table[i].command) || strlen(argv[0]) == (strlen(atcmd_table[i].command) + 1))
                        && (strncmp(argv[0], atcmd_table[i].command, strlen(atcmd_table[i].command)) == 0)) {
                        atcmd_table[i].function(argc, argv);
                        found = 1;
                        break;
                    }
                }
                if (!found)
                    at_rsp_error();
            }
        } else
#endif
        {
            if ((argc = cmd_parse(local_uart_buf, argv)) > 0) {
                int found = 0;
                for (i = 0; i < ARRAY_SIZE(cmd_table); i++) {
                    if (strcmp(argv[0], cmd_table[i].command) == 0) {
                        cmd_table[i].function(argc, argv);
                        found = 1;
                        break;
                    }
                }
#ifdef CONFIG_RF_TEST_SUPPORT
                if (!found) {
                    found = mp_cmd_handler(argc, argv);
                }
#endif
#ifdef CONFIG_INTERNAL_DEBUG
                if (!found) {
                    found = inner_cmd_handler(argc, argv);
                }
#endif
                if (!found)
                    DEBUGPRINT("Unknown command '%s'\r\n", argv[0]);
            }
            DEBUGPRINT("# ");
        }

        if (exit_cmd_mode == 1) {
            break;
        }
    }
}
#endif  /* defined(CONFIG_CONSOLE_ENABLE) */

#ifdef CONFIG_TELNET_SERVER
void cmd_telnet_server(int argc, char **argv)
{
    if (argc != 2) {
        goto exit;
    }
    if (strcmp(argv[1], "start") == 0) {
        telnet_server_start();
    } else if (strcmp(argv[1], "stop") == 0) {
        telnet_server_stop();
    } else {
        goto exit;
    }
    return;

exit:
    DEBUGPRINT("Usage: telnet <start/stop>\r\n");
    DEBUGPRINT("\tYou can use 'tenlet start' to start task or use 'tenlet stop' to stop task\r\n");
    return;
}
#endif
