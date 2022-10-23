/*!
    \file    net_intf.h
    \brief   Network interface definition for GD32W51x WiFi driver

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

#ifndef __NET_INTF_H_
#define __NET_INTF_H_

#include "wlan_cfg.h"

#ifdef CONFIG_SOC_HCI
#define MAX_PBUF_ONE_PKT 8
#else
#define MAX_PBUF_ONE_PKT 6
#endif

/**
 * @brief tcpip packet information which describes a tcpip packet
 */
struct tcpip_packet_info {
    uint8_t *data_ptr[MAX_PBUF_ONE_PKT];
    uint16_t data_len[MAX_PBUF_ONE_PKT];
    uint16_t buf_num;
    uint16_t packet_len;
};

#ifndef CONFIG_SOC_HCI
/**
 * @brief rx packet information which describes a received packet
 */
struct rx_packet_info {
    uint8_t *data_ptr;
    uint32_t data_len;
};
#endif
typedef uint8_t (*wifi_output_fn)(void *adapter, struct tcpip_packet_info *packet);
typedef void (*ethif_input_fn)(void *eth_if, struct tcpip_packet_info *rx_packet);

/* IMPORT from ethernetif.c */
extern void ethernetif_register_wifi(void *ethif, void *adapter, uint8_t *mac_addr, wifi_output_fn xmit_func);

/* EXPORT to ethernetif.c */
void wifi_register_ethernetif(void *ethif, void *adapter, ethif_input_fn ethif_input);

void sys_memcpy1(uint8_t *dst, uint8_t *src, uint32_t len);
void sys_memcpy2(uint8_t *dst, uint8_t *src, uint32_t len);

#endif
