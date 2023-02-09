/*!
    \file    telnet_main.c
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

#include "wrapper_os.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#ifdef CONFIG_TELNET_SERVER
#include "telnet_main.h"
#include "wifi_management.h"
#include "telnetserver.h"

extern void cmd_cb_scan_done(void *eloop_data, void *user_ctx);
extern void cmd_cb_scan_fail(void *eloop_data, void *user_ctx);
extern os_task_t telnet_task_tcb;

uint32_t telnet_task_stk[TELNET_TASK_STK_SIZE] __ALIGNED(8);

char resp_to_cli[MAX_COMMANDLINE_LENGTH];
int resp_to_cli_buff_idx = 0;

static void cmd_telnet_wifi_scan(int argc, char **argv);
static void cmd_telnet_help(int argc, char **argv);

os_task_t telnet_task_tcb;
os_task_t *telnet_task_handle = NULL;

static const telnet_cmd_entry cmd_telnet_table[] = {
    {"wifi_scan", cmd_telnet_wifi_scan},
    {"help", cmd_telnet_help}
};

typedef struct {
    char command_buffer[MAX_LEN_OF_TELNET_CMD];
    int  buffer_idx;
} CommandInterpreter_t;
static CommandInterpreter_t interpreter;

static void clear_command_buffer(void)
{
    memset(interpreter.command_buffer, '\0', MAX_LEN_OF_TELNET_CMD);
    interpreter.buffer_idx = 0;
}

/*!
    \brief      aggregation string
    \param[in]  p: point to string address need to aggregation
    \param[out] resp_to_cli: aggregationed result
    \retval     exec result  = 0 means success and != 0 means failed
*/
int string_aggregation_for_resp_to_cli(char *p)
{
    if (p == NULL || resp_to_cli_buff_idx >= MAX_COMMANDLINE_LENGTH) {
        return -1;
    }
    while (*p != 0 && resp_to_cli_buff_idx < MAX_COMMANDLINE_LENGTH) {
        resp_to_cli[resp_to_cli_buff_idx++] = *p;
        p++;
    }
    return 0;
}

void clean_resp_to_cli_buff(void)
{
    memset(resp_to_cli, 0, MAX_COMMANDLINE_LENGTH);
    resp_to_cli_buff_idx = 0;
}

static void cmd_telnet_help(int argc, char **argv)
{
    string_aggregation_for_resp_to_cli("\rCOMMAND LIST:\r\n");
    for (int i = 0; i < ARRAY_SIZE(cmd_telnet_table); i++) {
        string_aggregation_for_resp_to_cli(cmd_telnet_table[i].command);
        string_aggregation_for_resp_to_cli("\r\n");
    }
    telnet_write_revc_message(resp_to_cli);
    clean_resp_to_cli_buff();
}

static void cmd_telnet_wifi_scan(int argc, char **argv)
{
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_DONE, cmd_cb_scan_done, NULL, NULL);
    eloop_event_register(WIFI_MGMT_EVENT_SCAN_FAIL, cmd_cb_scan_fail, NULL, NULL);

    if (wifi_management_scan(FALSE) != 0) {
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_DONE);
        eloop_event_unregister(WIFI_MGMT_EVENT_SCAN_FAIL);
        DEBUGPRINT("telnet: start wifi_scan failed\r\n");
    }
}

static int telnet_cmd_parse(char *buf, char **argv)
{
    int argc = 0;
    while((argc < MAX_LEN_OF_TELNET_CMD) && (*buf != '\0')) {
        argv[argc] = buf;
        argc++;
        buf++;

        while((*buf != ' ') && (*buf != '\0'))
            buf++;

        while(*buf == ' ') {
            *buf = '\0';
            buf++;
        }

        if(argc == 1){
            if(strcmp(argv[0], "iwpriv") == 0){
                if(*buf != '\0'){
                    argv[1] = buf;
                    argc++;
                }
                break;
            }
        }
    }
    return argc;
}

/*!
    \brief      exec telnet remote cmd
    \param[in]  inbuf: remote cmd addr
    \param[out] none
    \retval     none
*/
void exec_remote_cmd(char *inbuf)
{
    int i, argc;
    char *argv[MAX_LEN_OF_TELNET_CMD];
    if ((argc = telnet_cmd_parse(inbuf, argv)) > 0) {
        int found = 0;
        for (i = 0; i < ARRAY_SIZE(cmd_telnet_table); i++) {
            if (strcmp(argv[0], cmd_telnet_table[i].command) == 0) {
                cmd_telnet_table[i].function(argc, argv);
                found = 1;
                break;
            }
        }
        if (!found) {
            telnet_write_revc_message("telnet: invalid command!\r\n");
        }
    }
    return;
}

static void command_process_char(const char character)
{
    if (character != 0x00 && interpreter.buffer_idx < MAX_COMMANDLINE_LENGTH) {
        if (character == 0x0A || character == 0x0D) {        /*Handling newline characters*/
            exec_remote_cmd(interpreter.command_buffer);
            clear_command_buffer();
        } else if (character == 0x08 || character == 0x7F) { /*Handling backspace characters*/
            if (interpreter.buffer_idx > 0) {
                --interpreter.buffer_idx;
                interpreter.command_buffer[interpreter.buffer_idx] = '\0';
            }
        } else if (character == 0x20) {                      /*Handling space characters*/
           if (interpreter.buffer_idx > 0
               && interpreter.command_buffer[interpreter.buffer_idx - 1] != 0x20) {
               interpreter.command_buffer[interpreter.buffer_idx] = character;
               ++interpreter.buffer_idx;
           }
        } else {
           interpreter.command_buffer[interpreter.buffer_idx] = character;
           ++interpreter.buffer_idx;
        }
    }
}

static void telnet_server_process(void* arg)
{
    char character;
    TELNET_MESSAGE_TYPE_E msg_type;
    if (initialize_telnet_server() != TELNET_OK) {
        return;
    }

    while (1) {
        if (sys_task_wait(0, &msg_type) == OS_OK) {
            if (msg_type == TELNET_MESSAGE_CHARS_NOTIFY) {
                do {
                    character = telnet_read();
                    command_process_char(character);
                } while (character != '\0');
            } else if (msg_type == TELNET_MESSAGE_SERVER_CLOSE_NOTIFY) {
                break;
            }
        }
    }

    close_telnet_tcp();
    DEBUGPRINT("telnet: server has been stoped!\r\n");
    telnet_task_handle = NULL;
    sys_task_delete(NULL);

    return;
}

/*!
    \brief      start telnet server
    \param[in]  none
    \param[out] none
    \retval     exec result  = 0 means success and != 0 means failed
*/
int telnet_server_start(void)
{
    if (telnet_task_handle != NULL) {
        DEBUGPRINT("telnet: server has already been started\r\n");
        return 0;
    }
    telnet_task_handle = sys_task_create(&telnet_task_tcb, (const uint8_t *)"telnet_server", &telnet_task_stk[0],
                TELNET_TASK_STK_SIZE, TELNET_TASK_QUEUE_SIZE, TELNET_TASK_PRIO,
                (task_func_t)telnet_server_process, NULL);
    if (telnet_task_handle == NULL) {
        DEBUGPRINT("telnet: create task failed\r\n");
        return -1;
    }
    DEBUGPRINT("telnet: start server successfully\r\n");
    return 0;
}

/*!
    \brief      stop telnet server
    \param[in]  none
    \param[out] none
    \retval     none
*/
void telnet_server_stop(void)
{
    if (telnet_task_handle != NULL) {
        send_messg_to_telnet(TELNET_MESSAGE_SERVER_CLOSE_NOTIFY);
    } else {
        DEBUGPRINT("telnet: server is closed!\r\n");
    }
    return;
}

/*!
    \brief      send message to notify telnet that remote chars comes
    \param[in]  none
    \param[out] none
    \retval
*/
void send_messg_to_telnet(TELNET_MESSAGE_TYPE_E msg_type)
{
    int32_t msg_waiting = 0;
    os_task_t *receiver_tcb = &telnet_task_tcb;
    msg_waiting = sys_task_msg_num(receiver_tcb, 1);

    if ((msg_waiting >= 0) && (msg_waiting < 3)) {
        sys_task_post(receiver_tcb, &msg_type, 1);
    }
    return;
}

/*!
    \brief      check and write revc message into buffer
    \param[in]  str: str need write into buff
    \param[out] str: write revc message into buffer
    \retval
*/
void check_and_write_revc_message(char* str)
{
    if (telnet_is_connected() == true) {
        telnet_write_revc_message(str);
    }
    return;
}
#endif
