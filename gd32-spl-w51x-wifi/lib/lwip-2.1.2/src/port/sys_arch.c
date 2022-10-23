#undef MOUDLE
#define MOUDLE SYS_ARCH

#include "debug_print.h"

#include "cc.h"
#include "sys_arch.h"
#include "lwip/sys.h"

os_task_t lwip_task_tcb[LWIP_TASK_MAX];
uint32_t lwip_task_stk[LWIP_TASK_STK_POOL_SIZE] __ALIGNED(8);
uint16_t lwip_task_stk_offset;/*point to the next available task stack beginning*/
uint16_t lwip_task_tcb_offset;/*point to the next available lwip task tcb*/

void sys_init(void)
{
    lwip_task_stk_offset = 0;
    lwip_task_tcb_offset = 0;
}

err_t sys_sem_new(sys_sem_t *sem, uint8_t count)
{
    int32_t err;

    err= sys_sema_init(sem, (int32_t)count);

    if (err != OS_OK ) {
        DEBUG_ERROR("sys_sem_new");
        return ERR_MEM;
    }

    return ERR_OK;
}

/*
Blocks the thread while waiting for the semaphore to be
signaled. If the "timeout" argument is non-zero, the thread should
only be blocked for the specified time (measured in
milliseconds). If the "timeout" argument is zero, the thread should be
blocked until the semaphore is signalled.

If the timeout argument is non-zero, the return value is the number of
milliseconds spent waiting for the semaphore to be signaled. If the
semaphore wasn't signaled within the specified time, the return value is
SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
(i.e., it was already signaled), the function may return zero.

Notice that lwIP implements a function with a similar name,
sys_sem_wait(), that uses the sys_arch_sem_wait() function.

*/
uint32_t sys_arch_sem_wait(sys_sem_t *sem, uint32_t timeout)
{
    int32_t err;
    uint32_t start_time;
    uint32_t stop_time;

    start_time = sys_current_time_get();

    err = sys_sema_down(sem, timeout);

    if (err == OS_TIMEOUT) {
        return SYS_ARCH_TIMEOUT;
    }

    if (timeout == 0) {
        return ERR_OK;
    }

    stop_time = sys_current_time_get();

    return (stop_time - start_time);
}



/*
Creates an empty mailbox for maximum "size" elements. Elements stored
in mailboxes are pointers. You have to define macros "_MBOX_SIZE"
in your lwipopts.h, or ignore this parameter in your implementation
and use a default size.
If the mailbox has been created, ERR_OK should be returned. Returning any
other error will provide a hint what went wrong, but except for assertions,
no real error handling is implemented.

*/
err_t sys_mbox_new(sys_mbox_t *mbox, int32_t size)
{
    int32_t err;

    err = sys_queue_init(mbox, size, sizeof(void *));

    if (err != OS_OK) {
        DEBUG_ERROR("sys_mbox_new");
        return ERR_MEM;
    }

    return ERR_OK;
}

/*
Posts the "msg" to the mailbox. This function have to block until
the "msg" is really posted.
*/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    int32_t err;
    uint8_t i=0;

    //try 10 times
    while (i<10) {
        err = sys_queue_post(mbox, &msg);
        if (err == OS_OK) {
            break;
        }
        i++;
        sys_ms_sleep(5);
    }

    DEBUG_ASSERT(i !=10);
}


/*
Try to post the "msg" to the mailbox. Returns ERR_MEM if this one
is full, else, ERR_OK if the "msg" is posted.

*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    int32_t err;

    err = sys_queue_post(mbox, &msg);

    if (err != OS_OK){
        return ERR_MEM;
    }

    return ERR_OK;
}

/*
Try to post the "msg" to the mailbox from isr. Returns ERR_MEM if this one
is full, else, ERR_OK if the "msg" is posted.
Not used by lwip now, implemented it same as sys_mbox_trypost.
*/
err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg)
{
    int32_t err;

    err = sys_queue_post(mbox, &msg);

    if (err != OS_OK){
        return ERR_MEM;
    }

    return ERR_OK;
}

/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
  be blocked until a message arrives. The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.

 */
uint32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, uint32_t timeout)
{
    uint32_t start_time;
    uint32_t stop_time;
    int32_t ret_val;

    start_time = sys_current_time_get();

    ret_val = sys_queue_fetch(mbox, msg, timeout, 1);

    if (ret_val != OS_OK) {
        return SYS_ARCH_TIMEOUT;
    }

    if (timeout == 0) {
        return ERR_OK;
    }

    stop_time = sys_current_time_get();

    return (stop_time - start_time);
}


/*
This is similar to sys_arch_mbox_fetch, however if a message is not
present in the mailbox, it immediately returns with the code
SYS_MBOX_EMPTY. On success 0 is returned.

To allow for efficient implementations, this can be defined as a
function-like macro in sys_arch.h instead of a normal function. For
example, a naive implementation could be:
  #define sys_arch_mbox_tryfetch(mbox,msg) \
    sys_arch_mbox_fetch(mbox,msg,1)
although this would introduce unnecessary delays.

*/
uint32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int32_t ret_val;

    ret_val = sys_queue_fetch(mbox, msg, 0, 0);

    if (ret_val != OS_OK) {
        return SYS_MBOX_EMPTY;
    }

    return ERR_OK;
}


/*
Starts a new thread named "name" with priority "prio" that will begin its
execution in the function "thread()". The "arg" argument will be passed as an
argument to the thread() function. The stack size to used for this thread is
the "stacksize" parameter. The id of the new thread is returned. Both the id
and the priority are system dependent.

*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int32_t stacksize, int32_t prio)
{
    void *handle;

    if ((lwip_task_stk_offset + stacksize) > LWIP_TASK_STK_POOL_SIZE) {
        DEBUG_ERROR("lwip task stack not enough");
        return 0;
    }

    if (lwip_task_tcb_offset == LWIP_TASK_MAX) {
        DEBUG_ERROR("lwip task tcb not enough");
        return 0;
    }

    handle = sys_task_create(&lwip_task_tcb[lwip_task_tcb_offset], (const uint8_t *)name, &lwip_task_stk[lwip_task_stk_offset],
                    stacksize, 0, prio, (task_func_t)thread, arg);
    if (handle == NULL) {
        DEBUG_ERROR("lwip create task failed");
        return 0;
    }

    lwip_task_stk_offset += stacksize;
    lwip_task_tcb_offset++;

    return prio;
}
