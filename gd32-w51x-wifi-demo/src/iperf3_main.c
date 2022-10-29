/*
 * iperf, Copyright (c) 2014, 2015, The Regents of the University of
 * California, through Lawrence Berkeley National Laboratory (subject
 * to receipt of any required approvals from the U.S. Dept. of
 * Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE.  This software is owned by the U.S. Department of Energy.
 * As such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * This code is distributed under a BSD style license, see the LICENSE
 * file for complete information.
 */

#include <string.h>
#include "app_cfg.h"
#include "bsp_inc.h"
#include "app_type.h"

#ifdef CONFIG_IPERF_TEST
#include "wrapper_os.h"
#include "debug_print.h"
#include "malloc.h"
#include <lwip/sockets.h>

#include "iperf_queue.h"
#include "timer.h"
#include "iperf.h"
//#include "rom_export.h"
#include "iperf_api.h"
#endif

#ifdef CONFIG_IPERF_TEST

#undef iperf_err
#define iperf_err(a, fmt, arg...) do{\
                                printf("\r\n"fmt, ##arg);\
                            }while(0);

unsigned char iperf_terminate;
unsigned char iperf_task_num = 0;
os_task_t *iperf_task_tcb_ptr[IPERF_TASK_MAX] = {NULL};

extern void gnu_getopt_reset(void);

/**************************************************************************/
static int
iperf_run(struct iperf_test *test)
{
    int consecutive_errors;

    switch (test->role) {
    case 's':
        if (test->daemon) {
            int rc = 0; //daemon(0, 0);
            if (rc < 0) {
                i_errno = 133; //IEDAEMON;
                iperf_err(test, "error - %s", iperf_strerror(i_errno));
                return (-1);
            }
        }
        consecutive_errors = 0;

        for (;;) {
            if (iperf_terminate) {
                break;
            }
            if (iperf_run_server(test) < 0) {
                iperf_err(test, "error - %s", iperf_strerror(i_errno));
                ++consecutive_errors;
                if (consecutive_errors >= 5) {
                    iperf_err(test, "too many errors, exiting");
                    return (-1);
                }
            } else {
                consecutive_errors = 0;
            }
            iperf_reset_test(test);
            if (iperf_get_test_one_off(test))
                break;
        }

        break;
    case 'c':
        if (iperf_run_client(test) < 0) {
            iperf_err(test, "error - %s", iperf_strerror(i_errno));
            return (-1);
        }
        break;
    default:
        usage();
        break;
    }

    return 0;
}

void iperf_test_task(void *param)
{
    struct iperf_test *test = (struct iperf_test *)param;
    unsigned int tcb_index = test->task_tcb_index;
    SYS_SR_ALLOC();

    if (iperf_run(test) < 0) {
        iperf_err(test, "error - %s", iperf_strerror(i_errno));
         //iperf_free_test(test);
        //return;
    }

    iperf_free_test(test);

    DEBUGPRINT("\r\niperf3 task: used stack = %d, free stack = %d\r\n",
                (IPERF_STACK_SIZE - sys_stack_free_get(NULL)), sys_stack_free_get(NULL));

    DEBUGPRINT("Iperf3 task stopped!\r\n");

    SYS_CRITICAL_ENTER();
    iperf_task_tcb_ptr[tcb_index] = NULL;
    iperf_task_num--;
    SYS_CRITICAL_EXIT();

    sys_task_delete(NULL);
}

/**************************************************************************/
int
iperf_main(int argc, char **argv)
{
    struct iperf_test *test;
    unsigned int i;
    unsigned int task_priority;
    void *handle;
    SYS_SR_ALLOC();

    iperf_terminate = 0;
    test = iperf_new_test();
    if (!test) {
        iperf_err(NULL, "create new test error - %s", iperf_strerror(i_errno));
        return (-1);
    }

    iperf_defaults(test);    /* sets defaults */

    if (iperf_parse_arguments(test, argc, argv) < 0) {
        iperf_err(test, "parameter error - %s", iperf_strerror(i_errno));
        //usage_long();
        usage();
        gnu_getopt_reset();
        iperf_free_test(test);
        return (-1);
    }

    gnu_getopt_reset();

    if (((test->role == 'c') || (test->role == 's')) && (iperf_task_num < IPERF_TASK_MAX)) {

        for (i=0; i<IPERF_TASK_MAX; i++) {
            if (iperf_task_tcb_ptr[i] == NULL) {
                test->task_tcb_index = i;
                break;
            }
        }

        if (i == IPERF_TASK_MAX) {
            DEBUGPRINT("\r\n\rERROR: can not find available iperf3 task tcb.");
            iperf_free_test(test);
            return (-1);
        }

        task_priority = IPERF_TASK_PRIO;

        if (test->role == 'c') {
            //DEBUGPRINT("\r\n\rtask priority, tos = %d", test->settings->tos);
            if (test->settings->tos == 0xe0) {
                task_priority = IPERF_TASK_PRIO;
            } else if (test->settings->tos == 0xa0) {
                task_priority = IPERF_TASK_PRIO + TASK_PRIO_LOWER(1);
            } else if (test->settings->tos == 0x20){
                task_priority = IPERF_TASK_PRIO + TASK_PRIO_LOWER(3);
            }
        }

        handle = sys_task_create(NULL, (const uint8_t *)"iperf_test", NULL,
                IPERF_STACK_SIZE, 0, task_priority,
                (task_func_t)iperf_test_task, test);
        if (handle == NULL) {
            DEBUGPRINT("\r\n\rERROR: create iperf3 task failed.");
            iperf_free_test(test);
            return (-1);
        } else {
            SYS_CRITICAL_ENTER();
            iperf_task_tcb_ptr[i] = handle;
            iperf_task_num++;
            SYS_CRITICAL_EXIT();
            return 0;
        }

    } else {
        iperf_free_test(test);
        return 0;
    }
}

void cmd_iperf3(int argc, char **argv)
{
    if(argc >= 2){
        if(strcmp(argv[1], "-s") == 0){
            DEBUGPRINT("\r\nIperf3: start iperf3 server!\r\n");
        } else if(strcmp(argv[1], "-c") == 0){
            DEBUGPRINT("\r\nIperf3: start iperf3 client!\r\n");
        } else if (strcmp(argv[1], "-v") == 0 ||strcmp(argv[1], "-h") == 0) {

        } else if (strcmp(argv[1], "stop") == 0) {
            iperf_terminate = 1;
            return;
        } else {
            goto Exit;
        }
    } else {
        goto Exit;
    }

    iperf_main(argc, argv);

    return;
Exit:
    DEBUGPRINT("\r\nIperf3: command format error!\r\n");
    DEBUGPRINT("\r\nUsage: \"iperf3 -s\" to start iperf3 server or \"iperf3 -c\" to start iperf3 client or \"iperf3 -h\" for help\r\n");
}

#endif
