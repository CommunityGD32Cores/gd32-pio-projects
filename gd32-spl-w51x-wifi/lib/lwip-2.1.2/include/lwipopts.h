#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

#include "app_cfg.h"
#include "stdint.h"
#define MEM_ALIGNMENT               4

#ifdef CONFIG_IPV6_SUPPORT
#define LWIP_IPV6                   1
#define LWIP_IPV6_DHCP6             0
#define LWIP_IPV6_MLD               0
#endif

#ifndef CONFIG_WIFI_HIGH_PERFORMANCE
/*====================================================================*/
/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                    (5 * 1024)

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF               24  // 12

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_NETBUF             6
#define MEMP_NUM_NETCONN            6
#define MEMP_NUM_UDP_PCB            6

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB            4

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN     3

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG            TCP_SND_QUEUELEN  // 16

#ifdef CONFIG_IPV6_SUPPORT
#define MEMP_NUM_SYS_TIMEOUT        11
#else
#define MEMP_NUM_SYS_TIMEOUT        8
#endif

#define MEMP_NUM_TCPIP_MSG_INPKT    16  // For CONFIG_RX_REORDER_SUPPORT
/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE              17  // 24

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE           512

#define IP_REASS_MAX_PBUFS          (PBUF_POOL_SIZE - 4)

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
//#define TCP_QUEUE_OOSEQ         0

/* TCP Maximum segment size. */
#define TCP_MSS                     (1500 - 40)      /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
#define TCP_SND_BUF                 (6 * TCP_MSS)  // (4 * TCP_MSS)

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN            (4 * TCP_SND_BUF / TCP_MSS)

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well
 */
#define TCP_WND                     (5 * TCP_MSS)  // (7 * TCP_MSS)

#else /* CONFIG_WIFI_HIGH_PERFORMANCE */
/*====================================================================*/
/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                    (10 * 1024)


/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF               24//100

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_NETBUF             6
#define MEMP_NUM_NETCONN            6

#define MEMP_NUM_UDP_PCB            6

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB            5
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN     5

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG            TCP_SND_QUEUELEN

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE              16

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#define PBUF_POOL_BUFSIZE           1536

#define IP_REASS_MAX_PBUFS          14

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ             1

/* TCP Maximum segment size. */
#define TCP_MSS                     (1500 - 40)      /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
#define TCP_SND_BUF                 (12 * TCP_MSS)

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN            (4 * TCP_SND_BUF / TCP_MSS)

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well
 */
#define TCP_WND                     (13 * TCP_MSS)

#define TCP_WND_UPDATE_THRESHOLD    (TCP_WND / 4)    //(TCP_WND / 4)

// #define MEMP_NUM_TCPIP_MSG_INPKT    13

#endif  /* CONFIG_WIFI_HIGH_PERFORMANCE */
/*====================================================================*/

#define CHECKSUM_CHECK_TCP          0

#define LWIP_UDP                    1

#define LWIP_RAW                    1

#define LWIP_DHCP                   1

#define LWIP_NETIF_API              1

#define LWIP_STATS                  0
#define LWIP_STATS_DISPLAY          0

#define LWIP_PROVIDE_ERRNO          1

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                1
/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                 1

#define LWIP_IGMP                   1

extern uint32_t trng_get(void);
#define LWIP_RAND()                 trng_get()

#define LWIP_DNS                    1

#ifdef CONFIG_IPERF_TEST
#define SO_REUSE                    1
#define ERRNO
#endif

#ifdef CONFIG_TELNET_SERVER
    #define LWIP_TCP_KEEPALIVE      1
#endif
/*
    OTHER
*/


#define TCPIP_THREAD_STACKSIZE      LWIP_TASK_STK_POOL_SIZE
#define TCPIP_THREAD_PRIO           LWIP_TASK_TCPIP_PRIO


#define LWIP_TCPIP_CORE_LOCKING     0

#define LWIP_COMPAT_MUTEX           1

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE             12  // 8
/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE   8    //5

#define DEFAULT_UDP_RECVMBOX_SIZE   8    //5
/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE     5

#define DEFAULT_RAW_RECVMBOX_SIZE   8

#define LWIP_DHCPD                  1

#define LWIP_PING                   1

#define LWIP_SO_RCVTIMEO            1

#define LWIP_UDP_ECHO               0

#define LWIP_TCP_ECHO               0

#define LWIP_SO_SNDRCVTIMEO_NONSTANDARD     1

extern void sys_memcpy(void *des, const void *src, uint32_t n);
#define MEMCPY(dst,src,len)         sys_memcpy(dst,src,len)
#define MEMCPY1(dst,src,len)        sys_memcpy1(dst,src,len)
#define SMEMCPY(dst,src,len)        sys_memcpy(dst,src,len)

/*
    DEBUG
*/
#define LWIP_DEBUG                  0

#define LWIP_DBG_TYPES_ON           LWIP_DBG_ON  // LWIP_DBG_TRACE
#define LWIP_DBG_MIN_LEVEL          LWIP_DBG_LEVEL_ALL

// #define ETHARP_DEBUG             LWIP_DBG_ON
// #define IP_DEBUG                 LWIP_DBG_ON
// #define TCP_QLEN_DEBUG           LWIP_DBG_ON
// #define TCP_DEBUG                LWIP_DBG_ON
// #define TCP_OUTPUT_DEBUG         LWIP_DBG_ON
// #define TCP_INPUT_DEBUG          LWIP_DBG_ON
// #define UDP_DEBUG                LWIP_DBG_ON
// #define DHCP_DEBUG               LWIP_DBG_ON
// #define ICMP_DEBUG               LWIP_DBG_ON

// #define IGMP_DEBUG               LWIP_DBG_ON

// #define SOCKETS_DEBUG            LWIP_DBG_ON

// #define TCP_CWND_DEBUG           LWIP_DBG_ON

// #define TIMERS_DEBUG             LWIP_DBG_ON

#endif
