/*!
    \file    wifi_management.c
    \brief   WiFi management for GD32W51x WiFi SDK

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
#include "app_cfg.h"
#include "state_machine.h"
#include "bsp_inc.h"
#include "wlan_intf.h"
#include "wifi_netlink.h"
#include "lwip/netifapi.h"
#include "debug_print.h"
#include "dhcpd.h"
#define EXTERN
#include "wifi_management.h"

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
/*============================ MACROS ========================================*/
#define STATE_MACHINE_DATA struct wifi_management_sm_data
#define STATE_MACHINE_DEBUG_PREFIX "WIFI_MGMT"

/*============================ MACRO FUNCTIONS ===============================*/
#define GET_SM_STATE(machine) \
    sm->machine ## _state

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
os_mutex_t hw_pka_lock;
os_mutex_t hw_cryp_lock;
os_mutex_t hw_hash_lock;
/*============================ LOCAL VARIABLES ===============================*/
static int8_t candidate_rssi_avg;
static int8_t candidate_rssi;
static uint8_t fast_scan;
static uint8_t throttle_scan;
static uint8_t ap_dhcpd_started = 0;

/*============================ PROTOTYPES ====================================*/
static void wifi_mgmt_dhcp_polling(void *eloop_data, void *user_ctx);
static void wifi_mgmt_link_status_polling(void *eloop_data, void *user_ctx);
static void wifi_mgmt_connect_retry(void *eloop_data, void *user_ctx);
#ifdef CONFIG_RF_TEST_SUPPORT
#include "cmd_rf_test.h"
#endif

/*============================ IMPLEMENTATION ================================*/

/***************************** WiFi Management State Machine ******************/
SM_STATE(MAINTAIN_CONNECTION, IDLE)
{
    SM_ENTRY(MAINTAIN_CONNECTION, IDLE);

    wifi_netif_stop_dhcp();

    eloop_timeout_cancel(wifi_mgmt_dhcp_polling, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_timeout_cancel(wifi_mgmt_link_status_polling, ELOOP_ALL_CTX, ELOOP_ALL_CTX);

    if (p_wifi_netlink->link_status >= WIFI_NETLINK_STATUS_LINKED) {
        wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX ": disconnect with ap %s\r\n",
                       p_wifi_netlink->connected_ap_info.ssid.ssid);
        wifi_netlink_disconnect_req();
    }

    sys_memset(&p_wifi_netlink->connected_ap_info, 0, sizeof(struct wifi_connect_result));
    p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_NO_LINK;
}

SM_STATE(MAINTAIN_CONNECTION, SCAN)
{
    struct wifi_ssid_config *connect_info = &(p_wifi_netlink->connect_info);
    struct wifi_scan_req scan_req;
    SM_ENTRY(MAINTAIN_CONNECTION, SCAN);

    wifi_netif_stop_dhcp();

    eloop_timeout_cancel(wifi_mgmt_dhcp_polling, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
    eloop_timeout_cancel(wifi_mgmt_link_status_polling, ELOOP_ALL_CTX, ELOOP_ALL_CTX);

    sys_memcpy(&scan_req.ssid.ssid, connect_info->ssid.ssid, connect_info->ssid.ssid_len);
    scan_req.ssid.ssid_len = connect_info->ssid.ssid_len;

    if (wifi_ops_entry.wifi_scan_proc_func(&scan_req) == _FALSE) {
        wifi_sm_printf(WIFI_SM_ERROR, STATE_MACHINE_DEBUG_PREFIX": start scan failed\r\n");
        eloop_event_send(WIFI_MGMT_EVENT_SCAN_FAIL);
    }
}

SM_STATE(MAINTAIN_CONNECTION, CONNECT)
{
    struct wifi_ssid_config *connect_info = &(p_wifi_netlink->connect_info);
    SM_ENTRY(MAINTAIN_CONNECTION, CONNECT);

    p_wifi_netlink->link_status = WIFI_NETLINK_STATUS_LINKING;
    if (wifi_ops_entry.wifi_connect_req_func(connect_info) == _FALSE) {
        wifi_sm_printf(WIFI_SM_ERROR, STATE_MACHINE_DEBUG_PREFIX": start connecting failed\r\n");
    }
}

SM_STATE(MAINTAIN_CONNECTION, DHCP)
{
    SM_ENTRY(MAINTAIN_CONNECTION, DHCP);

    sm->dhcp_polling_count = WIFI_MGMT_DHCP_POLLING_LIMIT;
    wifi_netif_start_dhcp();

    wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX ": start polling DHCP status\r\n");
    eloop_timeout_register(WIFI_MGMT_DHCP_POLLING_INTERVAL, wifi_mgmt_dhcp_polling, sm, NULL);
}

SM_STATE(MAINTAIN_CONNECTION, CONNECTED)
{
    SM_ENTRY(MAINTAIN_CONNECTION, CONNECTED);

    wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX ": start polling link status\r\n");

    candidate_rssi_avg = 0;
    fast_scan = WIFI_MGMT_START_SCAN_FAST_INTERVAL;
    throttle_scan = WIFI_MGMT_START_SCAN_THROTTLE_INTERVAL;

    if (wifi_netlink_auto_conn_get()) {
        wifi_netlink_joined_ap_store();
    }
    eloop_timeout_register(WIFI_MGMT_LINK_POLLING_INTERVAL, wifi_mgmt_link_status_polling, sm, NULL);
}

SM_STEP(MAINTAIN_CONNECTION)
{
    if (!sm->init || sm->event == WIFI_MGMT_EVENT_INIT) {
        sys_memset(sm, 0, sizeof(*sm));
        sm->init = TRUE;
        if (WIFI_MGMT_UNLIMITED_RETRY())
            sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
        else
            sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
        sm->retry_interval = WIFI_MGMT_ROAMING_RETRY_INTERVAL;

        SM_ENTER(MAINTAIN_CONNECTION, IDLE);
    } else if (GET_SM_STATE(MAINTAIN_CONNECTION) == MAINTAIN_CONNECTION_IDLE) {
        switch (sm->event) {
        case WIFI_MGMT_EVENT_CONNECT_CMD:
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            eloop_timeout_cancel(wifi_mgmt_connect_retry, sm, NULL);
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_AUTO_CONNECT:
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_DISCONNECT_CMD:
        case WIFI_MGMT_EVENT_SCAN_DONE:
        case WIFI_MGMT_EVENT_SCAN_FAIL:
        case WIFI_MGMT_EVENT_CONNECT_FAIL:
            break;
        // case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
        // case WIFI_MGMT_EVENT_DISCONN:
        // case WIFI_MGMT_EVENT_DHCP_SUCCESS:
        // case WIFI_MGMT_EVENT_DHCP_FAIL:
        default:
            goto unexpected_events;
        }
    } else if (GET_SM_STATE(MAINTAIN_CONNECTION) == MAINTAIN_CONNECTION_SCAN) {
        switch (sm->event) {
        case WIFI_MGMT_EVENT_CONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            eloop_timeout_cancel(wifi_mgmt_connect_retry, sm, NULL);
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_DISCONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            break;
        case WIFI_MGMT_EVENT_SCAN_DONE:
            SM_ENTER(MAINTAIN_CONNECTION, CONNECT);
            break;
        case WIFI_MGMT_EVENT_SCAN_FAIL:
            if (sm->retry_count--) {
                eloop_timeout_register(sm->retry_interval, wifi_mgmt_connect_retry, sm, NULL);
            } else {
                wifi_sm_printf(WIFI_SM_WARNING, STATE_MACHINE_DEBUG_PREFIX ": all connecting retries failed\r\n");
                SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            }
            break;
        // case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
        // case WIFI_MGMT_EVENT_CONNECT_FAIL:
        // case WIFI_MGMT_EVENT_DISCONNECT:
        // case WIFI_MGMT_EVENT_DHCP_SUCCESS:
        // case WIFI_MGMT_EVENT_DHCP_FAIL:
        default:
            goto unexpected_events;
        }
    } else if (GET_SM_STATE(MAINTAIN_CONNECTION) == MAINTAIN_CONNECTION_CONNECT) {
        switch (sm->event) {
        case WIFI_MGMT_EVENT_CONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            eloop_timeout_cancel(wifi_mgmt_connect_retry, sm, NULL);
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_SCAN_DONE:
        case WIFI_MGMT_EVENT_SCAN_FAIL:
            break;
        case WIFI_MGMT_EVENT_DISCONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            break;
        case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
            SM_ENTER(MAINTAIN_CONNECTION, DHCP);
            break;
        case WIFI_MGMT_EVENT_CONNECT_FAIL:
        case WIFI_MGMT_EVENT_DISCONNECT:
            if (sm->retry_count--) {
                eloop_timeout_register(sm->retry_interval, wifi_mgmt_connect_retry, sm, NULL);
                SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            } else {
                wifi_sm_printf(WIFI_SM_WARNING, STATE_MACHINE_DEBUG_PREFIX ": all connecting retries failed\r\n");
                SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            }
            break;
        // case WIFI_MGMT_EVENT_DHCP_SUCCESS:
        // case WIFI_MGMT_EVENT_DHCP_FAIL:
        default:
            goto unexpected_events;
        }
    } else if (GET_SM_STATE(MAINTAIN_CONNECTION) == MAINTAIN_CONNECTION_DHCP) {
        switch (sm->event) {
        case WIFI_MGMT_EVENT_CONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            eloop_timeout_cancel(wifi_mgmt_connect_retry, sm, NULL);
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_SCAN_DONE:
        case WIFI_MGMT_EVENT_SCAN_FAIL:
            break;
        case WIFI_MGMT_EVENT_DISCONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            break;
        case WIFI_MGMT_EVENT_DHCP_SUCCESS:
            SM_ENTER(MAINTAIN_CONNECTION, CONNECTED);
            eloop_timeout_cancel(wifi_mgmt_connect_retry, ELOOP_ALL_CTX, ELOOP_ALL_CTX);
            break;
        case WIFI_MGMT_EVENT_DHCP_FAIL:
            wifi_netlink_disconnect_req();
        case WIFI_MGMT_EVENT_CONNECT_FAIL:
        case WIFI_MGMT_EVENT_DISCONNECT:
            if (sm->retry_count--) {
                eloop_timeout_register(sm->retry_interval, wifi_mgmt_connect_retry, sm, NULL);
            } else {
                wifi_sm_printf(WIFI_SM_WARNING, STATE_MACHINE_DEBUG_PREFIX ": all connecting retries failed\r\n");
                SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            }
            break;
        // case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
        default:
            goto unexpected_events;
        }
    } else if (GET_SM_STATE(MAINTAIN_CONNECTION) == MAINTAIN_CONNECTION_CONNECTED) {
        switch (sm->event) {
        case WIFI_MGMT_EVENT_CONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            eloop_timeout_cancel(wifi_mgmt_connect_retry, sm, NULL);
            SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            break;
        case WIFI_MGMT_EVENT_DISCONNECT_CMD:
            SM_ENTER(MAINTAIN_CONNECTION, IDLE);
            break;
        case WIFI_MGMT_EVENT_SCAN_DONE:
        case WIFI_MGMT_EVENT_SCAN_FAIL:
            break;
        case WIFI_MGMT_EVENT_DISCONNECT:
            if (WIFI_MGMT_UNLIMITED_RETRY())
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_UNLIMITED;
            else
                sm->retry_count = WIFI_MGMT_ROAMING_RETRY_LIMIT - 1;
            if (sm->reason == WIFI_MESSAGE_INDICATE_DISCON_AP_CHANGED ||
                    sm->reason == WIFI_MESSAGE_INDICATE_DISCON_NO_BEACON ||
                    sm->reason == WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH)
                SM_ENTER(MAINTAIN_CONNECTION, SCAN);
            else
                SM_ENTER(MAINTAIN_CONNECTION, CONNECT);
            break;
        // case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
        // case WIFI_MGMT_EVENT_CONNECT_FAIL:
        // case WIFI_MGMT_EVENT_DHCP_SUCCESS:
        // case WIFI_MGMT_EVENT_DHCP_FAIL:
        default:
            goto unexpected_events;
        }
    }

    if (!sm->changed) {
        wifi_sm_printf(WIFI_SM_DEBUG, STATE_MACHINE_DEBUG_PREFIX
                       ": stay in the state %u upon the event (%u:%u)\r\n",
                       GET_SM_STATE(MAINTAIN_CONNECTION), sm->event, sm->reason);
    }
    sm->changed = FALSE;

    return;

unexpected_events:
    wifi_sm_printf(WIFI_SM_WARNING, STATE_MACHINE_DEBUG_PREFIX
                   ": unexpected event (%u:%u) in the state %u\r\n",
                   sm->event, sm->reason, GET_SM_STATE(MAINTAIN_CONNECTION));
}

/*!
    \brief      wifi management run state machine
    \param[in]  eloop_data: pointer to the eloop data
    \param[in]  user_ctx: pointer to the user parameters
    \param[out] none
    \retval     none
*/
void wifi_mgmt_cb_run_state_machine(void *eloop_data, void *user_ctx)
{
    wifi_management_sm_data_t *sm = eloop_data;
    eloop_message_t *message = (eloop_message_t *)user_ctx;

    if (message->event < WIFI_MGMT_EVENT_INIT)
        return;

    sm->event = (wifi_management_event_t)message->event;
    sm->reason = message->reason;

    wifi_sm_printf(WIFI_SM_DEBUG, STATE_MACHINE_DEBUG_PREFIX
                   ": received message: (%u:%u:%u)\r\n",
                   sm->MAINTAIN_CONNECTION_state, message->event, message->reason);

    SM_STEP_RUN(MAINTAIN_CONNECTION);
}

/************************ WiFi Management Callbacks ***************************/

/************************ WiFi Management Timeouts ****************************/
/*!
    \brief      wifi management dhcp polling
    \param[in]  eloop_data: pointer to the eloop data
    \param[in]  user_ctx: pointer to the user parameters
    \param[out] none
    \retval     none
*/
static void wifi_mgmt_dhcp_polling(void *eloop_data, void *user_ctx)
{
    wifi_management_sm_data_t *sm = eloop_data;

    if (wifi_netif_is_ip_got()) {
        wifi_netlink_ipaddr_set((uint8_t *)wifi_netif_get_ip());
        //DEBUGPRINT("DHCP: IP configured: "IP_FMT"\r\n", IP_ARG(wifi_netif_get_ip()->addr));
        eloop_event_send(WIFI_MGMT_EVENT_DHCP_SUCCESS);
        if (p_wifi_netlink->conn_blocked) {
            sys_sema_up(&p_wifi_netlink->block_sema);
            p_wifi_netlink->conn_blocked = FALSE;
        }
        return;
    }

    if (--sm->dhcp_polling_count) {
        if ((sm->dhcp_polling_count & 0xF) == 0) {
            wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX ": waiting for DHCP(%d)...\r\n", sm->dhcp_polling_count);
        }
        eloop_timeout_register(WIFI_MGMT_DHCP_POLLING_INTERVAL, wifi_mgmt_dhcp_polling, sm, NULL);
    } else {
        DEBUGPRINT("DHCP: IP request timeouted!\r\n");
        wifi_netif_stop_dhcp();
        eloop_event_send(WIFI_MGMT_EVENT_DHCP_FAIL);
    }
}

/*!
    \brief      find candidate ap
    \param[in]  eloscan_itemop_data: pointer to the wifi_scan_info struction
    \param[out] none
    \retval     none
*/
static void find_candidate_ap(struct wifi_scan_info *scan_item)
{
    if (!sys_memcmp(scan_item->ssid.ssid,
                   p_wifi_netlink->connected_ap_info.ssid.ssid,
                   p_wifi_netlink->connected_ap_info.ssid.ssid_len) &&
            sys_memcmp(scan_item->bssid_info.bssid,
                        p_wifi_netlink->connected_ap_info.bssid_info.bssid,
                        sizeof(scan_item->bssid_info.bssid))) {
        candidate_rssi_avg = (candidate_rssi_avg * 3 + scan_item->rssi) >> 2;
        candidate_rssi = scan_item->rssi;
        wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX
                       ": found a candidate AP "MAC_FMT", rssi %d, avg rssi %d\r\n",
                       MAC_ARG(scan_item->bssid_info.bssid), scan_item->rssi, candidate_rssi_avg);
    }
}

/*!
    \brief      wifi management scan done
    \param[in]  eloop_data: pointer to the eloop data
    \param[in]  user_ctx: pointer to the user parameters
    \param[out] none
    \retval     none
*/
void wifi_mgmt_cb_scan_done(void *eloop_data, void *user_ctx)
{
    int rssi;

    candidate_rssi = 0; /* rssi won't be larger than or equal to 0 */
    wifi_netlink_scan_list_get(find_candidate_ap);
    if (!candidate_rssi)
        goto exit;

    fast_scan = WIFI_MGMT_START_SCAN_FAST_INTERVAL;

    rssi = wifi_netlink_bss_rssi_get();
    if (rssi < WIFI_MGMT_TRIGGER_ROAMING_RSSI_THRESHOLD &&
            (rssi + WIFI_MGMT_START_ROAMING_RSSI_THRESHOLD_1 < candidate_rssi_avg ||
            rssi + WIFI_MGMT_START_ROAMING_RSSI_THRESHOLD_2 < candidate_rssi)) {
        wifi_sm_printf(WIFI_SM_NOTICE, STATE_MACHINE_DEBUG_PREFIX ": try roaming to a better AP\r\n");
        eloop_event_send(WIFI_MGMT_EVENT_CONNECT_CMD);
    }

exit:
    eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
}

/*!
    \brief      wifi management link status polling
    \param[in]  eloop_data: pointer to the eloop data
    \param[in]  user_ctx: pointer to the user parameters
    \param[out] none
    \retval     none
*/
static void wifi_mgmt_link_status_polling(void *eloop_data, void *user_ctx)
{
    wifi_management_sm_data_t *sm = eloop_data;
    int rssi;

    wifi_sm_printf(WIFI_SM_INFO, STATE_MACHINE_DEBUG_PREFIX ": polling link status\r\n");

    rssi = wifi_netlink_bss_rssi_get();
    if (rssi < WIFI_MGMT_TRIGGER_ROAMING_RSSI_THRESHOLD && (fast_scan || --throttle_scan == 0)) {
        if (candidate_rssi_avg == 0)
            candidate_rssi_avg = rssi;

        eloop_event_register(WIFI_MGMT_EVENT_SCAN_DONE, wifi_mgmt_cb_scan_done, NULL, NULL);

        if (wifi_management_scan(FALSE) != 0) {
            eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
        }

        if (fast_scan)
            fast_scan--;
        if (throttle_scan == 0)
            throttle_scan = WIFI_MGMT_START_SCAN_THROTTLE_INTERVAL;
    }

    eloop_timeout_register(WIFI_MGMT_LINK_POLLING_INTERVAL, wifi_mgmt_link_status_polling, sm, NULL);
}

/*!
    \brief      wifi management connect retry
    \param[in]  eloop_data: pointer to the eloop data
    \param[in]  user_ctx: pointer to the user parameters
    \param[out] none
    \retval     none
*/
static void wifi_mgmt_connect_retry(void *eloop_data, void *user_ctx)
{
    wifi_management_sm_data_t *sm = eloop_data;

    wifi_sm_printf(WIFI_SM_NOTICE, STATE_MACHINE_DEBUG_PREFIX ": retry to connect, remaining times 0x%x\r\n",
                   sm->retry_count);
    SM_ENTER(MAINTAIN_CONNECTION, SCAN);
}

/************************ WiFi Management Interfaces **************************/
/*!
    \brief      wifi management connect
    \param[in]  ssid: pointer to the SSID
    \param[in]  password: pointer to the password
    \param[in]  blocked: whether block
    \param[out] none
    \retval     function run status
      \arg        -1: link is ongoing
      \arg        -2: SSID empty
      \arg        -3: SSID's length error
      \arg        -4: password's length error
      \arg        OS_OK: run success
*/
int wifi_management_connect(uint8_t *ssid, uint8_t *password, uint8_t blocked)
{
    struct wifi_ssid_config *connect_info = &p_wifi_netlink->connect_info;

    if (p_wifi_netlink->link_status == WIFI_NETLINK_STATUS_LINKING) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX": Link is ongoing... Please wait for a minute.\r\n");
        return -1;
    }

    if (ssid == NULL) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX": SSID empty!\r\n");
        return -2;
    }

    if (strlen((const char *)ssid) > WIFI_SSID_MAX_LEN) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX": SSID's length should be less than %u\r\n", WIFI_SSID_MAX_LEN);
        return -3;
    }

    if (password &&
            ((strlen((const char *)password) > WPA_MAX_PSK_LEN) ||
            ((strlen((const char *)password) < WPA_MIN_PSK_LEN)))) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX": password's length should be between %d and %d inclusively\r\n",
                    WPA_MIN_PSK_LEN, WPA_MAX_PSK_LEN);
        return -4;
    }

    connect_info->ssid.ssid_len = strlen((const char *)ssid);
    sys_memcpy(connect_info->ssid.ssid, ssid, connect_info->ssid.ssid_len + 1);
    if (password) {
        sys_memcpy(connect_info->passwd, password, strlen((const char *)password) + 1);
    } else {
        sys_memset(connect_info->passwd, 0, (WPA_MAX_PSK_LEN + 1));
    }
    if (blocked)
        p_wifi_netlink->conn_blocked = TRUE;

    return eloop_event_send(WIFI_MGMT_EVENT_CONNECT_CMD);
}

/*!
    \brief      wifi management disconnect
    \param[in]  none
    \param[out] none
    \retval     function run status
                  -1: error happen
                  OS_OK: run success
*/
int wifi_management_disconnect()
{
    return eloop_event_send(WIFI_MGMT_EVENT_DISCONNECT_CMD);
}

/*!
    \brief      wifi management scan
    \param[in]  blocked: whether block
    \param[out] none
    \retval     function run status
                  -1: error happen
                  0: run success
*/
int wifi_management_scan(uint8_t blocked)
{
    wifi_management_sm_data_t *sm = &wifi_sm_data;

    if (GET_SM_STATE(MAINTAIN_CONNECTION) != MAINTAIN_CONNECTION_CONNECTED &&
            GET_SM_STATE(MAINTAIN_CONNECTION) != MAINTAIN_CONNECTION_IDLE) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX
                  ": ignore scan in the connecting process (%d)\r\n", GET_SM_STATE(MAINTAIN_CONNECTION));
        return -1;
    }

    if (wifi_netlink_scan_set() != 1) {
        DEBUGPRINT(STATE_MACHINE_DEBUG_PREFIX ": start scan failed\r\n");
    }
    if (blocked)
        p_wifi_netlink->scan_blocked = TRUE;
    return 0;
}

/*!
    \brief      wifi management block wait
    \param[in]  none
    \param[out] none
    \retval     0: run success
*/
int wifi_management_block_wait(void)
{
    if ((p_wifi_netlink->scan_blocked == TRUE) || (p_wifi_netlink->conn_blocked == TRUE))
        sys_sema_down(&p_wifi_netlink->block_sema, 20000);
    return 0;
}

/*!
    \brief      wifi management forward netlink message
    \param[in]  message: wifi message type, refer to WIFI_MESSAGE_TYPE_E
    \param[out] none
    \retval     function run status
                  OS_ERROR: error happen
                  OS_OK: run success
*/
int wifi_management_forward_netlink_message(WIFI_MESSAGE_TYPE_E message)
{
    eloop_message_t wifi_mgmt_message;
    wifi_management_event_t event;

    switch (message) {
    case WIFI_MESSAGE_NOTIFY_SCAN_RESULT_SUCC:
        event = WIFI_MGMT_EVENT_SCAN_DONE;
        if (p_wifi_netlink->scan_blocked) {
            sys_sema_up(&p_wifi_netlink->block_sema);
            p_wifi_netlink->scan_blocked = FALSE;
        }
        break;
    case WIFI_MESSAGE_NOTIFY_SCAN_RESULT_FAIL:
        event = WIFI_MGMT_EVENT_SCAN_FAIL;
        if (p_wifi_netlink->scan_blocked) {
            sys_sema_up(&p_wifi_netlink->block_sema);
            p_wifi_netlink->scan_blocked = FALSE;
        }
        break;
    case WIFI_MESSAGE_INDICATE_CONN_SUCCESS:
        event = WIFI_MGMT_EVENT_CONNECT_SUCCESS;
        break;
    case WIFI_MESSAGE_INDICATE_CONN_NO_AP:
    case WIFI_MESSAGE_INDICATE_CONN_ASSOC_FAIL:
    case WIFI_MESSAGE_INDICATE_CONN_HANDSHAKE_FAIL:
    case WIFI_MESSAGE_INDICATE_CONN_FAIL:
        event = WIFI_MGMT_EVENT_CONNECT_FAIL;
        if (p_wifi_netlink->conn_blocked) {
            sys_sema_up(&p_wifi_netlink->block_sema);
            p_wifi_netlink->conn_blocked = FALSE;
        }
        break;
    case WIFI_MESSAGE_INDICATE_DISCON_REKEY_FAIL:
    case WIFI_MESSAGE_INDICATE_DISCON_MIC_FAIL:
    case WIFI_MESSAGE_INDICATE_DISCON_RECV_DEAUTH:
    case WIFI_MESSAGE_INDICATE_DISCON_NO_BEACON:
    case WIFI_MESSAGE_INDICATE_DISCON_AP_CHANGED:
    case WIFI_MESSAGE_INDICATE_DISCON_FROM_UI:
    case WIFI_MESSAGE_INDICATE_DISCON_UNSPECIFIED:
        event = WIFI_MGMT_EVENT_DISCONNECT;
        if (p_wifi_netlink->conn_blocked) {
            sys_sema_up(&p_wifi_netlink->block_sema);
            p_wifi_netlink->conn_blocked = FALSE;
        }
        break;
    default:
        return OS_ERROR;
    }

    wifi_mgmt_message.event = event;
    wifi_mgmt_message.reason = message;

    return eloop_message_send(wifi_mgmt_message);
}

/*!
    \brief      wifi management task
    \param[in]  arg: pointer to the argureargument
    \param[out] none
    \retval     none
*/
void wifi_management_task(void *arg)
{
    eloop_init();

    eloop_run();

    eloop_destroy();

    sys_task_delete(NULL);
}
/*!
    \brief      wifi management start
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_management_start(void)
{
    void *handle;

    COMPILE_TIME_ASSERT(WIFI_MGMT_EVENT_MAX <= UINT16_MAX);

    handle = sys_task_create(&wifi_mgmt_task_tcb, (const uint8_t *)"wifi_mgmt", &wifi_mgmt_task_stk[0],
                    WIFI_MGMT_TASK_STK_SIZE, WIFI_MGMT_TASK_QUEUE_SIZE, WIFI_MGMT_TASK_PRIO,
                    (task_func_t)wifi_management_task, NULL);
    if (handle == NULL) {
        DEBUGPRINT("creating wifi management task failed\r\n");
        return;
    }

    eloop_event_send(WIFI_MGMT_EVENT_INIT);
}

/*!
    \brief      wifi management ap start
    \param[in]  ssid: pointer to the SSID
    \param[in]  password: pointer to the password
    \param[in]  channel: channel
    \param[in]  hidden: whether hidden
    \param[out] none
    \retval     none
*/
void wifi_management_ap_start(char *ssid, char *passwd, uint32_t channel, uint32_t hidden)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    eloop_event_send(ELOOP_EVENT_TERMINATE);

    wifi_netif_stop_dhcp();

    /* Stop DHCPD */
    if (ap_dhcpd_started) {
        stop_dhcpd_daemon();
        ap_dhcpd_started = 0;
    }

    wifi_netlink_ap_start(ssid, passwd, channel, hidden);

#ifdef CONFIG_IPV6_SUPPORT
    CONFIG_IP4_ADDR(&ipaddr.u_addr.ip4);
    CONFIG_IP4_ADDR_NM(&netmask.u_addr.ip4);
    CONFIG_IP4_ADDR_GW(&gw.u_addr.ip4);
#else
    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);
#endif

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
}

/*!
    \brief      wifi management station start
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wifi_management_sta_start(void)
{
    if (p_wifi_netlink->ap_started){
        /* Stop DHCPD */
        if (ap_dhcpd_started) {
            stop_dhcpd_daemon();
            ap_dhcpd_started = 0;
        }
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }

    if (eloop_event_send(ELOOP_EVENT_WAKEUP) != OS_OK) {
        wifi_management_start();
    }
}

/*!
    \brief      wifi management ap assoc infromation
    \param[in]  assoc_info: pointer to the assoc infromation
    \param[out] none
    \retval     run status(0x0000000-0xffffffff)
*/
uint32_t wifi_management_ap_assoc_info(uint8_t *assoc_info)
{
    return wifi_ops_entry.wifi_softap_get_assoc_info(assoc_info);
}

#endif /* CONFIG_WIFI_MANAGEMENT_TASK */

void wifi_management_init(void)
{
    void *handle;

#ifndef CONFIG_RF_TEST_SUPPORT
    printf("System clock is %d\r\n", SystemCoreClock);
#endif

    hw_crypt_lock_init();

    tcpip_init(NULL, NULL);
    wifi_netlink_init();
    /* prevent os from going to tickless sleep */
    wifi_wakelock_acquire();
    wifi_netlink_dev_open();

#ifdef CONFIG_RF_TEST_SUPPORT
    mp_mode_config();
#endif

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_start();

    if (wifi_netlink_auto_conn_get()) {
        if (wifi_netlink_joined_ap_load() >= 0) {
            eloop_event_send(WIFI_MGMT_EVENT_AUTO_CONNECT);
        }
    }
#endif
}
