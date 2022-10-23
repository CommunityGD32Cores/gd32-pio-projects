/*!
    \file    osal_types.h
    \brief   OSAL types definition for GD32W51x WiFi driver

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

#ifndef __OSAL_TYPES_H_
#define __OSAL_TYPES_H_

#include "app_type.h"
#include "bsp_wlan.h"
#include "dlist.h"


#if defined ( __ICCARM__ )
#define __inline__             inline
#define __inline               inline
#define __inline_definition
#elif defined ( __CC_ARM   )
#define __inline__             __inline
#define inline                 __inline
#define __inline_definition
#elif defined   (  __GNUC__  )
#define __inline__             __inline__
#define __inline               __inline__
#define __inline_definition    __inline__
#endif

#define _SUCCESS     1
#define _FAIL        0

#undef _TRUE
#define _TRUE        1

#undef _FALSE
#define _FALSE       0

#undef TRUE
#define TRUE         1

#undef FALSE
#define FALSE        0

#undef NULL
#define NULL         0

#ifndef BITS
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
#define REG32(addr)                  (*(volatile uint32_t *)(uint32_t)(addr))
#define REG16(addr)                  (*(volatile uint16_t *)(uint32_t)(addr))
#define REG8(addr)                   (*(volatile uint8_t *)(uint32_t)(addr))
#define BIT(x)                       ((uint32_t)((uint32_t)0x01U << (x)))
#define GET_BITS(regval, start, end) (((regval) & BITS((start), (end))) >> (start))
#endif

#ifndef BIT0
#define BIT0        0x00000001
#define BIT1        0x00000002
#define BIT2        0x00000004
#define BIT3        0x00000008
#define BIT4        0x00000010
#define BIT5        0x00000020
#define BIT6        0x00000040
#define BIT7        0x00000080
#define BIT8        0x00000100
#define BIT9        0x00000200
#define BIT10       0x00000400
#define BIT11       0x00000800
#define BIT12       0x00001000
#define BIT13       0x00002000
#define BIT14       0x00004000
#define BIT15       0x00008000
#define BIT16       0x00010000
#define BIT17       0x00020000
#define BIT18       0x00040000
#define BIT19       0x00080000
#define BIT20       0x00100000
#define BIT21       0x00200000
#define BIT22       0x00400000
#define BIT23       0x00800000
#define BIT24       0x01000000
#define BIT25       0x02000000
#define BIT26       0x04000000
#define BIT27       0x08000000
#define BIT28       0x10000000
#define BIT29       0x20000000
#define BIT30       0x40000000
#define BIT31       0x80000000
#endif
#ifndef ETH_ALEN
#define ETH_ALEN    6
#endif
#define jiffies     0

#define FIELD_OFFSET(type, field)           ((SSIZE_T)(&(((type*)0)->field)))
#define LIST_CONTAINOR(ptr, type, member)   list_entry(ptr, type, member)
#define TIME_AFTER(a, b)                    ((long)(b) - (long)(a) < 0)

/* SSAT returns val saturated to the signed range -2^(sat - 1) <= x <= 2^(sat - 1) - 1
 * USAT returns val saturated to the unsigned range 0 <= x <= 2^sat - 1 */
#if defined (__ICCARM__)
#define SSAT        __SSAT
#define USAT        __USAT
#elif defined (__CC_ARM)
#define SSAT        __ssat
#define USAT        __usat
#else
#define SSAT(val, sat)        (val > ((1 << (sat - 1)) - 1)) ? ((1 << (sat - 1)) - 1) : (val < -(1 << (sat - 1)) ? -(1 << (sat - 1)) : val)
#define USAT(val, sat)        (val > ((1 << sat) - 1)) ? ((1 << sat) - 1) : (val < 0 ? 0 : val)
#endif

#define SBFX(val, bits)       ((int32_t)(val << (32 - bits)) >> (32 - bits))


typedef uint8_t boolean;
//typedef uint64_t uint64_t;
//typedef uint32_t uint32_t;
//typedef uint16_t uint16_t;
//typedef uint8_t uint8_t;
//typedef int64_t int64_t;
//typedef int32_t int32_t;
//typedef int16_t int16_t;
//typedef int8_t int8_t;
typedef uint32_t SIZE_T;
typedef int32_t SSIZE_T;
typedef SIZE_T SIZE_PTR;


/**
 * @brief dlist struct
 */
typedef struct list_head _list;

/**
 * @brief semaphore/mutex/lock struct
 */
typedef void *_sema;
typedef void *_mutex;
typedef void *_lock;

/**
 * @brief task struct which encapsulate OS Task
 */
typedef void *_task;
typedef void (*wlan_task_func_t)(void *argv);

/**
 * @brief timer struct which encapsulate OS timer
 */
typedef void *_timer;
typedef void (*wlan_timer_func_t)(void *p_tmr, void *p_arg);
/**
 * @brief queue struct which includes list and lock
 */
typedef struct __queue {
    struct list_head queue;
    _lock lock;
} _queue;

typedef void (*work_func_t)(void *context, uint8_t *data);

/**
 * @brief workqueue structure like linux style
 */
struct workqueue_struct {
    char task_name[16];
    _task task_tcb;
    uint8_t blocked;
    _sema task_exit_sema;
};

/**
 * @brief work struct which represent one "work"
 */
struct work_struct {
    void *data;
    work_func_t func;
    void *context;
    struct workqueue_struct *used_wq;
};

/**
 * @brief wlan iw point struct like in linux wext
 */
struct wlan_iw_point {
    void *pointer;
    uint16_t length;
    uint16_t flags;
};

#endif
