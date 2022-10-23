/*!
    \file    wifi_management.h
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

#ifndef _WIFI_MANAGEMENT_H_
#define _WIFI_MANAGEMENT_H_

/*============================ INCLUDES ======================================*/
#include "wifi_eloop.h"
#include "wifi_netlink.h"
#include "wifi_netif.h"

/*============================ MACROS ========================================*/
#ifndef EXTERN
#define EXTERN extern
#endif

// #define CONFIG_WIFI_ROAMING_SUPPORT
#define CONFIG_WIFI_MANAGEMENT_TASK

#if defined(CONFIG_WIFI_ROAMING_SUPPORT) && defined(CONFIG_WIFI_MANAGEMENT_TASK)
#error CONFIG_WIFI_ROAMING_SUPPORT and CONFIG_WIFI_MANAGEMENT_TASK should not enabled at the same time
#endif

#define WIFI_MGMT_TASK_STK_SIZE                512
#define WIFI_MGMT_TASK_QUEUE_SIZE              4
#define WIFI_MGMT_TASK_PRIO                    (TASK_PRIO_APP_BASE + TASK_PRIO_HIGHER(2))

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
EXTERN uint32_t wifi_mgmt_task_stk[WIFI_MGMT_TASK_STK_SIZE] __ALIGNED(8);
EXTERN os_task_t wifi_mgmt_task_tcb;
#endif

#define WIFI_SM_ERROR            1
#define WIFI_SM_WARNING          2
#define WIFI_SM_NOTICE           3
#define WIFI_SM_INFO             4
#define WIFI_SM_DEBUG            5

#define WIFI_SM_LOG_LEVEL        WIFI_SM_NOTICE
#define WIFI_SM_DEBUG_ENABLE

#define WIFI_MGMT_ROAMING_RETRY_LIMIT                   10
#define WIFI_MGMT_ROAMING_RETRY_INTERVAL                10000
#define WIFI_MGMT_ROAMING_RETRY_UNLIMITED               UINT32_MAX
#define WIFI_MGMT_DHCP_POLLING_LIMIT                    200
#define WIFI_MGMT_DHCP_POLLING_INTERVAL                 100
#define WIFI_MGMT_LINK_POLLING_INTERVAL                 10000

#define WIFI_MGMT_TRIGGER_ROAMING_RSSI_THRESHOLD        -75
#define WIFI_MGMT_START_ROAMING_RSSI_THRESHOLD_1        10
#define WIFI_MGMT_START_ROAMING_RSSI_THRESHOLD_2        20
#define WIFI_MGMT_START_SCAN_THROTTLE_INTERVAL          15        // * WIFI_MGMT_LINK_POLLING_INTERVAL
#define WIFI_MGMT_START_SCAN_FAST_INTERVAL              5        // * WIFI_MGMT_LINK_POLLING_INTERVAL

/* IP config for softap mode */
#define CONFIG_IP4_ADDR(ip)            IP4_ADDR(ip, 192, 168, 237, 1)
#define CONFIG_IP4_ADDR_NM(ip)         IP4_ADDR(ip, 255, 255, 255, 0)
#define CONFIG_IP4_ADDR_GW(ip)         IP4_ADDR(ip, 192, 168, 237, 1)
/*============================ MACRO FUNCTIONS ===============================*/
#if defined(CONFIG_INTERNAL_DEBUG) && defined(WIFI_SM_DEBUG_ENABLE)
#define wifi_sm_printf(level, ...) do { \
        if (level <= WIFI_SM_LOG_LEVEL) \
            co_printf(__VA_ARGS__); \
    } while (0)
#else
#define wifi_sm_printf(...)
#endif

/* TODO: To retrieve the retry strategy from the configuration space */
#define WIFI_MGMT_UNLIMITED_RETRY()            (0)

/*============================ TYPES =========================================*/
typedef enum {
    WIFI_MGMT_EVENT_START = ELOOP_EVENT_MAX,
    WIFI_MGMT_EVENT_INIT,
    WIFI_MGMT_EVENT_CONNECT_CMD,
    WIFI_MGMT_EVENT_DISCONNECT_CMD,
    WIFI_MGMT_EVENT_SCAN_DONE,
    WIFI_MGMT_EVENT_SCAN_FAIL,
    WIFI_MGMT_EVENT_CONNECT_SUCCESS,
    WIFI_MGMT_EVENT_CONNECT_FAIL,
    WIFI_MGMT_EVENT_DISCONNECT,
    WIFI_MGMT_EVENT_DHCP_SUCCESS,
    WIFI_MGMT_EVENT_DHCP_FAIL,
    WIFI_MGMT_EVENT_AUTO_CONNECT,
    WIFI_MGMT_EVENT_MAX,
    WIFI_MGMT_EVENT_NUM = WIFI_MGMT_EVENT_MAX - WIFI_MGMT_EVENT_START - 1,
} wifi_management_event_t;

typedef enum {
    MAINTAIN_CONNECTION_IDLE,
    MAINTAIN_CONNECTION_SCAN,
    MAINTAIN_CONNECTION_CONNECT,
    MAINTAIN_CONNECTION_DHCP,
    MAINTAIN_CONNECTION_CONNECTED,
} MAINTAIN_CONNECTION_STATE;
typedef struct wifi_management_sm_data {
    wifi_management_event_t event;
    uint16_t reason;
    uint8_t dhcp_polling_count;
    uint8_t init;
    uint32_t retry_count;
    uint32_t retry_interval;    /* in milliseconds */
    MAINTAIN_CONNECTION_STATE MAINTAIN_CONNECTION_state;
    uint8_t changed;
} wifi_management_sm_data_t;

/*============================ GLOBAL VARIABLES ==============================*/
#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN wifi_management_sm_data_t wifi_sm_data;

#undef EXTERN

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
void wifi_management_init(void);
void wifi_management_start(void);
int wifi_management_scan(uint8_t blocked);
int wifi_management_connect(uint8_t *ssid, uint8_t *password, uint8_t blocked);
int wifi_management_disconnect(void);
int wifi_management_block_wait();
void wifi_management_task(void *arg);
int wifi_management_forward_netlink_message(WIFI_MESSAGE_TYPE_E message);

void wifi_mgmt_cb_scan_done(void *eloop_data, void *user_ctx);
void wifi_mgmt_cb_scan_fail(void *eloop_data, void *user_ctx);
void wifi_mgmt_cb_run_state_machine(void *eloop_data, void *user_ctx);
void wifi_mgmt_cb_scan_start(void *eloop_data, void *user_ctx);
uint32_t wifi_management_ap_assoc_info(uint8_t *assoc_info);
void wifi_management_ap_start(char *ssid, char *passwd, uint32_t channel, uint32_t hidden);
void wifi_management_sta_start(void);
void hw_crypt_lock_init(void);

/*============================ IMPLEMENTATION ================================*/

#define WIFI_MANAGEMENT_INSTALL_STATIC_EVENTS()     { \
        ELOOP_INSTALL_STATIC_EVENT(ELOOP_EVENT_ALL, wifi_mgmt_cb_run_state_machine, &wifi_sm_data, NULL),    \
        /* ELOOP_INSTALL_STATIC_EVENT(WIFI_MGMT_EVENT_SCAN_CMD, wifi_mgmt_cb_scan_start, NULL, NULL), */    \
    }

#endif  // _WIFI_MANAGEMENT_H_
