#include "arch/sys_arch.h"
#include "dhcpd.h"
#include "common_subr.h"
#include "lwip/api.h"
#include "dhcpd_conf.h"
#include "leases.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"

#if LWIP_DHCPD

struct dhcpOfferedAddr leases[5];
struct dhcpd payload_out;
struct server_config_t server_config;
struct udp_pcb *UdpPcb = NULL;
int    pass_wan_domain = 0;
#define DHCP_SERVER_PORT  67

extern void wifi_netlink_indicate_softap_sta_add(uint8_t *mac_addr, uint32_t ip_addr);

//pickup what i want according to "code" in the packet, "dest" callback
unsigned char *dhcpd_pickup_opt(struct dhcpd *packet, int code, int dest_len, void *dest)
{
    int i, length;
    unsigned char *ptr;
    int over = 0, done = 0, curr = OPTION_FIELD;
    unsigned char len;

    ptr = packet->options;
    i = 0;
    length = 308;
    while (!done)
    {
        if (i >= length)
        {
            //printf( "Option fields too long.");
            return 0;
        }

        if (ptr[i + OPT_CODE] == code)
        {
            if (i + 1 + ptr[i + OPT_LEN] >= length)
            return 0;

            if (dest)
            {
                len = ptr[i + OPT_LEN];
                if (len > dest_len)
                {
                    //printf( "Option fields too long to fit in dest.");
                    return 0;
                }

                sys_memcpy(dest, &ptr[i + OPT_DATA], (int)len);
            }

            return &ptr[i + OPT_DATA];
        }

        switch (ptr[i + OPT_CODE])
        {
            case DHCP_PADDING:
            i++;
            break;

            case DHCP_OPTION_OVER:
            if (i + 1 + ptr[i + OPT_LEN] >= length)
            return 0;

            over = ptr[i + 3];
            i += ptr[OPT_LEN] + 2;
            break;

            case DHCP_END:
            if (curr == OPTION_FIELD && over & FILE_FIELD)
            {
                ptr = packet->file;
                i = 0;
                length = 128;
                curr = FILE_FIELD;
            }
            else if (curr == FILE_FIELD && over & SNAME_FIELD)
            {
                ptr = packet->sname;
                i = 0;
                length = 64;
                curr = SNAME_FIELD;
            }
            else
            done = 1;
            break;

            default:
            i += ptr[OPT_LEN + i] + 2;
        }
    }

    return 0;
}

int dhcpd_add_option(unsigned char *ptr, unsigned char code, unsigned char len, void *data)
{
    int end;

    // Search DHCP_END
    end = 0;
    while (ptr[end] != DHCP_END)
    {
        if (ptr[end] == DHCP_PADDING)
        end++;
        else
        end += ptr[end + OPT_LEN] + 2; // 2 is opt_code and opt_len cost 2 bytes
    }

    if (end + len + 2 + 1 >= 308)
    {
        //printf( "Option 0x%02x cannot not fit into the packet!", code);
        return 0;
    }

    ptr += end; //rebuild pointer

    ptr[OPT_CODE] = code;
    ptr[OPT_LEN] = len; // bytes number of data stored in option
    sys_memcpy(&ptr[OPT_DATA], data, len);

    // Reassign DHCP_END
    ptr += (len+2);
    *ptr = DHCP_END;
    return (len + 2); // return this operation costs option bytes number
}

struct dhcpOfferedAddr *DHCPD_FindLeaseByYiaddr(struct in_addr yiaddr)
{
    unsigned int i;

    for (i = 0; i < server_config.max_leases; i++)
    {
        if (leases[i].yiaddr.s_addr == yiaddr.s_addr)
        return &leases[i];
    }
    return 0;
}

struct in_addr DHCPD_FindAddress()
{
    uint32_t addr;
    struct in_addr ret;
    struct dhcpOfferedAddr *lease = 0;
    for(addr=ntohl(server_config.start.s_addr);
    addr <= ntohl(server_config.end.s_addr);
    addr++)
    {
        // ie, xx.xx.xx.0 or xx.xx.xx.255 or itself
        if ((addr & 0xFF) == 0 ||
        (addr & 0xFF) == 0xFF ||
        (addr == ntohl(server_config.server.s_addr)) )
        {
            continue;
        }
        ret.s_addr = ntohl(addr);

        lease = DHCPD_FindLeaseByYiaddr(ret);
        if (lease  == 0)
        {
            return ret;
        }
    }
    ret.s_addr = 0;
    return ret;
}


struct dhcpOfferedAddr *DHCPD_FindLeaseByChaddr(uint8_t *chaddr)
{
    unsigned int i;

    for (i = 0; i < server_config.max_leases; i++)
    {
        if (memcmp(leases[i].chaddr, chaddr, 6) == 0)
        return &(leases[i]);
    }

    return NULL;
}

uint32_t softap_find_ipaddr_by_macaddr(uint8_t *mac_addr)
{
   struct dhcpOfferedAddr *dhcp_offered_addr = NULL;
   dhcp_offered_addr = DHCPD_FindLeaseByChaddr(mac_addr);
   if (dhcp_offered_addr) {
       return dhcp_offered_addr->yiaddr.s_addr;
   } else {
       return 0;
   }
}

void make_dhcpd_packet(struct dhcpd *packet, struct dhcpd *oldpacket, char type)
{
    uint32_t lease_time = server_config.lease;
    //uint32_t dns_server=0;
    unsigned char *option = packet->options;
    char domain_name[255] = {0};
    lease_time = htonl(lease_time);
    memset(packet, 0, sizeof(struct dhcpd));
    packet->op = BOOTREPLY;
    packet->htype = ETH_10MB;
    packet->hlen = ETH_10MB_LEN;
    packet->xid = oldpacket->xid;
    sys_memcpy(packet->chaddr, oldpacket->chaddr, 16);
    packet->flags = 0x0; // force to static
    packet->ciaddr = oldpacket->ciaddr;
    packet->siaddr = server_config.siaddr.s_addr;
    packet->giaddr = oldpacket->giaddr;
    packet->cookie = htonl(DHCP_MAGIC);
    packet->options[0] = DHCP_END;
    sys_memcpy(packet->sname,server_config.sname, 6);
    dhcpd_add_option(option, DHCP_MESSAGE_TYPE, sizeof(type), &type);
    dhcpd_add_option(option, DHCP_SERVER_ID, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
    dhcpd_add_option(option,DHCP_LEASE_TIME,sizeof(lease_time),&lease_time);
    dhcpd_add_option(option, DHCP_SUBNET, sizeof(server_config.mask.s_addr), &server_config.mask.s_addr);
    dhcpd_add_option(option, DHCP_ROUTER, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
    dhcpd_add_option(option, DHCP_DNS_SERVER, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
    sys_memcpy(domain_name,DEFAULT_DOMAIN,sizeof(DEFAULT_DOMAIN));
    dhcpd_add_option(option, DHCP_DOMAIN_NAME, strlen(domain_name), domain_name);
}

static int discover(struct dhcpd *packetinfo)
{
    struct in_addr addr;
    //struct in_addr req_ip;
    uint8_t i=0;
    struct dhcpOfferedAddr *lease;
    //struct dhcpOfferedAddr *lease_new;
    //struct dhcpOfferedAddr *lease_change=NULL;
    if (memcmp(packetinfo->chaddr, "\x00\x00\x00\x00\x00\x00", 6) == 0 ||
    memcmp(packetinfo->chaddr, "\xff\xff\xff\xff\xff\xff", 6) == 0)
    {
        return -1;
    }
    if((lease = DHCPD_FindLeaseByChaddr(packetinfo->chaddr))==NULL)
    {
        addr=DHCPD_FindAddress();

        while(*(leases[i].chaddr) != 0)
            i++;
        MEMCPY(leases[i].chaddr,packetinfo->chaddr,6);
        leases[i].yiaddr=addr;
        lease=&(leases[i]);
    }

    memset(&payload_out,0,sizeof(struct dhcpd));
    make_dhcpd_packet(&payload_out, packetinfo, DHCPOFFER);
    payload_out.yiaddr = lease->yiaddr.s_addr;

    return 0;
}


static int request(struct dhcpd *packetinfo)
{
    uint32_t lease_time = server_config.lease;
    //uint32_t dns_server=0;
    char type;
    unsigned char *option = payload_out.options;
    struct dhcpOfferedAddr *lease;
    char domain_name[255] = {0};
    lease_time = htonl(lease_time);

    memset(&payload_out,0,sizeof(struct dhcpd));

    if((lease = DHCPD_FindLeaseByChaddr(packetinfo->chaddr))!=NULL)
    {
        type=DHCPACK;
        payload_out.op = BOOTREPLY;
        payload_out.htype = ETH_10MB;
        payload_out.hlen = ETH_10MB_LEN;
        payload_out.xid = packetinfo->xid;
        sys_memcpy(payload_out.chaddr, packetinfo->chaddr, 16);
        payload_out.flags = 0x0; // force to static
        payload_out.ciaddr = packetinfo->ciaddr;
        payload_out.siaddr = server_config.siaddr.s_addr;
        payload_out.giaddr = packetinfo->giaddr;
        payload_out.cookie = htonl(DHCP_MAGIC);
        payload_out.options[0] = DHCP_END;
        payload_out.yiaddr = lease->yiaddr.s_addr;
        dhcpd_add_option(option, DHCP_MESSAGE_TYPE, sizeof(type), &type);
        dhcpd_add_option(option, DHCP_SERVER_ID, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
        dhcpd_add_option(option,DHCP_LEASE_TIME,sizeof(lease_time),&lease_time);
        dhcpd_add_option(option, DHCP_SUBNET, sizeof(server_config.mask.s_addr), &server_config.mask.s_addr);
        dhcpd_add_option(option, DHCP_ROUTER, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
        dhcpd_add_option(option, DHCP_DNS_SERVER, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
        sys_memcpy(domain_name,DEFAULT_DOMAIN,sizeof(DEFAULT_DOMAIN));
        dhcpd_add_option(option, DHCP_DOMAIN_NAME, strlen(domain_name), domain_name);

        printf("DHCPD: Assign %d.%d.%d.%d for %02x:%02x:%02x:%02x:%02x:%02x.\n\r\n",
            (uint32_t)(payload_out.yiaddr & 0xFF), (uint32_t)((payload_out.yiaddr >> 8) & 0xFF),
            (uint32_t)((payload_out.yiaddr >> 16) & 0xFF), (uint32_t)(payload_out.yiaddr >> 24),
            packetinfo->chaddr[0], packetinfo->chaddr[1], packetinfo->chaddr[2],
            packetinfo->chaddr[3], packetinfo->chaddr[4], packetinfo->chaddr[5]);
        wifi_netlink_indicate_softap_sta_add(packetinfo->chaddr, payload_out.yiaddr);
    }
    else
    {
        type=DHCPNAK;
        payload_out.op = BOOTREPLY;
        payload_out.htype = ETH_10MB;
        payload_out.hlen = ETH_10MB_LEN;
        payload_out.xid = packetinfo->xid;
        sys_memcpy(payload_out.chaddr, packetinfo->chaddr, 16);
        payload_out.flags = 0x0; // force to static
        payload_out.ciaddr = packetinfo->ciaddr;
        payload_out.giaddr = packetinfo->giaddr;
        payload_out.cookie = htonl(DHCP_MAGIC);
        payload_out.options[0] = DHCP_END;
        dhcpd_add_option(option, DHCP_MESSAGE_TYPE, sizeof(type), &type);
        dhcpd_add_option(option, DHCP_SERVER_ID, sizeof(server_config.server.s_addr), &server_config.server.s_addr);
    }

    return 0;
}

#define START_IP     0
#define END_IP        1
#define BOOT_IP        2
void ip_create_by_config(int flag, char * ipaddr)
{
    int i,j;
    i=j=0;
    while(ipaddr[i] != '\0')
    {
        if(ipaddr[i] == '.')
            j++;
        i++;
        if(j >= 3)
            break;
    }
    ipaddr[i] = '\0';
    switch(flag)
    {
        case START_IP:
            strcat(ipaddr,"150");
            break;
        case END_IP:
            strcat(ipaddr,"200");
            break;
        case BOOT_IP:
            strcat(ipaddr,"5");
            break;
        default:
            break;
    }
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("DHCP IP: [ %s ]\n\r",ipaddr));
}
static int init_config(void)
{
    uint32_t start;
    uint32_t end;
    uint32_t sa=0;
    //uint_32 lan_ip;
    //uint_32 lan_mask;
    //uint_32 lan_net;

    int val;
    //char valstr[255] = {0};
    //extern int config_dhcps_wan_domain;
    char dhcp_q_lann_ipaddr[16] = {"192.168.237.1"}; //config later
    char dhcp_q_lann_mask[16] = {"255.255.255.0"}; //config later
    char ip_addr[16] = {0};


    memset(&server_config, 0, sizeof(struct server_config_t));

    /* we use first listen interface as server IP */
    //server_config.interface = LAN_NAME;
    //if (read_interface(server_config.interface, &server_config.server.s_addr, &server_config.mask.s_addr, server_config.arp) < 0)
    //    return -1;

    strcpy(ip_addr, dhcp_q_lann_ipaddr);
    inet_aton(ip_addr/*DEFAULT_SERVER_IP*/, &sa);
    server_config.server.s_addr=sa;

    strcpy(ip_addr, dhcp_q_lann_mask);
        sa=0;
    inet_aton(ip_addr/*DEFAULT_MASK*/, &sa);
    server_config.mask.s_addr=sa;

    //lan_ip = A_BE2CPU32(server_config.server.s_addr);
    //lan_mask = A_BE2CPU32(server_config.mask.s_addr);
    //lan_net = (lan_ip & lan_mask);

    //start address
    //if (CFG_get(CFG_LAN_DHCPD_START, &start) == -1)
    strcpy(ip_addr, dhcp_q_lann_ipaddr);
    ip_create_by_config(START_IP, ip_addr);
    inet_aton(ip_addr/*DEFAULT_START*/, &start);
       server_config.start.s_addr=start;



    // end address
    //if (CFG_get( CFG_LAN_DHCPD_END, &end) == -1)
    strcpy(ip_addr, dhcp_q_lann_ipaddr);
    ip_create_by_config(END_IP, ip_addr);
    inet_aton(ip_addr/*DEFAULT_END*/, &end);
    server_config.end.s_addr=end;



    server_config.max_leases = DHCPD_MAX_LEASES;

    // lease time
    //if (CFG_get( CFG_LAN_DHCPD_LEASET, &val) == -1)
        val = DEFAULT_LEASE_TIME;//3600

    server_config.lease = val; //3600

    server_config.conflict_time = DEFAULT_CONFLICT_TIME; //3600
    server_config.decline_time = DEFAULT_DECLINE_TIME;   //3600
    server_config.min_lease = DEFAULT_MIN_LEASE_TIME;//60
    server_config.offer_time = DEFAULT_MIN_LEASE_TIME; //60
    server_config.auto_time = DEFAULT_AUTO_TIME; //3

    server_config.sname = DEFAULT_SNAME;
    server_config.boot_file = DEFAULT_BOOT_FILE;
    strcpy(ip_addr, dhcp_q_lann_ipaddr);
    ip_create_by_config(BOOT_IP, ip_addr);
    sa=0;
    inet_aton(ip_addr/*DEFAULT_BOOT_IP*/, &sa);
    server_config.siaddr.s_addr=sa;


    //
    // Attach embedded options
    //
    //dhcpd_flush_embed_options();

    //
    // add netmask option
    //
    //memcpy(&sa, server_config.mask.s_addr, sizeof(struct in_addr));
    //dhcpd_attach_embed_option(DHCP_SUBNET, &server_config.mask.s_addr);

    // add gateway option
    //sa = DHCPD_GetGwConf(server_config.server);
    //dhcpd_attach_embed_option(DHCP_ROUTER, &server_config.server.s_addr);

    // add dns options
   //    for (i=0; (sa.s_addr = DNS_dhcpd_dns_get(i)) != 0; i++)
    //{
    //    sa=0;
    //    inet_aton(DEFAULT_DNS,&sa);
    //    dhcpd_attach_embed_option(DHCP_DNS_SERVER, &sa);
    //}

    // add domain name
    //pass_wan_domain = config_dhcps_wan_domain;                // Menuconfig set the default.
    //CFG_get(CFG_LAN_DHCPD_PASS_WDN, &pass_wan_domain);        // Can be overridden by profile
    //if (pass_wan_domain == 0)
    //{
    //    if (CFG_get_str( CFG_SYS_DOMAIN, valstr) != -1)
    //   memcpy(valstr,DEFAULT_DOMAIN,sizeof(DEFAULT_DOMAIN));
    //        dhcpd_attach_embed_option(DHCP_DOMAIN_NAME, valstr);
    //}

    /* flush static leases, then reload from CFG */
    //DHCPD_LoadDynLeases();

    //DHCPD_FlushStaticLeases();
    //DHCPD_LoadStaticLeases();

    /* fix lease IP or expired time */
    //if (fix_lease_error() != 0)
    //    DHCPD_WriteDynLeases();

    return 0;
}


uint8_t dhcp_process(void *packet_addr)
     {
        char state;

        if (dhcpd_pickup_opt((struct dhcpd *)packet_addr, DHCP_MESSAGE_TYPE, sizeof(state), &state) == NULL)
        {
            LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: couldn't get option from packet, ignoring"));
            return 0;
        }

        switch (state)
        {
        case DHCPDISCOVER:
            LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: discover packet....\r\n"));
            discover(packet_addr);
            break;

        case DHCPREQUEST:
            LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: request packet...\n\r"));
            request(packet_addr);
            break;

        default:
            LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: unknown message\n\r"));
            return 0;
        }
        return 1;
     }

void UDP_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    ip_addr_t destAddr;
    struct pbuf *q;

#if LWIP_IPV6
    if (addr->type == IPADDR_TYPE_V6)
        destAddr.u_addr.ip6.addr[0] = 0xff;
    else
        destAddr.u_addr.ip4.addr = htonl(IPADDR_BROADCAST);
#else
    destAddr.addr = htonl(IPADDR_BROADCAST);
#endif
    if (p!=NULL) {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: UDP_Receive ....\r\n"));
        if (dhcp_process(p->payload) != 0) {
            LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[DHCPD]: dhcp packet send....\r\n"));
            q = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcpd), PBUF_REF);
            if (q) {
                q->payload = &payload_out;
                udp_sendto(upcb, q, &destAddr, port);
                pbuf_free(q);
            }
        }
        pbuf_free(p);
    }
}

void dhcpd_daemon( void )
{
    if (UdpPcb == NULL) {
    memset(leases,0,sizeof(struct dhcpOfferedAddr)*5);
    init_config();
    UdpPcb=udp_new();
    udp_bind(UdpPcb,IP_ADDR_ANY,67);
    udp_recv(UdpPcb,UDP_Receive,NULL);
}
}
void stop_dhcpd_daemon(void)
{
    if (UdpPcb) {
        udp_remove(UdpPcb);
        UdpPcb = NULL;
    }
}

#endif
