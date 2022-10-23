#ifndef DHCPD_H_
#define DHCPD_H_

//#include "portmacro.h"
#include "lwip/inet.h"
#include "leases.h"
#include "stdint.h"
struct dhcpd
{
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t cookie;
    uint8_t options[308]; /* 312 - cookie */
} PACK_STRUCT_STRUCT;

struct server_config_t
{
    struct in_addr  server;
    struct in_addr  mask;
    struct in_addr  start;
    struct in_addr  end;

    char *interface;        /* The name of the interface to use */
    uint8_t arp[6];         /* Our arp address */
    uint32_t lease;         /* lease time in seconds (host order) */
    uint32_t max_leases;    /* maximum number of leases (including reserved address) */
    uint32_t auto_time;     /* how long should udhcpd wait before writing a config file if this is zero, it will only write one on SIGUSR1 */
    uint32_t decline_time;  /* how long an address is reserved if a client returns a decline message */
    uint32_t conflict_time; /* how long an arp conflict offender is leased for */
    uint32_t offer_time;    /* how long an offered address is reserved */
    uint32_t min_lease;     /* minimum lease a client can request*/

    struct in_addr  siaddr; /* next server bootp option */
    char *sname;            /* bootp server name */
    char *boot_file;        /* bootp boot file option */
};

void dhcpd_daemon(void);
void stop_dhcpd_daemon(void);
#endif /* DHCPD_H_ */
