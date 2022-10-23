/*!
    \file    wrapper_freertos.h
    \brief   FreeRTOS wrapper for GD32W51x WiFi SDK

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

#ifndef __WRAPPER_FREERTOS_H
#define __WRAPPER_FREERTOS_H

/*============================ INCLUDES ======================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "list.h"
#include "app_type.h"

/*============================ MACROS ========================================*/
/* Enable them if use OS's provided system ISR implementations */
/* #define SYS_HARDFAULT_HANDLER */
#define SYS_SVC_HANDLER
#define SYS_PENDSV_HANDLER
#define SYS_SYSTICK_HANDLER

/* Priority: 0 ~ 56 (configMAX_PRIORITIES).
 * The priority of idle task is 0. */
#define TASK_PRIO_APP_BASE            16  // 24
#define TASK_PRIO_WIFI_BASE           16  // 24
#define TASK_PRIO_IDLE                (tskIDLE_PRIORITY)

#define OS_TICK_RATE_HZ               configTICK_RATE_HZ
#define OS_MS_PER_TICK                portTICK_PERIOD_MS

/*============================ MACRO FUNCTIONS ===============================*/
#define TASK_PRIO_HIGHER(n)           (n)
#define TASK_PRIO_LOWER(n)            (-n)

/* FreeRTOS uses an internal variable to record the nesting, so we no longer need a variable here */
#define SYS_SR_ALLOC()
/*
 * Disable interrupts (nested), only appliable from task context
 * FreeRTOS only guarantees ISR interrupt safe with priority lower than configMAX_SYSCALL_INTERRUPT_PRIORITY
 */
#define SYS_CRITICAL_ENTER()        taskENTER_CRITICAL()
/* Enable interrupts (nested), only appliable from task context */
#define SYS_CRITICAL_EXIT()         taskEXIT_CRITICAL()

/* OS IRQ service hook called just after the ISR starts */
#define sys_int_enter()
/* OS IRQ service hook called before the ISR exits */
#define sys_int_exit()

/*============================ TYPES =========================================*/
/*
 * All wrapped new types are pointers to hold dynamically allocated
 * objects except for wrapped task type which we will use for static objects,
 * so keep it as a struct type.
 * In principle, we should nullify all pointers after they are
 * freed or deleted to make them available for sys_xxx_valid macros.
 */
typedef void *os_sema_t;
typedef void *os_mutex_t;
typedef StaticTask_t os_task_t;
typedef void *os_timer_t;
typedef void *os_queue_t;

typedef void (*task_func_t)(void *argv);
typedef void (*timer_func_t)(void *p_tmr, void *p_arg);

typedef struct timer_context {
    void *p_arg;
    timer_func_t timer_func;
} os_timer_context_t;

typedef struct task_wrapper {
    TaskHandle_t task_handle;
    os_queue_t task_queue;
} task_wrapper_t;

/*============================ GLOBAL VARIABLES ==============================*/
#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN uint32_t os_idle_task_stk[configMINIMAL_STACK_SIZE] __ALIGNED(8);
EXTERN uint32_t os_timer_task_stk[configTIMER_TASK_STACK_DEPTH] __ALIGNED(8);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#endif //#ifndef __WRAPPER_FREERTOS_H
