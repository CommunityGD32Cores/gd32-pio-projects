/*!
    \file    telnet_main.h
    \brief   telnet server demo

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
#ifndef _TELNET_MAIN_H
#define _TELNET_MAIN_H

#include "wrapper_os.h"
#include <stdbool.h>

#define TELNET_TASK_STK_SIZE                  512
#define TELNET_TASK_QUEUE_SIZE                4
#define MAX_LEN_OF_TELNET_CMD                 30
#define MAX_COMMANDLINE_LENGTH                2048

typedef struct _telnet_cmd_entry {
    char *command;
    void (*function)(int, char **);
} telnet_cmd_entry;

/*============================ MSSAGE TYPES =========================================*/
typedef enum {
    TELNET_MESSAGE_CHARS_NOTIFY = 1,
    TELNET_MESSAGE_SERVER_CLOSE_NOTIFY,
    TELNET_MESSAGE_NUM
}TELNET_MESSAGE_TYPE_E;
/*============================ MSSAGE TYPES =========================================*/

extern os_task_t telnet_task_tcb;
extern os_task_t *telnet_task_handle;
extern uint32_t telnet_task_stk[TELNET_TASK_STK_SIZE] __ALIGNED(8);

void telnet_server_stop(void);
int telnet_server_start(void);
int string_aggregation_for_resp_to_cli(char *str);
void exec_remote_cmd(char *inbuf);
void send_messg_to_telnet(TELNET_MESSAGE_TYPE_E msg_type);
void check_and_write_revc_message(char* str);

#endif
