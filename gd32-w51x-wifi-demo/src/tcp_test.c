/*!
    \file    tcp_test.c
    \brief   TCP test for GD32W51x WiFi SDK

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

#include <string.h>
#include <stdlib.h>
#include "app_cfg.h"
#include "app_type.h"
#include "wrapper_os.h"
#include "malloc.h"
#include "debug_print.h"
#include <lwip/sockets.h>

// #define DEBUG_DUAL_MODE
#define HEADER_VERSION1            0x80000000
#define DUAL_MODE_RUN_NOW          0x00000001

#define ERROR_ALREADY_RUNNING      1
#define ERROR_NO_MEMORY            -1

struct tcp_data_t {
    void *task_hdl;
    int32_t srv_fd;
    int32_t cli_fd;
    uint8_t is_server;
    uint16_t srv_port;        /* server port to listen on/connect to (default 5001) */
    char srv_ip[16];          /* server ip to listen on/connect to */
    uint8_t dualmode;         /* Set dual mode, do a bidirectional test simultaneously */
    uint8_t nodelay;          /* set TCP no delay, disabling Nagle's Algorithm */
    uint16_t buf_len;         /* length of buffer to read or write (default 1460 Bytes) */
    uint8_t tos;              /* type of service byte */
    uint32_t rpt_intvl;       /* ticks between periodic bandwidth reports (default 1000 ticks) */
    uint32_t time_limit;      /* time in ticks to transmit for (default 10000 ticks) */
    uint64_t size_limit;      /* number of buffer to transmit for (default use time) */
};

struct client_hdr {
    int32_t flags;
    int32_t numThreads;
    int32_t mPort;
    int32_t bufferlen;
    int32_t mWindowSize;
    int32_t mAmount;
    int32_t mRate;
    int32_t mUDPRateUnits;
};

static const long kKilo_to_Unit = 1024;
static const long kMega_to_Unit = 1024 * 1024;
static const long kGiga_to_Unit = 1024 * 1024 * 1024;

static const long kkilo_to_Unit = 1000;
static const long kmega_to_Unit = 1000 * 1000;
static const long kgiga_to_Unit = 1000 * 1000 * 1000;

static struct tcp_data_t *tcp_srv_data = NULL;
static struct tcp_data_t *tcp_cli_data = NULL;
static int32_t tcp_terminate = 0;
static void tcp_task_func(void *param);

static uint64_t byte_atoi(const char *inString)
{
    double theNum;
    char suffix = '\0';

    /* scan the number and any suffices */
    sscanf(inString, "%lf%c", &theNum, &suffix);

    /* convert according to [Gg Mm Kk] */
    switch ( suffix ) {
        case 'G':  theNum *= kGiga_to_Unit;  break;
        case 'M':  theNum *= kMega_to_Unit;  break;
        case 'K':  theNum *= kKilo_to_Unit;  break;
        case 'g':  theNum *= kgiga_to_Unit;  break;
        case 'm':  theNum *= kmega_to_Unit;  break;
        case 'k':  theNum *= kkilo_to_Unit;  break;
        default: break;
    }
    return (uint64_t)theNum;
}

#ifdef DEBUG_DUAL_MODE
/*!
    \brief      dump client header
    \param[in]  h: pointer to the input client header structure
    \param[in]  flags: a bitmap for different options
        \arg        HEADER_VERSION1 0x80000000
        \arg        RUN_NOW         0x00000001
        \arg        UNITS_PPS       0x00000002
    \param[in]  numThreads: number of the client threads
    \param[in]  mPort: port number of the client
    \param[in]  bufferlen: buffer length of the client
    \param[in]  mWindowSize: TCP window size of the client
    \param[in]  mAmount: number of bytes to send
    \param[in]  mRate: UDP data rate
    \param[in]  mUDPRateUnits: UDP rate uinit (either bw or pps)
    \param[out] none
    \retval     none
*/
static void client_header_dump(struct client_hdr *h)
{
    DEBUGPRINT("\r\n========== dump client header ============\r\n");
    DEBUGPRINT("flags = 0x%x, ntohl(flags) = 0x%x\r\n", h->flags, ntohl(h->flags));
    DEBUGPRINT("numThreads = %d\r\n", ntohl(h->numThreads));
    DEBUGPRINT("mPort = %d\r\n", ntohl(h->mPort));
    DEBUGPRINT("bufferlen = %d\r\n", ntohl(h->bufferlen));
    DEBUGPRINT("mWindowSize = %d\r\n", ntohl(h->mWindowSize));
    DEBUGPRINT("mAmount = 0x%x (%d)\r\n", ntohl(h->mAmount), ntohl(h->mAmount));
    DEBUGPRINT("mRate = %d\r\n", ntohl(h->mRate));
    DEBUGPRINT("mUDPRateUnits = %d\r\n", ntohl(h->mUDPRateUnits));
    DEBUGPRINT("========== dump client header end ============\r\n");
}
#endif

/*!
    \brief      send client header
    \param[in]  tcp_data: pointer to the send client header structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  nodelay: set TCP no delay, disabling Nagle's Algorithm
                  buf_len: length of buffer to read or write (default 1460 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
    \param[out] none
    \retval     none
*/
static void client_header_send(struct tcp_data_t *tcp_data)
{
    struct client_hdr header;

    if (tcp_data->dualmode == 1) {
        header.flags = htonl(HEADER_VERSION1 | DUAL_MODE_RUN_NOW);  // 0x01000080
    } else {
        header.flags = 0;
    }
    header.numThreads = htonl(0x01);
    header.mPort = htonl(tcp_data->srv_port);
    header.bufferlen = htonl(tcp_data->buf_len);
    header.mWindowSize = htonl(TCP_WND);
    if (tcp_data->size_limit> 0) {
        header.mAmount = tcp_data->size_limit & 0x7FFFFFFF;
        header.mAmount = htonl(header.mAmount);
    } else {
        header.mAmount = (int32_t)(tcp_data->time_limit * 100 / OS_TICK_RATE_HZ);
        header.mAmount = htonl(-header.mAmount);
    }
    header.mRate = 0;  // UDP
    header.mUDPRateUnits = 0;  // UDP

#ifdef DEBUG_DUAL_MODE
    buffer_dump("Client header:", &header, sizeof(header));
    client_header_dump(&header);
#endif
    if (send(tcp_data->cli_fd, (char*)&header, sizeof(header), 0) <= 0) {
        DEBUGPRINT("TCP ERROR: TCP client send client header error.\r\n");
    }
}

/*!
    \brief      initialize client data
    \param[in]  none
    \param[out] none
    \retval     function run status
      \arg        ERROR_ALREADY_RUNNING: tcp client is already running
      \arg        ERROR_NO_MEMORY: No memory for tcp data
      \arg        0: run success
*/
static int client_data_init(void)
{
    if (tcp_cli_data) {
        if (tcp_cli_data->task_hdl != NULL) {
            DEBUGPRINT("TCP: Tcp client is already running.\r\n");
            return ERROR_ALREADY_RUNNING;
        }
    } else {
        tcp_cli_data = sys_malloc(sizeof(struct tcp_data_t));
        if (tcp_cli_data == NULL) {
            DEBUGPRINT("TCP: No memory for tcp_data.\r\n");
            return ERROR_NO_MEMORY;
        }
    }

    sys_memset(tcp_cli_data, 0, sizeof(struct tcp_data_t));
    tcp_cli_data->cli_fd = -1;
    tcp_cli_data->buf_len = 1460;
    tcp_cli_data->srv_port = 5001;
    tcp_cli_data->tos = 0;
    tcp_cli_data->rpt_intvl = 1 * OS_TICK_RATE_HZ;  // default 1s
    tcp_cli_data->time_limit = 10 * OS_TICK_RATE_HZ + 100;  // default 10s

    return 0;
}

/*!
    \brief      initialize server data
    \param[in]  none
    \param[out] none
    \retval     function run status
      \arg        ERROR_ALREADY_RUNNING: tcp server is already running
      \arg        ERROR_NO_MEMORY: No memory for tcp data
      \arg        0: run success
*/
static int server_data_init(void)
{
    if (tcp_srv_data) {
        if (tcp_srv_data->task_hdl != NULL) {
            DEBUGPRINT("TCP WARNING: Tcp server is already running.\r\n");
            return ERROR_ALREADY_RUNNING;
        }
    } else {
        tcp_srv_data = sys_malloc(sizeof(struct tcp_data_t));
        if (tcp_srv_data == NULL) {
            DEBUGPRINT("TCP ERROR: No memory for tcp_data.\r\n");
            return ERROR_NO_MEMORY;
        }
    }

    sys_memset(tcp_srv_data, 0, sizeof(struct tcp_data_t));
    tcp_srv_data->is_server = 1;
    tcp_srv_data->srv_fd = -1;
    tcp_srv_data->cli_fd = -1;
    tcp_srv_data->buf_len = 1460;
    tcp_srv_data->srv_port = 5001;
    tcp_srv_data->rpt_intvl = 1 * OS_TICK_RATE_HZ;  // default 1s

    return 0;
}

/*!
    \brief      tcp client function
    \param[in]  tcp_data: pointer to the send client header structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  nodelay: set TCP no delay, disabling Nagle's Algorithm
                  buf_len: length of buffer to read or write (default 1460 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
    \param[out] none
    \retval     none
    \retval     function run status
      \arg        -1: allocate client buffer failed
      \arg        0: run success
*/
static int tcp_client(struct tcp_data_t *tcp_data)
{
    struct sockaddr_in sAddr;
    int iAddrSize = sizeof(struct sockaddr_in);
    char *cBsdBuf = NULL;
    int iStatus, iCounter, n;
    uint32_t stime, etime, rpt_stime;  // ticks
    uint32_t m;
    uint64_t rpt_sz = 0, tot_sz = 0;
    uint32_t time = 0, tcnt = 0;  // seconds
    uint32_t intvl_sec = tcp_data->rpt_intvl / OS_TICK_RATE_HZ;

    cBsdBuf = sys_malloc(tcp_data->buf_len);
    if(NULL == cBsdBuf){
        DEBUGPRINT("TCP ERROR: allocate client buffer failed (len = %d).\r\n", tcp_data->buf_len);
        return -1;
    }

    for (iCounter = 0; iCounter < tcp_data->buf_len; iCounter++) {
        cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    FD_ZERO(&sAddr);
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(tcp_data->srv_port);
    sAddr.sin_addr.s_addr = inet_addr(tcp_data->srv_ip);

    /* creating a TCP socket */
    tcp_data->cli_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_data->cli_fd < 0) {
        DEBUGPRINT("TCP ERROR: create tcp client socket fd error!\r\n");
        goto Exit2;
    }

    /* Set IP TOS */
    if (tcp_data->tos) {
        m = tcp_data->tos;
        if (setsockopt(tcp_data->cli_fd, IPPROTO_IP, IP_TOS, &m, sizeof(m)) < 0) {
            DEBUGPRINT("TCP ERROR:set socket tos %d error, errno = %d!\r\n", m, errno);
            goto Exit2;
        }
    }

    n = tcp_data->nodelay;
    setsockopt(tcp_data->cli_fd, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &n, sizeof( n ) );

    DEBUGPRINT("TCP: server IP=%s port=%d.\r\n", tcp_data->srv_ip, tcp_data->srv_port);
    DEBUGPRINT("TCP: create socket %d.\r\n", tcp_data->cli_fd);

    /* connecting to TCP server */
    iStatus = connect(tcp_data->cli_fd, (struct sockaddr *)&sAddr, iAddrSize);
    if (iStatus < 0) {
        DEBUGPRINT("TCP ERROR: tcp client connect server error!\r\n");
        goto Exit1;
    }

    client_header_send(tcp_data);

    rpt_stime = etime = stime = sys_current_time_get();
    if (tcp_data->size_limit != 0) {
        while (!tcp_terminate && (tot_sz < tcp_data->size_limit)) {
            if (send(tcp_data->cli_fd, cBsdBuf, tcp_data->buf_len, 0) <= 0) {
                DEBUGPRINT("TCP ERROR: TCP client send data error.\r\n");
                goto Exit1;
            }
            tot_sz += tcp_data->buf_len;
            rpt_sz += tcp_data->buf_len;
            etime = sys_current_time_get();
            time = (etime - rpt_stime) * OS_MS_PER_TICK;

            if ((etime - rpt_stime) >= tcp_data->rpt_intvl) {
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Send\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
            }
        }
    } else {
        while (!tcp_terminate && ((etime - stime) <= tcp_data->time_limit)) {
            if (send(tcp_data->cli_fd, cBsdBuf, tcp_data->buf_len, 0) <= 0) {
                DEBUGPRINT("TCP ERROR: TCP client send data error.\r\n");
                goto Exit1;
            }
            tot_sz += tcp_data->buf_len;
            rpt_sz += tcp_data->buf_len;
            etime = sys_current_time_get();
            time = (etime - rpt_stime) * OS_MS_PER_TICK;

            if ((etime - rpt_stime) >= tcp_data->rpt_intvl) {
                // DEBUGPRINT("etime=%d stime=%d limit=%d\r\n", etime, stime, tcp_data->time_limit);
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Send\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
            }
        }
    }

    time = (etime - stime) * OS_MS_PER_TICK;
    DEBUGPRINT("Total\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", 0, tcnt, (uint32_t)(tot_sz >> 10), (uint32_t)((tot_sz << 3) / time));

Exit1:
    close(tcp_data->cli_fd);

Exit2:
    sys_mfree(cBsdBuf);
    return 0;
}

/*!
    \brief      tcp server function
    \param[in]  tcp_data: pointer to the send client header structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  nodelay: set TCP no delay, disabling Nagle's Algorithm
                  buf_len: length of buffer to read or write (default 1460 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
    \param[out] none
    \retval     none
    \retval     function run status
      \arg        -1: allocate server buffer failed
      \arg        0: run success
*/
static int tcp_server(struct tcp_data_t *tcp_data)
{
    struct sockaddr_in  sLocalAddr;
    struct sockaddr_in  sCliAddr;
    int iAddrSize = sizeof(sLocalAddr);
    int iCounter, iStatus, n, recv_sz;
    char *cBsdBuf = NULL;
    uint32_t stime, etime, rpt_stime;  // ticks
    uint64_t rpt_sz = 0, tot_sz = 0;
    uint32_t time, tcnt = 0;  // ms
    uint32_t intvl_sec = tcp_data->rpt_intvl / OS_TICK_RATE_HZ;
    struct client_hdr header;

    cBsdBuf = sys_malloc(tcp_data->buf_len);
    if(NULL == cBsdBuf){
        DEBUGPRINT("TCP ERROR: allocate server buffer failed (len = %d).\r\n", tcp_data->buf_len);
        return -1;
    }

    for (iCounter = 0; iCounter < tcp_data->buf_len; iCounter++) {
        cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    tcp_data->srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_data->srv_fd < 0) {
        goto Exit3;
    }

    DEBUGPRINT("TCP: create server socket %d.\r\n", tcp_data->srv_fd);
    n = 1;
    setsockopt(tcp_data->srv_fd, SOL_SOCKET, SO_REUSEADDR,
            (const char *) &n, sizeof( n ) );

    sys_memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family      = AF_INET;
    sLocalAddr.sin_len         = sizeof(sLocalAddr);
    sLocalAddr.sin_port        = htons(tcp_data->srv_port);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the TCP socket to the TCP server address */
    iStatus = bind(tcp_data->srv_fd, (struct sockaddr *)&sLocalAddr, iAddrSize);
    if( iStatus < 0 ) {
        DEBUGPRINT("TCP ERROR: bind tcp server socket fd error!\r\n");
        goto Exit2;
    }
    DEBUGPRINT("TCP: bind successfully.\r\n");

    /* putting the socket for listening to the incoming TCP connection */
    /* Make it listen to socket with max 20 connections */
    iStatus = listen(tcp_data->srv_fd, 20);
    if( iStatus != 0 ) {
        DEBUGPRINT("TCP ERROR: listen tcp server socket fd error!\r\n");
        goto Exit2;
    }
    DEBUGPRINT("TCP: listen port %d\r\n", tcp_data->srv_port);

    /* waiting for an incoming TCP connection */
    /* accepts a connection form a TCP client, if there is any. otherwise returns SL_EAGAIN */
    tcp_data->cli_fd = accept(tcp_data->srv_fd,
                                (struct sockaddr *)&sCliAddr,
                                (socklen_t*)&iAddrSize);
    if (tcp_data->cli_fd < 0) {
        DEBUGPRINT("TCP ERROR: accept tcp client socket fd error!\r\n");
        goto Exit2;
    }
    DEBUGPRINT("TCP: accept socket %d successfully.\r\n", tcp_data->cli_fd);
    n = tcp_data->nodelay;
    setsockopt(tcp_data->srv_fd, IPPROTO_TCP, TCP_NODELAY,
            (const char *) &n, sizeof( n ) );

    /* Process client header */
    recv_sz = recv(tcp_data->cli_fd, cBsdBuf, tcp_data->buf_len, 0);
    if (tcp_data->dualmode == 0) {
        DEBUGPRINT("TCP: Check client header.\r\n");
        /* Run as tcp server. The server is created by user. */
        sys_memcpy(&header, cBsdBuf, sizeof(header));
#ifdef DEBUG_DUAL_MODE
        buffer_dump("Client header:", &header, sizeof(header));
        client_header_dump(&header);
#endif
        if (ntohl(header.flags) & DUAL_MODE_RUN_NOW) {
            /* Dual mode, do a bidirectional test simultaneously */
            /* Create tcp client data to send packets. */
            int ret = client_data_init();
            if (ret != 0) {
                goto Exit1;
            }
            DEBUGPRINT("TCP: dual mode, start client task.\r\n");
            tcp_cli_data->buf_len = ntohl(header.bufferlen);
            if (tcp_cli_data->buf_len == 0)
                tcp_cli_data->buf_len = 1460;
            if (header.mAmount != 0) {
                header.mAmount = ntohl(header.mAmount);
                if (((uint32_t)header.mAmount) > 0x7fffffff) {
                    header.mAmount = (-header.mAmount) / 100;
                    tcp_cli_data->time_limit = header.mAmount * OS_TICK_RATE_HZ + 100;
                } else {
                    tcp_cli_data->size_limit = header.mAmount;
                }
            }
            tcp_cli_data->srv_port = ntohl(header.mPort);
            snprintf(tcp_cli_data->srv_ip, sizeof(tcp_cli_data->srv_ip), "%s", inet_ntoa(sCliAddr.sin_addr));
            tcp_cli_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"tcp_client", NULL,
                            TCP_TEST_STACK_SIZE, 0, TCP_TEST_CLIENT_PRIO + TASK_PRIO_HIGHER(1),
                            (task_func_t)tcp_task_func, tcp_cli_data);
            if (tcp_cli_data->task_hdl == NULL) {
                DEBUGPRINT("TCP ERROR: create tcp client task failed.\r\n");
                goto Exit1;
            }
        }
    } else {
        /* Run as tcp client. The server is auto created since the client is dual mode. */
        /* Not to parse client header. */
    }

    /* waits packets from the connected TCP client */
    rpt_stime = stime = sys_current_time_get();
    while (!tcp_terminate) {
        recv_sz = recv(tcp_data->cli_fd, cBsdBuf, tcp_data->buf_len, 0);
        if (recv_sz < 0) {
            DEBUGPRINT("TCP ERROR: server recv data error. recv_sz is %d.\r\n", recv_sz);
            goto Exit1;
        } else if (recv_sz == 0) {
            break;
        }
        etime = sys_current_time_get();
        tot_sz += recv_sz;
        rpt_sz += recv_sz;
        time = (etime - rpt_stime) * OS_MS_PER_TICK;
        if ((etime - rpt_stime) >= tcp_data->rpt_intvl) {
            if (tcnt == 0)
                DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
            DEBUGPRINT("Recv\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
            tcnt += intvl_sec;
            rpt_stime = etime;
            rpt_sz = 0;
        }
    }
    time = (etime - stime) * OS_MS_PER_TICK;
    DEBUGPRINT("Total\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", 0, tcnt, (uint32_t)(tot_sz >> 10), (uint32_t)((tot_sz << 3) / time));

Exit1:
    /* close the connected socket after receiving from connected TCP client */
    close(tcp_data->cli_fd);

Exit2:
    /* close the listening socket */
    close(tcp_data->srv_fd);

Exit3:
    /* free buffer */
    sys_mfree(cBsdBuf);

    return 0;
}

/*!
    \brief      tcp task function
    \param[in]  param: pointer to the input argument
    \param[out] none
    \retval     none
*/
static void tcp_task_func(void *param)
{
    struct tcp_data_t *tcp_data = (struct tcp_data_t *)param;

    if (tcp_data->is_server == 1) {
        DEBUGPRINT("TCP: start tcp Server!\r\n");

        tcp_server(tcp_data);

        DEBUGPRINT("tcp server task: used stack = %d, free stack = %d\r\n",
                    (TCP_TEST_STACK_SIZE - sys_stack_free_get(NULL)), sys_stack_free_get(NULL));

        DEBUGPRINT("TCP: tcp server stopped!\r\n");
    } else {
        DEBUGPRINT("TCP: start tcp client!\r\n");

        tcp_client(tcp_data);

        DEBUGPRINT("tcp client task: used stack = %d, free stack = %d\r\n",
                    (TCP_TEST_STACK_SIZE - sys_stack_free_get(NULL)), sys_stack_free_get(NULL));

        DEBUGPRINT("TCP: tcp client stopped!\r\n");
    }

    tcp_data->task_hdl = NULL;

    sys_task_delete(NULL);
}

/*!
    \brief      command of tcp
    \param[in]  argc: counter of input argument
    \param[out] argv: pointer to the input argument
    \retval     none
*/
void cmd_tcp(int argc, char **argv)
{
    int arg_cnt = 1, is_server = 0;
    char *endptr = NULL;
    tcp_terminate = 0;

    /* Parse main option */
    if (arg_cnt == 1) {
        if (strcmp(argv[1], "-s") == 0) {
            DEBUGPRINT("\r\nIperf2: start iperf2 server!\r\n");
            int ret = server_data_init();
            if (ret == ERROR_ALREADY_RUNNING || ret == ERROR_NO_MEMORY)
                return;

            arg_cnt += 1;
            is_server = 1;
        } else if (strcmp(argv[1], "-c") == 0) {
            DEBUGPRINT("\r\nIperf2: start iperf2 client!\r\n");
            int ret = client_data_init();
            if (ret == ERROR_ALREADY_RUNNING || ret == ERROR_NO_MEMORY)
                return;

            if (argc >= 3)
                snprintf(tcp_cli_data->srv_ip, sizeof(tcp_cli_data->srv_ip), "%s", argv[2]);
            else
                goto Exit;
            arg_cnt += 2;
        } else if (strcmp(argv[1], "exit") == 0) {
            int cnt = 0;
            tcp_terminate = 1;
            if (tcp_srv_data) {
                while ((tcp_srv_data->task_hdl != NULL) && (cnt++ < 10))
                    sys_ms_sleep(100);
                sys_mfree(tcp_srv_data);
                tcp_srv_data = NULL;
            }
            if (tcp_cli_data) {
                cnt = 0;
                while ((tcp_cli_data->task_hdl != NULL) && (cnt++ < 10))
                    sys_ms_sleep(100);
                sys_mfree(tcp_cli_data);
                tcp_cli_data = NULL;
            }
            return;
        } else {
            goto Exit;
        }
    }

    if (is_server)
        DEBUG_ASSERT(tcp_srv_data != NULL);
    else
        DEBUG_ASSERT(tcp_cli_data != NULL);

    /* Parse other options */
    while (arg_cnt < argc) {
        /* Client/Server */
        if (strcmp(argv[arg_cnt], "-i") == 0) {
            uint32_t intvl = 0;
            if (argc <= (arg_cnt + 1))
                goto Exit;
            intvl = (uint32_t)atoi(argv[arg_cnt + 1]) * OS_TICK_RATE_HZ;
            if (intvl > 3600 * OS_TICK_RATE_HZ) {
                DEBUGPRINT("TCP WARNNING: Report interval is larger than 3600 seconds. Use 3600 seconds instead.\r\n");
                intvl = 3600 * OS_TICK_RATE_HZ;
            }
            if (intvl > 0) {
                if (is_server) {
                    tcp_srv_data->rpt_intvl = intvl;
                } else {
                    tcp_cli_data->rpt_intvl = intvl;
                }
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-l") == 0) {
            uint32_t len = 0;
            if (argc <= (arg_cnt + 1))
                goto Exit;
            len = (uint32_t)atoi(argv[arg_cnt + 1]);
            if (len > 4380) {
                DEBUGPRINT("TCP WARNNING: To save memory, the buffer size is preferably less than 4380. Use 4380 instead.\r\n");
                len = 4380;
            }
            if (len > 0) {
                if (is_server) {
                    tcp_srv_data->buf_len = len;
                } else {
                    tcp_cli_data->buf_len = len;
                }
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-p") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                tcp_srv_data->srv_port = (uint16_t)atoi(argv[arg_cnt + 1]);
            } else {
                tcp_cli_data->srv_port = (uint16_t)atoi(argv[arg_cnt + 1]);
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-N") == 0) {
            if (is_server) {
                tcp_srv_data->nodelay = 1;
            } else {
                tcp_cli_data->nodelay = 1;
            }
            arg_cnt += 2;
        /* Client Only */
        } else if (strcmp(argv[arg_cnt], "-n") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                tcp_cli_data->size_limit = byte_atoi(argv[arg_cnt + 1]);
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-t") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                tcp_cli_data->time_limit = (uint32_t)atoi(argv[arg_cnt + 1]) * OS_TICK_RATE_HZ + 100;
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-d") == 0) {
            if (is_server) {
                goto Exit;
            } else {
                tcp_cli_data->dualmode = 1;
            }
            arg_cnt += 1;
        }  else if (strcmp(argv[arg_cnt], "-S") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                tcp_cli_data->tos = (uint8_t)strtoul(argv[arg_cnt + 1], &endptr, 0);
                if (*endptr != '\0') {
                    DEBUGPRINT("iperf: invalid tos\r\n");
                    goto Exit;
                }
            }
            arg_cnt += 2;
        } else {
            goto Exit;
        }
    }

    /* Create task */
    if (is_server) {
        tcp_srv_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"tcp_server", NULL,
                        TCP_TEST_STACK_SIZE, 0, TCP_TEST_SERVER_PRIO,
                        (task_func_t)tcp_task_func, tcp_srv_data);
        if (tcp_srv_data->task_hdl == NULL) {
            DEBUGPRINT("TCP ERROR: create tcp server task failed.\r\n");
            return;
        }
    } else {
        /* Check report interval */
        if ((tcp_cli_data->size_limit == 0) && (tcp_cli_data->rpt_intvl > tcp_cli_data->time_limit)) {
            tcp_cli_data->rpt_intvl = tcp_cli_data->time_limit;
        }
        tcp_cli_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"tcp_client", NULL,
                        TCP_TEST_STACK_SIZE, 0, TCP_TEST_CLIENT_PRIO,
                        (task_func_t)tcp_task_func, tcp_cli_data);
        if (tcp_cli_data->task_hdl == NULL) {
            DEBUGPRINT("TCP ERROR: create tcp client task failed.\r\n");
            return;
        }

        if (tcp_cli_data->dualmode == 1) {
            int ret = server_data_init();
            if (ret == ERROR_ALREADY_RUNNING || ret == ERROR_NO_MEMORY)
                return;
            tcp_srv_data->srv_port = tcp_cli_data->srv_port;
            tcp_srv_data->dualmode = 1;
            tcp_srv_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"tcp_server", NULL,
                            TCP_TEST_STACK_SIZE, 0, TCP_TEST_SERVER_PRIO + TASK_PRIO_LOWER(1),
                            (task_func_t)tcp_task_func, tcp_srv_data);
            if (tcp_srv_data->task_hdl == NULL) {
                DEBUGPRINT("TCP ERROR: create tcp server task failed.\r\n");
                return;
            }
        }
    }

    return;

Exit:
    DEBUGPRINT("\rIperf2: iperf2 test command format error!\r\n");
    DEBUGPRINT("\r\nUsage: \"iperf -s\" to start iperf2 server or \"iperf -c\" to start iperf2 client or \"iperf -h\" for help\r\n");
}
