/*!
    \file    soc_intf.h
    \brief   SoC related interface for GD32W51x WiFi driver

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

#ifndef __SOC_INTF_H__
#define __SOC_INTF_H__

#ifndef SRAM_DATA_BASE_ADDR
#define SRAM_DATA_BASE_ADDR               (soc_intf.sram_base)
#endif
#define WLAN_REG_BASE_ADDR                (soc_intf.wifi_reg_base)
#define WLAN_INTERNAL_BUF_BASE_ADDR       (soc_intf.wifi_reg_base + 0x1000)

#define MCU_REG_NVIC_SETEN(n)             (*((volatile uint32_t *)(soc_intf.nvic_base + (n) * 4u)))         // IRQ Set En Reg.
#define MCU_REG_NVIC_CLREN(n)             (*((volatile uint32_t *)(soc_intf.nvic_base + 0x80 + (n) * 4u)))  // IRQ Clr En Reg.

typedef struct soc_intf_t {
    uint32_t dump_sram_base;
    uint32_t wifi_reg_base;
    uint32_t nvic_base;
} soc_intf_t;

extern soc_intf_t soc_intf;

void wifi_set_reg_base(uint32_t reg_base);
void wifi_set_dump_sram_base(uint32_t dump_sram_base);
void wifi_set_nvic_base(uint32_t nvic_base);

#endif  //__SOC_INTF_H__
