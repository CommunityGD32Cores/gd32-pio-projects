/*!
    \file    udp_test.c
    \brief   UDP test for GD32W51x WiFi SDK

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
#include "lwip/stats.h"

// #define DEBUG_DUAL_MODE
#define HEADER_VERSION1          0x80000000
#define DUAL_MODE_RUN_NOW        0x00000001

#define ERROR_ALREADY_RUNNING    1
#define ERROR_NO_MEMORY          -1

struct udp_data_t {
    void *task_hdl;
    int32_t srv_fd;
    int32_t cli_fd;
    uint8_t is_server;
    uint16_t srv_port;        /* server port to listen on/connect to (default 5001) */
    char srv_ip[16];          /* server ip to listen on/connect to */
    uint8_t dualmode;         /* Set dual mode, do a bidirectional test simultaneously */
    uint8_t rsvd;
    uint16_t buf_len;         /* length of buffer to read or write (default 1460 Bytes) */
    uint8_t tos;              /* type of service byte */
    uint32_t rpt_intvl;       /* ticks between periodic bandwidth reports (default 1000 ticks) */
    uint32_t time_limit;      /* time in ticks to transmit for (default 10000 ticks) */
    uint64_t size_limit;      /* number of buffer to transmit for (default use time) */
    uint64_t bandwidth;       /* for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u) */
};

struct udp_datagram {
    int32_t id;
    uint32_t tv_sec;
    uint32_t tv_usec;
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

static struct udp_data_t *udp_srv_data = NULL;
static struct udp_data_t *udp_cli_data = NULL;
static int32_t udp_terminate = 0;
static void udp_task_func(void *param);

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
/*!
    \brief      dump client header
    \param[in]  h: pointer to the input client header structure
                  flags:(???)
                  numThreads:
                  mPort:
                  bufferlen:
                  mWindowSize:
                  mAmount:
                  mRate:
                  mUDPRateUnits:
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

/*!
    \brief      dump udp data
    \param[in]  udp_data: pointer to the input udp data structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  rsvd: reserve
                  buf_len: length of buffer to read or write (default 1472 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
                  bandwidth: for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)
    \param[out] none
    \retval     none
*/
static void dump_udp_data(struct udp_data_t *udp_data)
{
    DEBUGPRINT("\r\n========== dump udp data ============\r\n");
    DEBUGPRINT("task_hdl = %p\r\n", udp_data->task_hdl);
    DEBUGPRINT("srv_fd = %d\r\n", udp_data->srv_fd);
    DEBUGPRINT("cli_fd = %d\r\n", udp_data->cli_fd);
    DEBUGPRINT("is_server = %d\r\n", udp_data->is_server);
    DEBUGPRINT("srv_port = %d\r\n", udp_data->srv_port);
    DEBUGPRINT("srv_ip = %s\r\n", udp_data->srv_ip);
    DEBUGPRINT("dualmode = %d\r\n", udp_data->dualmode);
    DEBUGPRINT("buf_len = %d\r\n", udp_data->buf_len);
    DEBUGPRINT("rpt_intvl = %d\r\n", udp_data->rpt_intvl);
    DEBUGPRINT("time_limit = %d\r\n", udp_data->time_limit);
    DEBUGPRINT("size_limit = %u\r\n", (uint32_t)udp_data->size_limit);
    DEBUGPRINT("bandwidth = %u\r\n", (uint32_t)udp_data->bandwidth);
    DEBUGPRINT("========== dump udp data end ============\r\n");
}
#endif

/*!
    \brief      generate client header
    \param[in]  udp_data: pointer to the input udp data structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  rsvd: reserve
                  buf_len: length of buffer to read or write (default 1472 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
                  bandwidth: for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)
    \param[in]  buf: pointer to the input udp datagram structure
                  id: number of id
                  tv_sec: second vlaue
                   tv_usec: microsecond vlaue
    \param[out] none
    \retval     none
*/
static void client_header_generate(struct udp_data_t *udp_data, char *buf)
{
    struct udp_datagram *udp_hdr = (struct udp_datagram *)buf;
    struct client_hdr *cli_hdr = (struct client_hdr *)(udp_hdr + 1);

    udp_hdr->id = 0;
    udp_hdr->tv_sec = 0;
    udp_hdr->tv_usec = 0;

    if (udp_data->dualmode == 1) {
        cli_hdr->flags = htonl(HEADER_VERSION1 | DUAL_MODE_RUN_NOW);  // 0x01000080
    } else {
        cli_hdr->flags = 0;
    }
    cli_hdr->numThreads = htonl(0x01);
    cli_hdr->mPort = htonl(udp_data->srv_port);
    cli_hdr->bufferlen = htonl(udp_data->buf_len);
    cli_hdr->mWindowSize = htonl(udp_data->bandwidth);
    if (udp_data->size_limit> 0) {
        cli_hdr->mAmount = udp_data->size_limit & 0x7FFFFFFF;
        cli_hdr->mAmount = htonl(cli_hdr->mAmount);
    } else {
        cli_hdr->mAmount = (int32_t)(udp_data->time_limit * 100 / OS_TICK_RATE_HZ);
        cli_hdr->mAmount = htonl(-cli_hdr->mAmount);
    }
    cli_hdr->mRate = htonl(udp_data->bandwidth);
    cli_hdr->mUDPRateUnits = 0;  // kRate_BW

#ifdef DEBUG_DUAL_MODE
    // buffer_dump("Client header:", cli_hdr, sizeof(struct client_hdr));
    client_header_dump(cli_hdr);
#endif
}

/*!
    \brief      initialize client data
    \param[in]  none
    \param[out] none
    \retval     function run status
      \arg        ERROR_ALREADY_RUNNING: tcp server is already running
      \arg        ERROR_NO_MEMORY: No memory for tcp data
      \arg        0: run success
*/
static int client_data_init(void)
{
    if (udp_cli_data) {
        if (udp_cli_data->task_hdl != NULL) {
            DEBUGPRINT("UDP: UDP client is already running.\r\n");
            return ERROR_ALREADY_RUNNING;
        }
    } else {
        udp_cli_data = sys_malloc(sizeof(struct udp_data_t));
        if (udp_cli_data == NULL) {
            DEBUGPRINT("UDP: No memory for udp_data.\r\n");
            return ERROR_NO_MEMORY;
        }
    }

    sys_memset(udp_cli_data, 0, sizeof(struct udp_data_t));
    udp_cli_data->cli_fd = -1;
    udp_cli_data->buf_len = 1460;
    udp_cli_data->srv_port = 5001;
    udp_cli_data->tos = 0;
    udp_cli_data->rpt_intvl = 1 * OS_TICK_RATE_HZ;  // default 1s
    udp_cli_data->time_limit = 10 * OS_TICK_RATE_HZ + 100;  // default 10s
    udp_cli_data->bandwidth = (1 * kMega_to_Unit) >> 8;  // default 1Mbps
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
    if (udp_srv_data) {
        if (udp_srv_data->task_hdl != NULL) {
            DEBUGPRINT("UDP WARNING: UDP server is already running.\r\n");
            return ERROR_ALREADY_RUNNING;
        }
    } else {
        udp_srv_data = sys_malloc(sizeof(struct udp_data_t));
        if (udp_srv_data == NULL) {
            DEBUGPRINT("UDP ERROR: No memory for udp_data.\r\n");
            return ERROR_NO_MEMORY;
        }
    }

    sys_memset(udp_srv_data, 0, sizeof(struct udp_data_t));
    udp_srv_data->is_server = 1;
    udp_srv_data->srv_fd = -1;
    udp_srv_data->cli_fd = -1;
    udp_srv_data->buf_len = 1460;
    udp_srv_data->srv_port = 5001;
    udp_srv_data->time_limit = 10 * OS_TICK_RATE_HZ;  // default 10s
    udp_srv_data->rpt_intvl = 1 * OS_TICK_RATE_HZ;  // default 1s

    return 0;
}

/*!
    \brief      udp client function
     \param[in]  udp_data: pointer to the input udp data structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  rsvd: reserve
                  buf_len: length of buffer to read or write (default 1472 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
                  bandwidth: for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)
    \param[out] none
    \retval     none
    \retval     function run status
                  -1: allocate client buffer failed
                  0: run success
*/
static int udp_client(struct udp_data_t *udp_data)
{
    struct sockaddr_in sAddr;
    socklen_t iAddrSize = sizeof(struct sockaddr_in);
    char *cBsdBuf = NULL;
    int iCounter, iStatus;
    uint32_t stime, etime, rpt_stime, bw_stime;  // ticks
    uint32_t m;
    uint64_t rpt_sz = 0, tot_sz = 0, bw_sz = 0;
    uint32_t time = 0, tcnt = 0;  // seconds
    uint32_t intvl_sec = udp_data->rpt_intvl / OS_TICK_RATE_HZ;
    struct udp_datagram *udp_hdr = NULL;
    uint32_t pkt_cnt = 0;

#ifdef DEBUG_DUAL_MODE
    dump_udp_data(udp_data);
#endif

    cBsdBuf = sys_malloc(udp_data->buf_len);
    if (NULL == cBsdBuf) {
        DEBUGPRINT("UDP ERROR: allocate client buffer failed (len = %d).\r\n", udp_data->buf_len);
        return -1;
    }

    for (iCounter = 0; iCounter < udp_data->buf_len; iCounter++) {
        cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    client_header_generate(udp_data, cBsdBuf);
    udp_hdr = (struct udp_datagram *)cBsdBuf;

    FD_ZERO(&sAddr);
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(udp_data->srv_port);
    sAddr.sin_addr.s_addr = inet_addr(udp_data->srv_ip);

    /* creating a UDP socket */
    udp_data->cli_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_data->cli_fd < 0) {
        DEBUGPRINT("UDP ERROR: create UDP client socket fd error!\r\n");
        goto Exit;
    }

    /* Set IP TOS */
    if (udp_data->tos) {
        m = udp_data->tos;
        if (setsockopt(udp_data->cli_fd, IPPROTO_IP, IP_TOS, &m, sizeof(m)) < 0) {
            DEBUGPRINT("UDP ERROR:set socket tos %d error, errno = %d!\r\n", m, errno);
            goto Exit;
        }
    }

    DEBUGPRINT("UDP: server IP=%s port=%d.\r\n", udp_data->srv_ip, udp_data->srv_port);
    DEBUGPRINT("UDP: create socket %d.\r\n", udp_data->cli_fd);

    bw_stime = rpt_stime = etime = stime = sys_current_time_get();
    if (udp_data->size_limit != 0) {
        while (!udp_terminate && (tot_sz < udp_data->size_limit)) {
            iStatus = sendto(udp_data->cli_fd, cBsdBuf, udp_data->buf_len, 0, (struct sockaddr *)&sAddr, iAddrSize);
            if (iStatus <= 0) {
                /* low_level_output return ERR_MEM because wifi skb is not enough */
                // DEBUGPRINT("UDP ERROR: UDP client send data error (iStatus = %d).\r\n", iStatus);
            }
            pkt_cnt++;
            udp_hdr->id = htonl(pkt_cnt);
            tot_sz += udp_data->buf_len;
            rpt_sz += udp_data->buf_len;
            bw_sz += udp_data->buf_len;
            etime = sys_current_time_get();

            if ((bw_sz >= udp_data->bandwidth) && ((etime - bw_stime) < 1 * OS_TICK_RATE_HZ)) {
                sys_ms_sleep((1 * OS_TICK_RATE_HZ - (etime - bw_stime)) / OS_MS_PER_TICK);
                etime = sys_current_time_get();
                bw_stime = etime;
                bw_sz = 0;
            }

            time = (etime - rpt_stime) * OS_MS_PER_TICK;
            if ((etime - rpt_stime) >= udp_data->rpt_intvl) {
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Send\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
                bw_stime = etime;
                bw_sz = 0;
            }
        }
    } else {
        while (!udp_terminate && ((etime - stime) <= udp_data->time_limit)) {
            iStatus = sendto(udp_data->cli_fd, cBsdBuf, udp_data->buf_len, 0, (struct sockaddr *)&sAddr, iAddrSize);
            if (iStatus <= 0) {
                /* low_level_output return ERR_MEM because wifi skb is not enough */
                // DEBUGPRINT("UDP ERROR: UDP client send data error (iStatus = %d).\r\n", iStatus);
            }
            pkt_cnt++;
            udp_hdr->id = htonl(pkt_cnt);
            tot_sz += udp_data->buf_len;
            rpt_sz += udp_data->buf_len;
            bw_sz += udp_data->buf_len;

            etime = sys_current_time_get();
            if ((bw_sz >= udp_data->bandwidth) && ((etime - bw_stime) < OS_TICK_RATE_HZ)) {
                sys_ms_sleep((1 * OS_TICK_RATE_HZ - (etime - bw_stime)) / OS_MS_PER_TICK);
                etime = sys_current_time_get();
                bw_stime = etime;
                bw_sz = 0;
            }

            time = (etime - rpt_stime) * OS_MS_PER_TICK;
            if ((etime - rpt_stime) >= udp_data->rpt_intvl) {
                // DEBUGPRINT("etime=%d stime=%d limit=%d\r\n", etime, stime, udp_data->time_limit);
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Send\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
                bw_stime = etime;
                bw_sz = 0;
            }
        }
    }

    time = (etime - stime) * OS_MS_PER_TICK;
    DEBUGPRINT("Total\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", 0, tcnt, (uint32_t)(tot_sz >> 10), (uint32_t)((tot_sz << 3) / time));

    /* Inform server to end this transfer */
    udp_hdr->id = -1;
    sendto(udp_data->cli_fd, cBsdBuf, sizeof(struct udp_datagram), 0, (struct sockaddr *)&sAddr, iAddrSize);
    sys_ms_sleep(100);

    close(udp_data->cli_fd);

Exit:
    sys_mfree(cBsdBuf);
    return 0;
}

/*!
    \brief      udp server function
    \param[in]  udp_data: pointer to the input udp data structure
                  task_hdl: task handle
                  srv_fd: server socket file descriptor
                  cli_fd: client socket file descriptor
                  is_server: is server falg
                  srv_port: server port to listen on/connect to (default 5001)
                  srv_ip: server ip to listen on/connect to
                  dualmode: Set dual mode, do a bidirectional test simultaneously
                  rsvd: reserve
                  buf_len: length of buffer to read or write (default 1472 Bytes)
                  tos: type of service byte
                  rpt_intvl: ticks between periodic bandwidth reports (default 1000 ticks)
                  time_limit: time in ticks to transmit for (default 10000 ticks)
                  size_limit: number of buffer to transmit for (default use time)
                  bandwidth: for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)
    \param[out] none
    \retval     none
    \retval     function run status
      \arg        -1: allocate server buffer failed
      \arg        0: run success
*/
static int udp_server(struct udp_data_t *udp_data)
{
    struct sockaddr_in  sLocalAddr;
    struct sockaddr_in  sCliAddr;
    socklen_t iAddrSize = sizeof(sLocalAddr);
    int iCounter, iStatus, n = 1, recv_sz;
    char *cBsdBuf = NULL;
    uint32_t stime, etime, rpt_stime;  // ticks
    uint64_t rpt_sz = 0, tot_sz = 0;
    uint32_t time, tcnt = 0;  // ms
    uint32_t intvl_sec = udp_data->rpt_intvl / OS_TICK_RATE_HZ;
    struct udp_datagram udp_hdr;
    struct client_hdr header;

#ifdef DEBUG_DUAL_MODE
    dump_udp_data(udp_data);
#endif

    cBsdBuf = sys_malloc(udp_data->buf_len);
    if (NULL == cBsdBuf) {
        DEBUGPRINT("UDP ERROR: allocate server buffer failed (len = %d).\r\n", udp_data->buf_len);
        return -1;
    }

    for (iCounter = 0; iCounter < udp_data->buf_len; iCounter++) {
        cBsdBuf[iCounter] = (char)(iCounter % 10);
    }

    udp_data->srv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_data->srv_fd < 0) {
        goto Exit2;
    }

    DEBUGPRINT("UDP: create server socket %d.\r\n", udp_data->srv_fd);
    setsockopt(udp_data->srv_fd, SOL_SOCKET, SO_REUSEADDR,
            (const char *) &n, sizeof( n ) );

    sys_memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family      = AF_INET;
    sLocalAddr.sin_len         = sizeof(sLocalAddr);
    sLocalAddr.sin_port        = htons(udp_data->srv_port);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* binding the UDP socket to the UDP server address */
    iStatus = bind(udp_data->srv_fd, (struct sockaddr *)&sLocalAddr, iAddrSize);
    if (iStatus < 0) {
        DEBUGPRINT("UDP ERROR: bind udp server socket fd error!\r\n");
        goto Exit1;
    }
    DEBUGPRINT("UDP: bind successfully.\r\n");


    /* Process client header */
    recv_sz = recvfrom(udp_data->srv_fd, cBsdBuf, udp_data->buf_len, 0, (struct sockaddr *)&sLocalAddr, &iAddrSize);
    tot_sz += recv_sz;
    rpt_sz += recv_sz;
    etime = rpt_stime = stime = sys_current_time_get();
    if (udp_data->dualmode == 0) {
        /* Run as udp server. The server is created by user. */
        DEBUGPRINT("UDP: Check client header.\r\n");
        sys_memcpy(&udp_hdr, cBsdBuf, sizeof(udp_hdr));
        sys_memcpy(&header, cBsdBuf + sizeof(udp_hdr), sizeof(header));
#ifdef DEBUG_DUAL_MODE
        // buffer_dump("Clent header:", &header, sizeof(header));
        client_header_dump(&header);
#endif
        if (ntohl(header.flags) & DUAL_MODE_RUN_NOW) {
            /* Dual mode, do a bidirectional test simultaneously */
            /* Create udp client data to send packets. */
            int ret = client_data_init();
            if (ret != 0) {
                goto Exit1;
            }
            DEBUGPRINT("UDP: dual mode, start client task.\r\n");
            udp_cli_data->buf_len = ntohl(header.bufferlen);
            if (udp_cli_data->buf_len == 0)
                udp_cli_data->buf_len = 1460;
            if (header.mAmount != 0) {
                header.mAmount = ntohl(header.mAmount);
                if (((uint32_t)header.mAmount) > 0x7fffffff) {
                    header.mAmount = (-header.mAmount) / 100;
                    udp_cli_data->time_limit = header.mAmount * OS_TICK_RATE_HZ + 100;
                } else {
                    udp_cli_data->size_limit = header.mAmount;
                }
            }
            udp_cli_data->srv_port = ntohl(header.mPort);
            udp_cli_data->bandwidth = (uint32_t)(ntohl(header.mWindowSize) >> 3);
            DEBUGPRINT("UDP: client bandwidth is %d.\r\n", udp_cli_data->bandwidth);
            snprintf(udp_cli_data->srv_ip, sizeof(udp_cli_data->srv_ip), "%s", inet_ntoa(sCliAddr.sin_addr));
#ifdef DEBUG_DUAL_MODE
            dump_udp_data(udp_data);
#endif
            udp_cli_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"udp_client", NULL,
                            UDP_TEST_STACK_SIZE, 0, UDP_TEST_CLIENT_PRIO + TASK_PRIO_HIGHER(1),
                            (task_func_t)udp_task_func, udp_cli_data);
            if (udp_cli_data->task_hdl == NULL) {
                DEBUGPRINT("UDP ERROR: create udp client task failed.\r\n");
                goto Exit1;
            }
        } else {
            if (header.mAmount != 0) {
                header.mAmount = ntohl(header.mAmount);
                if (((uint32_t)header.mAmount) > 0x7fffffff) {
                    header.mAmount = (-header.mAmount) / 100;
                    udp_data->time_limit = header.mAmount * OS_TICK_RATE_HZ + 100;
                    DEBUGPRINT("UDP: time limit is %d.\r\n", udp_data->time_limit);
                } else {
                    udp_data->size_limit = header.mAmount;
                    DEBUGPRINT("UDP: size limit is %d.\r\n", udp_data->size_limit);
                }
            }
        }
    } else {
        /* Run as udp client. This server is auto created since the client is dual mode. */
    }

    /* Waits packets from the UDP client */
    if (udp_data->size_limit > 0) {
        while (!udp_terminate && (tot_sz < udp_data->size_limit)) {
            recv_sz = recvfrom(udp_data->srv_fd, cBsdBuf, udp_data->buf_len, 0, (struct sockaddr *)&sLocalAddr, &iAddrSize);
            if (recv_sz < 0) {
                DEBUGPRINT("UDP ERROR: server recv data error. recv_sz is %d.\r\n", recv_sz);
                goto Exit1;
            }

            etime = sys_current_time_get();
            tot_sz += recv_sz;
            rpt_sz += recv_sz;
            time = (etime - rpt_stime) * OS_MS_PER_TICK;
            if ((etime - rpt_stime) >= udp_data->rpt_intvl) {
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Recv\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
            }
        }
    } else {
        while (!udp_terminate && ((etime - stime) <= udp_data->time_limit)) {
            recv_sz = recvfrom(udp_data->srv_fd, cBsdBuf, udp_data->buf_len, 0, (struct sockaddr *)&sLocalAddr, &iAddrSize);
            if (recv_sz < 0) {
                DEBUGPRINT("UDP ERROR: server recv data error. recv_sz is %d.\r\n", recv_sz);
                goto Exit1;
            }

            etime = sys_current_time_get();
            tot_sz += recv_sz;
            rpt_sz += recv_sz;
            time = (etime - rpt_stime) * OS_MS_PER_TICK;
            if ((etime - rpt_stime) >= udp_data->rpt_intvl) {
                if (tcnt == 0)
                    DEBUGPRINT("\t\tInterval\t\tTransfer\t\tBandwidth\r\n");
                DEBUGPRINT("Recv\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", tcnt, (tcnt + intvl_sec), (uint32_t)(rpt_sz >> 10), (uint32_t)((rpt_sz << 3) / time));
                // DEBUGPRINT("etime=%d stime=%d limit=%d\r\n", etime, stime, udp_data->time_limit);
                tcnt += intvl_sec;
                rpt_stime = etime;
                rpt_sz = 0;
            }
        }
    }
    time = (etime - stime) * OS_MS_PER_TICK;
    DEBUGPRINT("Total\t%4d - %4d sec\t\t%4d KBytes\t\t%5d Kbps\r\n", 0, tcnt, (uint32_t)(tot_sz >> 10), (uint32_t)((tot_sz << 3) / time));

Exit1:
    /* close the server socket */
    close(udp_data->srv_fd);

Exit2:
    /* free buffer */
    sys_mfree(cBsdBuf);

    return 0;
}

/*!
    \brief      udp task function
    \param[in]  param: pointer to the input argument
    \param[out] none
    \retval     none
*/
static void udp_task_func(void *param)
{
    struct udp_data_t *udp_data = (struct udp_data_t *)param;

    if (udp_data->is_server == 1) {
        DEBUGPRINT("UDP: start udp server!\r\n");

        udp_server(udp_data);

        DEBUGPRINT("UDP server task: used stack = %d, free stack = %d\r\n",
                    (UDP_TEST_STACK_SIZE - sys_stack_free_get(NULL)), sys_stack_free_get(NULL));

        DEBUGPRINT("UDP: udp server stopped!\r\n");
    } else {
        DEBUGPRINT("UDP: start udp client!\r\n");

        udp_client(udp_data);

        DEBUGPRINT("UDP client task: used stack = %d, free stack = %d\r\n",
                    (UDP_TEST_STACK_SIZE - sys_stack_free_get(NULL)), sys_stack_free_get(NULL));

        DEBUGPRINT("UDP: udp client stopped!\r\n");
    }

    udp_data->task_hdl = NULL;

    sys_task_delete(NULL);
}

/*!
    \brief      command of tcp
    \param[in]  argc: counter of input argument
    \param[out] argv: pointer to the input argument
    \retval     none
*/
void cmd_udp(int argc, char **argv)
{
    int arg_cnt = 1, is_server = 0;
    char *endptr = NULL;
    udp_terminate = 0;

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
                snprintf(udp_cli_data->srv_ip, sizeof(udp_cli_data->srv_ip), "%s", argv[2]);
            else
                goto Exit;
            arg_cnt += 2;
        } else if (strcmp(argv[1], "exit") == 0) {
            int cnt = 0;
            udp_terminate = 1;
            if (udp_srv_data) {
                while ((udp_srv_data->task_hdl != NULL) && (cnt++ < 10))
                    sys_ms_sleep(100);
                sys_mfree(udp_srv_data);
                udp_srv_data = NULL;
            }
            if (udp_cli_data) {
                cnt = 0;
                while ((udp_cli_data->task_hdl != NULL) && (cnt++ < 10))
                    sys_ms_sleep(100);
                sys_mfree(udp_cli_data);
                udp_cli_data = NULL;
            }
            return;
        } else {
            goto Exit;
        }
    }

    if (is_server)
        DEBUG_ASSERT(udp_srv_data != NULL);
    else
        DEBUG_ASSERT(udp_cli_data != NULL);

    /* Parse other options */
    while (arg_cnt < argc) {
        /* Client/Server */
        if (strcmp(argv[arg_cnt], "-i") == 0) {
            uint32_t intvl = 0;
            if (argc <= (arg_cnt + 1))
                goto Exit;
            intvl = (uint32_t)atoi(argv[arg_cnt + 1]) * OS_TICK_RATE_HZ;
            if (intvl > 3600 * OS_TICK_RATE_HZ) {
                DEBUGPRINT("UDP WARNNING: Report interval is larger than 3600 seconds. Use 3600 seconds instead.\r\n");
                intvl = 3600 * OS_TICK_RATE_HZ;
            }
            if (intvl > 0) {
                if (is_server) {
                    udp_srv_data->rpt_intvl = intvl;
                } else {
                    udp_cli_data->rpt_intvl = intvl;
                }
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-l") == 0) {
            uint32_t len = 0;
            if (argc <= (arg_cnt + 1))
                goto Exit;
            len = (uint32_t)atoi(argv[arg_cnt + 1]);
            if (len > 5000) {
                DEBUGPRINT("UDP WARNNING: To save memory, the buffer size is preferably less than 5K. Use 5K instead.\r\n");
                len = 5000;
            }
            if (len > 0) {
                if (is_server) {
                    udp_srv_data->buf_len = len;
                } else {
                    udp_cli_data->buf_len = len;
                }
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-p") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                udp_srv_data->srv_port = (uint16_t)atoi(argv[arg_cnt + 1]);
            } else {
                udp_cli_data->srv_port = (uint16_t)atoi(argv[arg_cnt + 1]);
            }
            arg_cnt += 2;
        /* Client Only */
        } else if (strcmp(argv[arg_cnt], "-b") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                udp_cli_data->bandwidth = (byte_atoi(argv[arg_cnt + 1]) >> 3);  // unit: Bytes per second
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-n") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                udp_cli_data->size_limit = byte_atoi(argv[arg_cnt + 1]);
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-t") == 0) {
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                udp_cli_data->time_limit = (uint32_t)atoi(argv[arg_cnt + 1]) * OS_TICK_RATE_HZ + 100;
            }
            arg_cnt += 2;
        } else if (strcmp(argv[arg_cnt], "-d") == 0) {
            if (is_server) {
                goto Exit;
            } else {
                udp_cli_data->dualmode = 1;
            }
            arg_cnt += 1;
        } else if (strcmp(argv[arg_cnt], "-u") == 0){
            arg_cnt += 1;/* ignore -u option */
        } else if (strcmp(argv[arg_cnt], "-S") == 0){
            if (argc <= (arg_cnt + 1))
                goto Exit;
            if (is_server) {
                goto Exit;
            } else {
                udp_cli_data->tos = (uint8_t)strtoul(argv[arg_cnt + 1], &endptr, 0);
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

    /* Check report interval */
    if ((udp_cli_data->size_limit == 0) && (udp_cli_data->rpt_intvl > udp_cli_data->time_limit)) {
        udp_cli_data->rpt_intvl = udp_cli_data->time_limit;
    }

    /* Create task */
    if (is_server) {
        udp_srv_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"udp_server", NULL,
                        UDP_TEST_STACK_SIZE, 0, UDP_TEST_SERVER_PRIO,
                        (task_func_t)udp_task_func, udp_srv_data);
        if (udp_srv_data->task_hdl == NULL) {
            DEBUGPRINT("UDP ERROR: create udp server task failed.\r\n");
            return;
        }
    } else {
        udp_cli_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"udp_client", NULL,
                        UDP_TEST_STACK_SIZE, 0, UDP_TEST_CLIENT_PRIO,
                        (task_func_t)udp_task_func, udp_cli_data);
        if (udp_cli_data->task_hdl == NULL) {
            DEBUGPRINT("UDP ERROR: create udp client task failed.\r\n");
            return;
        }

        if (udp_cli_data->dualmode == 1) {
            int ret = server_data_init();
            if (ret == ERROR_ALREADY_RUNNING || ret == ERROR_NO_MEMORY)
                return;
            udp_srv_data->srv_port = udp_cli_data->srv_port;
            udp_srv_data->dualmode = 1;
            udp_srv_data->time_limit = udp_cli_data->time_limit - 100;
            udp_srv_data->size_limit = udp_cli_data->size_limit;
            udp_srv_data->task_hdl = sys_task_create(NULL, (const uint8_t *)"udp_server", NULL,
                            UDP_TEST_STACK_SIZE, 0, UDP_TEST_SERVER_PRIO + TASK_PRIO_LOWER(1),
                            (task_func_t)udp_task_func, udp_srv_data);
            if (udp_srv_data->task_hdl == NULL) {
                DEBUGPRINT("UDP ERROR: create udp server task failed.\r\n");
                return;
            }
        }
    }

    return;

Exit:
    DEBUGPRINT("\rIperf2: iperf2 test command format error!\r\n");
    DEBUGPRINT("\r\nUsage: \"iperf -s\" to start iperf2 server or \"iperf -c\" to start iperf2 client or \"iperf -h\" for help\r\n");
}
