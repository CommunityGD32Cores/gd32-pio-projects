#ifndef LWIP_PING_H
#define LWIP_PING_H

#include "lwip/ip_addr.h"

#define PING_TASK_STACK_SIZE                   256
#define PING_TASK_PRIO                         16

struct ping_info_t {
    /* ping parameters */
#if LWIP_IPV6
    char ping_ip[64];
    u8_t ip_type;
#else
    char ping_ip[16];
#endif
    u32_t ping_cnt;
    size_t ping_size;
    u32_t ping_interval;

    /* ping variables */
    u16_t ping_seq_num;
    u16_t ping_max_delay;
    u16_t ping_min_delay;
    u32_t ping_avg_delay;
    u32_t ping_time;
    u32_t ping_recv_count;
    u8_t *reply_buf;
};

err_t ping(struct ping_info_t *ping_info);
void cmd_ping(int argc, char **argv);

#endif /* LWIP_PING_H */
