/*!
    \file    wrapper_freertos.c
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

#include <debug_print.h>
#include <malloc.h>
#include <delay.h>

static os_task_t os_idle_task_tcb;
static os_task_t os_timer_task_tcb;

/***************** heap management implementation *****************/
/*!
    \brief      allocate a block of memory with a minimum of 'size' bytes.
    \param[in]  size: the minimum size of the requested block in bytes
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_malloc(size_t size)
{
    return pvSysMalloc(size);
}

/*!
    \brief      allocate a certian chunks of memory with specified size
                Note: The allocated memory is filled with bytes of value zero.
                All chunks in the allocated memory are contiguous.
    \param[in]  count: multiple number of size want to malloc
    \param[in]  size:  number of size want to malloc
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_calloc(size_t count, size_t size)
{
    void *mem_ptr;

    mem_ptr = pvSysMalloc(count*size);
    sys_memset(mem_ptr, 0, (count*size));

    return mem_ptr;
}

/*!
    \brief      change the size of a previously allocated memory block.
    \param[in]  mem: address to the old buffer
    \param[in]  size: number of the new buffer size
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_realloc(void *mem, size_t size)
{
    return pvSysReAlloc(mem, size);
}

/*!
    \brief      free a memory to the heap
    \param[in]  ptr: pointer to the address want to free
    \param[out] none
    \retval     none
*/
void sys_mfree(void *ptr)
{
    vSysFree(ptr);
}

/*!
    \brief      get system free heap size
    \param[in]  none
    \param[out] none
    \retval     system free heap size value(0x00000000-0xffffffff)
*/
int32_t sys_free_heap_size(void)
{
    return xSysGetFreeHeapSize();
}

/*!
    \brief      get minimum free heap size that has been reached
    \param[in]  none
    \param[out] none
    \retval     system minimum free heap size value(0x00000000-0xffffffff)
*/
int32_t sys_min_free_heap_size(void)
{
    return xSysGetMinimumEverFreeHeapSize();
}

/*!
    \brief      get system least heap block size
                Note: it's used to count all memory used by heap and heap management structures.
                    A least block is usually consist of a heap block header and a minimum block.
                    However heap allocation may not align the allocated heap area with this minimum block,
                    the counting results may be inaccurate.
    \param[in]  none
    \param[out] none
    \retval     system heap block size value in bytes(0x0000-0xffff)
*/
uint16_t sys_heap_block_size(void)
{
    return xSysGetHeapMinimumBlockSize();
}

/***************** memory manipulation *****************/
extern void Mem_Copy(void *des, const void *src, uint32_t n);

/*!
    \brief      copy buffer content from source address to destination address.
    \param[in]  src: the address of source buffer
    \param[in]  n: the length to copy
    \param[out] des: the address of destination buffer
    \retval     none
*/
void sys_memcpy(void *des, const void *src, uint32_t n)
{
#if 0
    uint8_t *xdes = (uint8_t *)des;
    uint8_t *xsrc = (uint8_t *)src;

    while (n--) {
        *xdes++ = *xsrc++;
    }
#else
    Mem_Copy(des, src, n);
#endif
}

/*!
    \brief      move buffer content from source address to destination address
                Note: It could work between two overlapped buffers.
    \param[in]  src: the address of source buffer
    \param[in]  n: the length to move
    \param[out] des: the address of destination buffer
    \retval     none
*/
void sys_memmove(void *des, const void *src, uint32_t n)
{
    char *tmp = (char *)des;
    char *s = (char *)src;

    if (s < tmp && tmp < s + n) {
        tmp += n;
        s += n;

        while (n--)
            *(--tmp) = *(--s);
    } else {
        while (n--)
            *tmp++ = *s++;
    }
}

/*!
    \brief      set the content of the buffer to specified value
    \param[in]  s: The address of a buffer
    \param[in]  c: the value want to memset
    \param[in]  count: count value want to memset
    \param[out] none
    \retval     none
*/
void sys_memset(void *s, uint8_t c, uint32_t count) SECTION_RAM_CODE
{
    uint32_t dword_cnt;
    uint32_t dword_value;
    uint8_t *p_dst = (uint8_t *)s;

    while (((uint32_t)p_dst & 0x03) != 0) {
        if (count == 0) {
            return;
        }
        *p_dst++ = c;
        count--;
    }

    dword_cnt = (count >> 2);
    count &= 0x03;
    dword_value = (((uint32_t)(c) << 0) & 0x000000FF) | (((uint32_t)(c) << 8) & 0x0000FF00) |
                    (((uint32_t)(c) << 16) & 0x00FF0000) | (((uint32_t)(c) << 24) & 0xFF000000);

    while (dword_cnt > 0u) {
        *(uint32_t *)p_dst = dword_value;
        p_dst += 4;
        dword_cnt--;
    }

    while (count > 0u) {
        *p_dst++ = c;
        count--;
    }
}

/*!
    \brief      compare two buffers
    \param[in]  buf1: address to the source buffer 1
    \param[in]  buf2: address to the source buffer 2
    \param[in]  count: the compared buffer size in bytes
    \param[out] none
    \retval      0 if buf1 equals buf2, non-zero otherwise.
*/
int32_t sys_memcmp(const void *buf1, const void *buf2, uint32_t count)
{
    if(!count)
        return 0;

    while (--count && *((char *)buf1) == *((char *)buf2)) {
        buf1 = (char *)buf1 + 1;
        buf2 = (char *)buf2 + 1;
    }

    return (*((char *)buf1) - *((char *)buf2));
}

/***************** OS API wrappers *****************/
/*!
    \brief      create a task wrapping a task handle and a message queue
                Note:  Message queue wrapped in the task_wrapper_t is created to only hold pointers,
                  therefore the objects pointed by these pointers are not thread safe. You need to protect
                  them for some scenarios.
    \param[in]  static_tcb: pointer to the task buffer
    \param[in]  name: the task's name
    \param[in]  stack_base: the task's stack base address
    \param[in]  stack_size: the task's stack size, in words (4 bytes)
    \param[in]  queue_size: the task's message queue size
    \param[in]  priority: the task's priority
    \param[in]  func: the task's entry function
    \param[in]  ctx: the task's parameter
    \param[out] none
    \retval     the task handle if succeeded, NULL otherwise.
*/
void *sys_task_create(void *static_tcb, const uint8_t *name, uint32_t *stack_base, uint32_t stack_size,
                    uint32_t queue_size, uint32_t priority, task_func_t func, void *ctx)
{
    void *result = NULL;
    task_wrapper_t *task_wrapper = NULL;
    TaskHandle_t task_handle;

    if(queue_size > 0) {
        task_wrapper = (task_wrapper_t *)sys_malloc(sizeof(task_wrapper_t));
        if (task_wrapper == NULL) {
            DEBUGPRINT("sys_task_create, malloc wrapper failed\r\n");
            return NULL;
        }

        task_wrapper->task_queue = xQueueCreate(queue_size, sizeof(void *));
        if (task_wrapper->task_queue == NULL) {
            DEBUGPRINT("sys_task_createtask, create task queue failed\r\n");
            sys_mfree(task_wrapper);
            return NULL;
        }
    }

    /* protect task creation and task wrapper pointer storing against inconsistency of them if preempted in the middle */
    vTaskSuspendAll();
    if (static_tcb != NULL && stack_base != NULL) {
        if ((task_handle = xTaskCreateStatic(func, (const char *)name, stack_size, ctx, priority,
                        (StackType_t *)stack_base, (StaticTask_t *)static_tcb)) != NULL) {
            result = task_handle;
        }
    } else {
        if (xTaskCreate(func, (const char *)name, stack_size, ctx, priority, &task_handle) == pdPASS) {
            result = task_handle;
        }
    }

    if (result == NULL) {
        DEBUGPRINT("sys_task_create, return failed\r\n");
        xTaskResumeAll();
        goto exit;
    }

    if (queue_size > 0) {
        task_wrapper->task_handle = task_handle;
    }
    vTaskSetThreadLocalStoragePointer(task_handle, 0, task_wrapper);
    xTaskResumeAll();

    return result;

exit:
    if (queue_size > 0) {
        vQueueDelete(task_wrapper->task_queue);
        sys_mfree(task_wrapper);
    }

    return NULL;
}

/*!
    \brief      delete a task and free all associated resources
                Note: Message queue wrapped in the task_wrapper_t is created to only hold pointers,
                  therefore the objects pointed by these pointers are not thread safe. You need to protect
                  them for some scenarios.
    \param[in]  task: pointer to the task handle, delete the current task if it's NULL
    \param[out] none
    \retval     none
*/
void sys_task_delete(void *task)
{
    TaskHandle_t task_handle = (TaskHandle_t)task;
    task_wrapper_t *task_wrapper;

    if (task == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }
    task_wrapper = (task_wrapper_t *)pvTaskGetThreadLocalStoragePointer(task_handle, 0);

    /* if task is deleted by another task, delete task first, then free task_wrapper. Otherwise, when
        task_wrapper is freed but task is still alive, task_wrapper pointer got by task is invalid.
    */
    if (task != NULL) {
        vTaskDelete(task_handle);
    }

    if (task_wrapper != NULL) {
        vQueueDelete(task_wrapper->task_queue);
        sys_mfree(task_wrapper);
    }

    if (task == NULL) {
        vTaskDelete(NULL);
    }
}

/*!
    \brief      wait for a message from the message queue wrapped in the current task
    \param[in]  timeout_ms: millisecond timeout
                            0 means forever,
                            if the time is less than a tick, will roll up to 1 tick
    \param[out] msg_ptr: pointer to the message
    \retval     function run status
      \arg        OS_ERROR: sys_task_wait, task wrapper is NULL
      \arg        OS_TIMEOUT: get message timeout
      \arg        OS_OK: run success
*/
int32_t sys_task_wait(uint32_t timeout_ms, void *msg_ptr)
{
    TaskHandle_t task_handle;
    task_wrapper_t *task_wrapper;
    int32_t result;

    task_handle = xTaskGetCurrentTaskHandle();

    task_wrapper = (task_wrapper_t *)pvTaskGetThreadLocalStoragePointer(task_handle, 0);
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_wait, task wrapper is NULL\r\n");
        return OS_ERROR;
    }

    result = sys_queue_fetch(&task_wrapper->task_queue, msg_ptr, timeout_ms, 1);
    if (result != 0) {
        return OS_TIMEOUT;
    }

    return OS_OK;
}

/*!
    \brief      post a message to the message queue wrapped in the target task
    \param[in]  receiver_task: pointer to the message receiver task handle
    \param[in]  msg_ptr: pointer to the message
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                    with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: have error happen
      \arg        OS_OK: run success
*/
int32_t sys_task_post(void *receiver_task, void *msg_ptr, uint8_t from_isr)
{
    TaskHandle_t task_handle = (TaskHandle_t)receiver_task;
    task_wrapper_t *task_wrapper;
    portBASE_TYPE HigherPriorityTaskWoken = pdFALSE;

    task_wrapper = (task_wrapper_t *)pvTaskGetThreadLocalStoragePointer(task_handle, 0);
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_post, task wrapper is NULL\r\n");
        return OS_ERROR;
    }

    if (xQueueIsQueueFullFromISR(task_wrapper->task_queue) == pdTRUE) {
        DEBUGPRINT("sys_task_post: queue full, task is %s\r\n", pcTaskGetName(task_handle));
    }

    if (from_isr) {
        if (xQueueSendFromISR(task_wrapper->task_queue, msg_ptr, &HigherPriorityTaskWoken) != pdPASS) {
            DEBUGPRINT("sys_task_post from isr: send fail, return error\r\n");
            return OS_ERROR;
        }
        if (HigherPriorityTaskWoken != pdFALSE) {
            taskYIELD();
        }
    } else {
        if (xQueueSend(task_wrapper->task_queue, msg_ptr, 0) != pdPASS) {
            DEBUGPRINT("sys_task_post: send fail, return error\r\n");
            return OS_ERROR;
        }
    }

    return OS_OK;
}

/*!
    \brief      flush all messages of the message queue wrapped in the target task
                Note: All messages will be recycled to the free queue and the task will be resumed to
                  run after the operation, since we are queuing pointers in the task wrapped message queue,
                  do not to forget to handle all these pointers well to avoid memory leak.
    \param[in]  task: pointer to the task handle
    \param[out] none
    \retval     none
*/
void sys_task_msg_flush(void *task)
{
    TaskHandle_t task_handle = (TaskHandle_t)task;
    task_wrapper_t *task_wrapper;

    if (task == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_wrapper = (task_wrapper_t *)pvTaskGetThreadLocalStoragePointer(task_handle, 0);
    if (task_wrapper != NULL) {
        xQueueReset(task_wrapper->task_queue);
    }
}

/*!
    \brief      get the number of waiting messages in the task queue
    \param[in]  task: pointer to the task handle
    \param[in]  from_isr: whether is from ISR
    \param[out] none
    \retval     the number of waiting messages if succeeded, OS_ERROR otherwise.
*/
int32_t sys_task_msg_num(void *task, uint8_t from_isr)
{
    TaskHandle_t task_handle = (TaskHandle_t)task;
    task_wrapper_t *task_wrapper;
    uint32_t msg_waiting = 0;

    if (task == NULL) {
        task_handle = xTaskGetCurrentTaskHandle();
    }

    task_wrapper = (task_wrapper_t *)pvTaskGetThreadLocalStoragePointer(task_handle, 0);
    if (task_wrapper == NULL) {
        DEBUGPRINT("sys_task_msg_num, task wrapper is NULL\r\n");
        return OS_ERROR;
    }

    if (from_isr)
        msg_waiting = uxQueueMessagesWaitingFromISR(task_wrapper->task_queue);
    else
        msg_waiting = uxQueueMessagesWaiting(task_wrapper->task_queue);

    return msg_waiting;
}

/*!
    \brief     get the handle of the OS idle task
    \param[in]  none
    \param[out] none
    \retval     the pointer to task handle
*/
os_task_t *sys_idle_task_handle_get(void)
{
    return &os_idle_task_tcb;
}

/*!
    \brief      get the handle of the OS timer task
    \param[in]  none
    \param[out] none
    \retval     the pointer to task handle
*/
os_task_t *sys_timer_task_handle_get(void)
{
    return &os_timer_task_tcb;
}

/*!
    \brief      get the free stack size of the target task
    \param[in]  task: the pointer to the task handle
    \param[out] none
    \retval     the free stack size in words (4 bytes)(0x00000000-0xffffffff)
*/
uint32_t sys_stack_free_get(void *task)
{
#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
    return uxTaskGetStackHighWaterMark((TaskHandle_t)task);
#else
    DEBUGPRINT("sys_stack_free_get, INCLUDE_uxTaskGetStackHighWaterMark should defined to 1\r\n");
    return 0;
#endif
}

/*!
    \brief      list statistics for all tasks
    \param[in]  none
    \param[out] pwrite_buf: pointer to the result buffer, expect a statistics string
    \retval     none
*/
void sys_task_list(char *pwrite_buf)
{
#if (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
    vTaskList(pwrite_buf);
#endif
}

/*!
    \brief      create and initialize a semaphore
    \param[in]  sema: the pointer to the semaphore handle
    \param[in]  init_val: initialize vlaue of semaphore
    \param[out] sema: pointer to the semaphore handle
    \retval     0 if succeeded, -1 otherwise.
*/
int32_t sys_sema_init(os_sema_t *sema, int32_t init_val)
{
    *sema = xSemaphoreCreateCounting(0xffffffff, init_val);
    DEBUG_ASSERT(*sema != NULL);
    return OS_OK;
}

/*!
    \brief      free a semaphore
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_free(os_sema_t *sema)
{
    if (*sema == NULL) {
        DEBUGPRINT("sys_sema_free, sema = NULL\r\n");
        return;
    }

    vSemaphoreDelete(*sema);
    *sema = NULL;
}

/*!
    \brief      release a semaphore
                Note:  It could be called either in task context or interrupt context except for some OS
                  that has seperated implementations for the different context, like FreeRTOS
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_up(os_sema_t *sema)
{
    xSemaphoreGive(*sema);
}

/*!
    \brief      release a semaphore in a interrupt context, it's only meaningful to some OS
                with implementations seperated for different execution context, like FreeRTOS
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_up_from_isr(os_sema_t *sema)
{
    portBASE_TYPE taskWoken = pdFALSE;

    xSemaphoreGiveFromISR(*sema, &taskWoken);
    portEND_SWITCHING_ISR(taskWoken);
}

/*void sys_post_sema(os_sema_t *sema)
{
    xSemaphoreGive(*sema);
}
*/

/*!
    \brief      Require the semaphore within a given time constraint
    \param[in]  sema: pointer to the semaphore handle
    \param[in]  timeout_ms: millisecond value of timeout,
                    0 means forever,
                    if the time is less than a tick, will roll up to 1 tick.
    \param[out] none
    \retval     function run status
      \arg        OS_TIMEOUT: timeout
      \arg        OS_OK: run success
*/
int32_t sys_sema_down(os_sema_t *sema, uint32_t timeout_ms)
{
    uint32_t timeout_tick;
    BaseType_t result;

    if (timeout_ms == 0) {
        timeout_tick = portMAX_DELAY;
    } else {
        timeout_tick = (timeout_ms / OS_MS_PER_TICK);
        if (timeout_tick == 0) {
            timeout_tick = 1;
        }
    }

    result = xSemaphoreTake(*sema, timeout_tick);
    if(result != pdTRUE) {
        if (result == errQUEUE_EMPTY) {
            //DEBUGPRINT("sys_sema_down, queue empty\r\n");
        } else {
            DEBUGPRINT("sys_sema_down, return error\r\n");
        }
        return OS_TIMEOUT;
    }

    return OS_OK;
}

/*!
    \brief      create and initialize a mutex
    \param[in]  mutex: pointer to the mutext handle
    \param[out] none
    \retval     none
*/
void sys_mutex_init(os_mutex_t *mutex)
{
    *mutex = xSemaphoreCreateMutex();
    DEBUG_ASSERT(*mutex != NULL);
}

/*!
    \brief      free a mutex
    \param[in]  mutex: pointer to the mutex handle
    \param[out] none
    \retval     none
*/
void sys_mutex_free(os_mutex_t *mutex)
{
    if (*mutex == NULL) {
        DEBUGPRINT("sys_mutex_free, mutex = NULL\r\n");
        return;
    }

    vSemaphoreDelete(*mutex);
    *mutex = NULL;
}

/*!
    \brief      require the mutex
    \param[in]  mutex: pointer to the mutex handle
    \param[out] none
    \retval     none
*/
void sys_mutex_get(os_mutex_t *mutex)
{
    while (xSemaphoreTake(*mutex, 60 * 1000 / OS_MS_PER_TICK) != pdTRUE) {
        DEBUGPRINT("[%s] get mutex 0x%08x failed, retry\r\n", pcTaskGetTaskName(NULL), *mutex);
    }
}

/*!
    \brief      release a mutex
                Note: It could be only called in task context for some OS
                  that has seperated implementations for the different context, like FreeRTOS
    \param[in]  mutex: pointer to the mutext
    \param[out] none
    \retval     none
*/
void sys_mutex_put(os_mutex_t *mutex)
{
    xSemaphoreGive(*mutex);
}

/*!
    \brief      create and initialize a message queue
    \param[in]  queue_size: queue size
    \param[in]  item_size: queue item size
    \param[out] queue: pointer to the queue
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_queue_init(os_queue_t *queue, int32_t queue_size, uint32_t item_size)
{
    if ((*queue = xQueueCreate(queue_size, item_size)) == NULL) {
        DEBUGPRINT("sys_queue_init, return error\r\n");
        return OS_ERROR;
    }

    return OS_OK;
}

/*!
    \brief      free a message queue
    \param[in]  queue: pointer to the queue
    \param[out] none
    \retval     none
*/
void sys_queue_free(os_queue_t *queue)
{
    vQueueDelete(*queue);
    *queue = NULL;
}

/*!
    \brief      post a message to the target message queue
    \param[in]  queue: pointer to the queue handle
    \param[in]  msg: pointer to the message
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_queue_post(os_queue_t *queue, void *msg)
{
    if (xQueueSend(*queue, msg, 0) != pdPASS) {
        DEBUGPRINT("sys_queue_post, return error (%s)\r\n", pcTaskGetName(NULL));
        return OS_ERROR;
    }

    return OS_OK;
}

/*!
    \brief      fetch a message from the message queue within a given time constraint
    \param[in]  queue: pointer to the queue handle
    \param[in]  timeout_ms: timeout in ms. For blocking operation, 0 means forever, if the time is
                    less than a tick, will roll up to 1 tick; for non-blocking operation, it's unused.
    \param[in]  is_blocking: whether it's a blocking operation
    \param[out] msg: pointer to the message
    \retval     function run status
      \arg        OS_TIMEOUT: timeout
      \arg        OS_OK: run success
*/
int32_t sys_queue_fetch(os_queue_t *queue, void *msg, uint32_t timeout_ms, uint8_t is_blocking)
{
    uint32_t timeout_tick;

    if (!is_blocking) {
        timeout_tick = 0;
    } else if (is_blocking && (timeout_ms == 0)) {
        timeout_tick = portMAX_DELAY;
    } else {
        timeout_tick = (timeout_ms / OS_MS_PER_TICK);
        if ((timeout_ms != 0) && (timeout_tick == 0)) {
            timeout_tick = 1;
        }
    }

    if (xQueueReceive(*queue, msg, timeout_tick) != pdPASS) {
        return OS_TIMEOUT;
    }

    return OS_OK;
}

/*!
    \brief      get the current system up time
    \param[in]  none
    \param[out] none
    \retval     milliseconds since the system boots up(0x00000000-0xffffffff)
*/
uint32_t sys_current_time_get(void)
{
    return (uint32_t)(xTaskGetTickCount() * OS_MS_PER_TICK);
}

/*!
    \brief      get the system time
    \param[in]  p: not used
    \param[out] none
    \retval     milliseconds since the system boots up(0x00000000-0xffffffff)
*/
uint32_t sys_time_get(void *p)
{
    return sys_current_time_get();
}

/*!
    \brief      put current task to sleep for a specified period of time
    \param[in]  ms: millisecond value, if the time is less than a tick, will roll up to 1 tick.
    \param[out] none
    \retval     none
*/
void sys_ms_sleep(uint32_t ms)
{
    uint32_t tick = ms / OS_MS_PER_TICK;

    if (tick == 0) {
        tick = 1;
    }

    vTaskDelay(tick);
}

/*!
    \brief      delay the current task for several microseconds
                Note: The task will being blocked during the delayed time
    \param[in]  nus: microsecond value
    \param[out] none
    \retval     none
*/
void sys_us_delay(uint32_t nus)
{
    vTaskSuspendAll();
    systick_udelay(nus);
    xTaskResumeAll();
}

/*!
    \brief      give up the execution of the current task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_yield(void)
{
    taskYIELD();
}

/*!
    \brief      pend the task scheduling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_sched_lock(void)
{
    vTaskSuspendAll();
}

/*!
    \brief      resume the task scheduling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_sched_unlock(void)
{
    xTaskResumeAll();
}

/*!
    \brief      system generate random
    \param[in]  none
    \param[out] none
    \retval     random value(0x00000000-0xffffffff)
*/
static int32_t _arc4_random(void)
{
    uint32_t res = sys_current_time_get();
    uint32_t seed = seed_rand32();

    seed = ((seed & 0x007F00FF) << 7) ^
        ((seed & 0x0F80FF00) >> 8) ^
        (res << 13) ^ (res >> 9);

    return (int32_t)seed;
}

/*!
    \brief      Initialize the given buffer with the random data
    \param[in]  size: size of random data
    \param[out] dst: pointer to the get random bytes
    \retval     function run status
      \arg        OS_OK: run success
*/
int32_t sys_random_bytes_get(void *dst, uint32_t size)
{
    uint32_t ranbuf;
    uint32_t *lp;
    int32_t i, count;
    count = size / sizeof(uint32_t);
    lp = (uint32_t *)dst;

    for (i = 0; i < count; i++) {
        lp[i] = _arc4_random();
        size -= sizeof(uint32_t);
    }

    if(size > 0) {
        ranbuf = _arc4_random();
        sys_memcpy(&lp[i], &ranbuf, size);
    }

    return OS_OK;
}

/*!
    \brief      set system timer callback
    \param[in]  p_tmr:pointer to the timer callback
    \param[out] none
    \retval     none
*/
static void _sys_timer_callback(void *p_tmr)
{
    timer_func_t timer_func;
    os_timer_t timer = (os_timer_t)p_tmr;
    os_timer_context_t *timer_ctx = (os_timer_context_t *)pvTimerGetTimerID(timer);

    timer_func = timer_ctx->timer_func;
    if (timer_func == NULL) {
        DEBUGPRINT("_sys_timer_callback, timer func is NULL, return\r\n");
        return;
    }

    timer_func(p_tmr, timer_ctx->p_arg);
}

/*!
    \brief      initialize a timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  name: pointer to the timer name
    \param[in]  delay: the timeout in milliseconds
    \param[in]  periodic: whether it's periodic timer
    \param[in]  func: the timer call back function
    \param[in]  arg:the argument that will pass to the call back
    \param[out] none
    \retval     none
*/
void sys_timer_init(os_timer_t *timer, const uint8_t *name, uint32_t delay, uint8_t periodic, timer_func_t func, void *arg)
{
    os_timer_context_t *timer_ctx;

    timer_ctx = (os_timer_context_t *)sys_malloc(sizeof(os_timer_context_t));
    if (timer_ctx == NULL) {
        DEBUGPRINT("sys_timer_init, malloc timer context failed\r\n");
        return;
    }

    if ((*timer = xTimerCreate((const char *)name, (delay / OS_MS_PER_TICK), periodic, NULL, (TimerCallbackFunction_t)_sys_timer_callback)) == NULL) {
        sys_mfree(timer_ctx);
        DEBUGPRINT("sys_timer_init, return error\r\n");
        return;
    }

    timer_ctx->p_arg = arg;
    timer_ctx->timer_func = func;
    vTimerSetTimerID(*timer, (void *)timer_ctx);
}

/*!
    \brief      delete a timer
    \param[in]  timer: pointer to the timer handle
    \param[out] none
    \retval     none
*/
void sys_timer_delete(os_timer_t *timer)
{
    os_timer_context_t *timer_ctx = NULL;
    os_timer_t p_timer;

    if (*timer == NULL) {
        DEBUGPRINT("sys_timer_delete, timer = NULL\r\n");
        return;
    }

    p_timer = *timer;
    *timer = NULL;
    timer_ctx = (os_timer_context_t *)pvTimerGetTimerID(p_timer);

    if (xTimerDelete(p_timer, (configTIMER_MAX_BLOCK_TIME / OS_MS_PER_TICK)) != pdPASS) {
        DEBUGPRINT("sys_timer_delete, return error\r\n");
    }

    if (timer_ctx != NULL) {
        sys_mfree(timer_ctx);
    }
}

/*!
    \brief       start a timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                  with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     none
*/
void sys_timer_start(os_timer_t *timer, uint8_t from_isr)
{
    uint8_t result = 0;
    portBASE_TYPE HigherPriorityTaskWoken = pdFALSE;

    if (from_isr) {
        if (xTimerStartFromISR(*timer, &HigherPriorityTaskWoken) == pdPASS) {
            if (HigherPriorityTaskWoken != pdFALSE) {
                taskYIELD();
            }
            result = 1;
        }
    } else {
        if (xTimerStart(*timer, (configTIMER_MAX_BLOCK_TIME / OS_MS_PER_TICK)) == pdPASS) {
            result = 1;
        }
        }

    if (result == 0) {
        DEBUGPRINT("sys_timer_start (0x%08x) return fail, from_isr is %d\r\n", *timer, from_isr);
    }
}

/*!
    \brief      start the timer extension with a specified timeout
    \param[in]  timer: pointer to the timer handle
    \param[in]  delay: time want to delay
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                  with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     none
*/
void sys_timer_start_ext(os_timer_t *timer, uint32_t delay, uint8_t from_isr)
{
    uint32_t timer_ticks;
    uint8_t result = 0;
    portBASE_TYPE HigherPriorityTaskWoken = pdFALSE;

    if (delay <= OS_MS_PER_TICK) {
        timer_ticks = 1;
    } else {
        timer_ticks = delay / OS_MS_PER_TICK;
    }

    if (from_isr) {
        if (xTimerChangePeriodFromISR(*timer, timer_ticks, &HigherPriorityTaskWoken) == pdPASS) {
            if (HigherPriorityTaskWoken != pdFALSE) {
                taskYIELD();
            }
            result = 1;
        }
    } else {
        if (xTimerChangePeriod(*timer, timer_ticks, (configTIMER_MAX_BLOCK_TIME / OS_MS_PER_TICK)) == pdPASS) {
            result = 1;
        }
    }

    if (result == 0) {
        DEBUGPRINT("sys_timer_start_ext (0x%08x) return fail, from_isr is %d\r\n", *timer, from_isr);
    }
}

/*!
    \brief      stop the timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                      with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     function run state 1: run ok, 0: have error happen
*/
uint8_t sys_timer_stop(os_timer_t *timer, uint8_t from_isr)
{
    uint8_t result = 0;
    portBASE_TYPE HigherPriorityTaskWoken = pdFALSE;

    if (from_isr) {
        if (xTimerStopFromISR(*timer, &HigherPriorityTaskWoken) == pdPASS) {
            if (HigherPriorityTaskWoken != pdFALSE) {
                taskYIELD();
            }
            result = 1;
        }
    } else {
        if (xTimerStop(*timer, (configTIMER_MAX_BLOCK_TIME / OS_MS_PER_TICK)) == pdPASS) {
            result = 1;
        }
    }

    if (result == 0) {
        DEBUGPRINT("sys_timer_stop (0x%08x) return fail, from_isr is %d\r\n", *timer, from_isr);
    }

    return result;
}

/*!
    \brief      check if the timer is active and pending for expiration
    \param[in]  timer: pointer to the timer handle
    \param[out] none
    \retval     1 if it's pending, 0 otherwise.
*/
uint8_t sys_timer_pending(os_timer_t *timer)
{
    return xTimerIsTimerActive(*timer);
}

/*!
    \brief      Miscellaneous initialization work after OS initialized and
                task scheduler started
                Note: It runs in the standalone start task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_misc_init(void)
{

}

/*!
    \brief      initialize the OS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_init(void)
{

}

/*!
    \brief      start the OS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_start(void)
{
    /* start scheduler */
    vTaskStartScheduler();
}
