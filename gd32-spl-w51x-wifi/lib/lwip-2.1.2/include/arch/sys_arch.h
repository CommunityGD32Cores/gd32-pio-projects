#ifndef _SYS_ARCH_H
#define _SYS_ARCH_H

#include "app_cfg.h"
#include "wrapper_os.h"
#include "arch/cc.h"

#define LWIP_TASK_STK_POOL_SIZE                512     /* 256 */
#if defined(PLATFORM_OS_UCOS) || defined(PLATFORM_OS_LITEOS)
#define LWIP_TASK_TCPIP_PRIO                   TASK_PRIO_APP_BASE
#elif defined(PLATFORM_OS_FREERTOS) || defined(PLATFORM_OS_AOS_RHINO) || defined(PLATFORM_OS_RTTHREAD)
#define LWIP_TASK_TCPIP_PRIO                   (TASK_PRIO_APP_BASE + TASK_PRIO_HIGHER(1))
#endif

#define LWIP_TASK_MAX                          1

extern os_task_t lwip_task_tcb[LWIP_TASK_MAX];
extern uint32_t lwip_task_stk[LWIP_TASK_STK_POOL_SIZE] __ALIGNED(8);

typedef os_sema_t           sys_sem_t;
typedef os_queue_t          sys_mbox_t;
typedef uint8_t             sys_thread_t;

#define SYS_MBOX_NULL       (void *)0
#define SYS_SEM_NULL        (void *)0

#define sys_sem_valid(sem)             (*((os_sema_t *)sem) != NULL)
#define sys_sem_set_invalid(sem)       (*((os_sema_t *)sem)  = NULL)
#define sys_mbox_valid(mbox)           (*((os_queue_t *)mbox) != NULL)
#define sys_mbox_set_invalid(mbox)     (*((os_queue_t *)mbox) = NULL)

#define sys_now             sys_current_time_get
#define sys_jiffies         (sys_now * OS_TICK_RATE_HZ / 1000)
#define sys_msleep          sys_ms_sleep

#define sys_sem_free(s)     sys_sema_free(s)
#define sys_sem_signal(s)   sys_sema_up(s)

#define sys_mbox_free(m)    sys_queue_free(m)

extern void sys_ms_sleep(uint32_t ms);


#endif
