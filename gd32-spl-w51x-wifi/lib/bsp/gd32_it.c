/*!
    \file    gd32_it.c
    \brief   interrupt service routines for GD32W51x WiFi SDK

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

#include "bsp_inc.h"
#include "gd32_it.h"
#include "uart.h"
#include "dma.h"
#include "wrapper_os.h"
#include "bsp_wlan.h"
#include "wakelock.h"
//#include "systick.h"

extern void wlan_interrupt_rx_handler(void);
extern void wlan_interrupt_tx_handler(void);
extern void wlan_interrupt_others_handler(void);

extern void wlan_ps_others_isr(void);
extern void wlan_ps_rx_isr(void);
extern void wlan_ps_tx_isr(void);
extern void wlan_ps_wakeup_isr(void);

extern void systick_udelay(uint32_t nus);

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

#if 0
#ifndef SYS_HARDFAULT_HANDLER
static char msg[32];
/* Private typedef -----------------------------------------------------------*/
enum { r0, r1, r2, r3, r12, lr, pc, psr};

/* Private functions ---------------------------------------------------------*/
static void printUsageErrorMsg(uint32_t CFSRValue)
{
    printf("Usage fault: \r\n");
    CFSRValue >>= 16; // right shift to lsb

    if((CFSRValue & (1<<9)) != 0) {
        printf("Divide by zero \r\n");
    }
    if((CFSRValue & (1<<8)) != 0) {
        printf("Unaligned access \r\n");
    }
}

static void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    printf("Bus fault: \r\n");
    CFSRValue = ((CFSRValue & 0x0000FF00) >> 8); // mask and right shift to lsb
    printf("BFAR: %08x\r\n", SCB->BFAR);
}

static void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    printf("Memory Management fault: \r\n");
    CFSRValue &= 0x000000FF; // mask just mem faults
}

static void stackDump(uint32_t stack[])
{
    snprintf(msg, 32, "R0 = 0x%08x\r\n", stack[r0]); printf("%s", msg);
    snprintf(msg, 32, "R1 = 0x%08x\r\n", stack[r1]); printf("%s", msg);
    snprintf(msg, 32, "R2 = 0x%08x\r\n", stack[r2]); printf("%s", msg);
    snprintf(msg, 32, "R3 = 0x%08x\r\n", stack[r3]); printf("%s", msg);
    snprintf(msg, 32, "R12 = 0x%08x\r\n", stack[r12]); printf("%s", msg);
    snprintf(msg, 32, "LR = 0x%08x\r\n", stack[lr]); printf("%s", msg);
    snprintf(msg, 32, "PC = 0x%08x\r\n", stack[pc]); printf("%s", msg);
    snprintf(msg, 32, "PSR = 0x%08x\r\n", stack[psr]); printf("%s", msg);
}

void Hard_Fault_Handler(uint32_t stack[])
{
    //char buffer[448];

    //if((CoreDebug->DHCSR & 0x01) != 0) {
        printf("\r\nIn Hard Fault Handler\r\n");
        sprintf(msg, "SCB->HFSR = 0x%08x\r\n", SCB->HFSR);
        printf("%s", msg);
        if ((SCB->HFSR & (1 << 30)) != 0) {
            printf("Forced Hard Fault\r\n");
            sprintf(msg, "SCB->CFSR = 0x%08x\r\n", SCB->CFSR );
            printf("%s", msg);
            if((SCB->CFSR & 0xFFFF0000) != 0) {
                printUsageErrorMsg(SCB->CFSR);
            }
            if((SCB->CFSR & 0xFF00) != 0) {
                printBusFaultErrorMsg(SCB->CFSR);
            }
            if((SCB->CFSR & 0xFF) != 0) {
                printMemoryManagementErrorMsg(SCB->CFSR);
            }
        }
        stackDump(stack);
        __ASM volatile("BKPT #01");
    //}

    //sys_task_list(buffer);
    while(1);
}

#if defined ( __ICCARM__ )
void HardFault_Handler_asm(void)
{
    __asm(/*"PUBLIC Hard_Fault_Handler\n"*/
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B Hard_Fault_Handler");
}
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
void HardFault_Handler_asm(void)
{
    __asm volatile
    (
    "    TST lr, #4                \n"
    "    ITE EQ                    \n"
    "    MRSEQ r0, MSP            \n"
    "    MRSNE r0, PSP            \n"
    "    B Hard_Fault_Handler    \n"
    );
}
#else
__ASM void HardFault_Handler_asm(void)
{
    IMPORT Hard_Fault_Handler

    TST lr, #4
    ITE EQ
    MRSEQ r0, MSP
    MRSNE r0, PSP
    B Hard_Fault_Handler
}
#endif
/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    HardFault_Handler_asm();
}
#endif
#else
#ifndef SYS_HARDFAULT_HANDLER
void HardFault_Handler_c(unsigned int stack[])
{
    printf("[Hard Fault Handler]\r\n");
    printf("    R0 = 0x%08X\r\n", stack[0]);
    printf("    R1 = 0x%08X\r\n", stack[1]);
    printf("    R2 = 0x%08X\r\n", stack[2]);
    printf("    R3 = 0x%08X\r\n", stack[3]);
    printf("    R12 = 0x%08X\r\n", stack[4]);
    printf("    LR = 0x%08X\r\n", stack[5]);
    printf("    PC = 0x%08X\r\n", stack[6]);
    printf("    PSR = 0x%08X\r\n", stack[7]);
    printf("    BFAR = 0x%08X\r\n", SCB->BFAR);
    printf("    CFSR = 0x%08X\r\n", SCB->CFSR);
    printf("    HFSR = 0x%08X\r\n", SCB->HFSR);
    printf("    DFSR = 0x%08X\r\n", SCB->DFSR);
    printf("    AFSR = 0x%08X\r\n", SCB->AFSR);

    __ASM volatile("BKPT #01");

    while(1);
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    __asm(/*"PUBLIC Hard_Fault_Handler\n"*/
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B HardFault_Handler_c");
}
#endif
#endif
/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    printf("\r\nIn MemManage Handler\r\n");
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    printf("\r\nIn BusFault Handler\r\n");
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    printf("\r\nIn UsageFault Handler\r\n");
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

#ifndef SYS_SVC_HANDLER
/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}
#endif

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

#ifndef SYS_PENDSV_HANDLER
/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}
#endif

#ifndef SYS_SYSTICK_HANDLER
/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    led_spark();
    delay_decrement();
}
#endif

/*!
    \brief      this function handles USART0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{
    sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    log_uart_interrupt_handler();

    sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles USART1 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART1_IRQHandler(void)
{
    sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    log_uart_interrupt_handler();

    sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles USART2 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART2_IRQHandler(void)
{
    sys_int_enter();                            /* Tell the OS that we are starting an ISR            */

    log_uart_interrupt_handler();

    sys_int_exit();                             /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles EXTI5_9 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
#if CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    deep_sleep_exit();

    printf("WAKEUP For Console, Input Any Command or Press 'Enter' Key to Deep Sleep\r\n#\r\n");
    usart_command_enable(LOG_UART,USART_CMD_RXFCMD);
    sys_wakelock_acquire(LOCK_ID_USART);
#endif
}

/*!
    \brief      this function handles EXTI10_15 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI10_15_IRQHandler(void)
{
#if CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    deep_sleep_exit();

    printf("WAKEUP For Console, Input Any Command or Press 'Enter' Key to Deep Sleep\r\n#\r\n");
    usart_command_enable(LOG_UART,USART_CMD_RXFCMD);
    sys_wakelock_acquire(LOCK_ID_USART);
#endif
}

/*!
    \brief      this function handles WLAN WKUP exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void WLAN_WKUP_IRQHandler(void)
{
#if CONFIG_PLATFORM == PLATFORM_ASIC_32W51X

    deep_sleep_exit();

    wlan_ps_wakeup_isr();
#endif
}

/*!
    \brief      this function handles RTC WKUP exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_WKUP_IRQHandler(void)
{
#if CONFIG_PLATFORM == PLATFORM_ASIC_32W51X
    deep_sleep_exit();
#endif
}

/*!
    \brief      this function handles WLAN Rx exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void WLAN_Rx_IRQHandler(void) SECTION_RAM_CODE
{
    sys_int_enter();                             /* Tell the OS that we are starting an ISR            */

    wlan_interrupt_rx_handler();

    sys_int_exit();                              /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles WLAN Tx exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void WLAN_Tx_IRQHandler(void)
{
    sys_int_enter();                             /* Tell the OS that we are starting an ISR            */

    wlan_interrupt_tx_handler();

    sys_int_exit();                              /* Tell the OS that we are leaving the ISR            */
}

/*!
    \brief      this function handles WLAN common exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void WLAN_Cmn_IRQHandler(void)
{
    sys_int_enter();                             /* Tell the OS that we are starting an ISR            */

    wlan_interrupt_others_handler();

    sys_int_exit();                              /* Tell the OS that we are leaving the ISR            */
}
