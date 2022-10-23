/**
 * @file wifi_netif.c
 * @brief wifi netif interfaces
 * @copyright Freethink
 * @author Freethink
 * @date 2018-11-05
 * @version v001
 */

#include "app_cfg.h"
#include "lwip/opt.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "netif/etharp.h"
#include "wlan_intf.h"
#include "wifi_netif.h"
#include "wrapper_os.h"
#include "ethernetif.h"
#include "debug_print.h"
#include "wifi_netlink.h"
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif

static struct netif wifi_netif;
static uint8_t ip_static_mode = 0;

void wifi_netif_open(void)
{
    uint8_t *mac;
#if LWIP_IPV6
    netif_add(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), NULL, &ethernetif_init, &tcpip_input);
    netif_create_ip6_linklocal_address(&wifi_netif, 1);
#if LWIP_IPV6_DHCP6
    dhcp6_enable_stateless(&wifi_netif);
#endif
#else
    netif_add(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY, NULL, &ethernetif_init, &tcpip_input);
#endif
    mac = wifi_netif_get_hwaddr();
    DEBUGPRINT("WiFi MAC address: "MAC_FMT"\r\n",MAC_ARG(mac));
    netif_set_default(&wifi_netif);
}

void wifi_netif_close(void)
{
    struct ethernetif *ethernet_if = (struct ethernetif *)(wifi_netif.state);

    wifi_netif.state = NULL;
    ethernet_if->adapter = NULL;

    wifi_ops_entry.wifi_close_func();

    ethernetif_deinit(ethernet_if);
    netifapi_dhcp_stop(&wifi_netif);
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY));
#else
    netifapi_netif_set_addr(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY);
#endif
    netifapi_netif_remove(&wifi_netif);
    dhcp_cleanup(&wifi_netif);
}

uint8_t wifi_netif_set_hwaddr(uint8_t *mac_addr)
{
    struct ethernetif *ethernet_if = (struct ethernetif *)(wifi_netif.state);

    if (wifi_ops_entry.wifi_set_mac_addr(mac_addr)) {
        sys_memcpy(wifi_netif.hwaddr, mac_addr, NETIF_MAX_HWADDR_LEN);
        sys_memcpy(ethernet_if->mac_addr, mac_addr, NETIF_MAX_HWADDR_LEN);
        return TRUE;
    } else {
        return FALSE;
    }

}

uint8_t *wifi_netif_get_hwaddr(void)
{
    return (uint8_t *)wifi_netif.hwaddr;
}

ip_addr_t *wifi_netif_get_ip(void)
{
    return &wifi_netif.ip_addr;
}

#if LWIP_IPV6
ip_addr_t *wifi_netif_get_ip6(uint8_t index)
{
    return &wifi_netif.ip6_addr[index];
}

void wifi_netif_set_ip6addr_invalid(void)
{
    wifi_netif.ip6_addr_state[1] = IP6_ADDR_INVALID;
    ip6_addr_set_zero(&wifi_netif.ip6_addr[1].u_addr.ip6);
}
#endif

void wifi_netif_set_ip(ip4_addr_t *ip, ip4_addr_t *netmask, ip4_addr_t *gw)
{
    netifapi_netif_set_addr(&wifi_netif, ip, netmask, gw);
    wifi_ops_entry.wifi_set_ipaddr_func((uint8_t *)ip);
}

ip_addr_t *wifi_netif_get_gw(void)
{
    return &wifi_netif.gw;
}

ip_addr_t *wifi_netif_get_netmask(void)
{
    return &wifi_netif.netmask;
}

void wifi_netif_set_addr(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(ip), ip_2_ip4(mask), ip_2_ip4(gw));
#else
    netifapi_netif_set_addr(&wifi_netif, ip, mask, gw);
#endif
}

void wifi_netif_set_up(void)
{
    netifapi_netif_set_up(&wifi_netif);
}

void wifi_netif_set_down(void)
{
    netifapi_netif_set_down(&wifi_netif);
}

int32_t wifi_netif_is_ip_got(void)
{
    if (!ip_addr_isany(&wifi_netif.ip_addr) && ((netif_dhcp_data(&wifi_netif)->state == DHCP_STATE_BOUND) || wifi_netif_is_static_ip_mode())) {
#if LWIP_IPV6
        if (ip6_addr_isany(&wifi_netif.ip6_addr[1].u_addr.ip6))
            return FALSE;
        else
#endif
            return TRUE;
    } else
        return FALSE;
}

void wifi_netif_start_dhcp(void)
{
    //DEBUGPRINT("WiFi Netif: Start DHCP\n");

    netifapi_netif_set_up(&wifi_netif);
    if (wifi_netif_is_static_ip_mode())
        return;
    netifapi_dhcp_start(&wifi_netif);
}

int32_t wifi_netif_polling_dhcp(void)
{
    uint32_t i;
    if (wifi_netif_is_static_ip_mode())
        return TRUE;
    for (i=0; i < 200; i++) {
        if ((i % 40) == 0) {
            //DEBUGPRINT("WiFi Netif: DHCP doing (state = %d)...\n", netif_dhcp_data(&wifi_netif)->state);
        }
        sys_ms_sleep(100);
        if (netif_dhcp_data(&wifi_netif)->state == DHCP_STATE_BOUND) {
            return TRUE;
        }
    }
    return FALSE;
}

void wifi_netif_stop_dhcp(void)
{
    //DEBUGPRINT("WiFi Netif: Stop DHCP\n");
     if(wifi_netif_is_static_ip_mode()) {
         return;
     }
    netifapi_dhcp_stop(&wifi_netif);
    netifapi_netif_set_down(&wifi_netif);
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY));
#else
    netifapi_netif_set_addr(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY);
#endif
}

void wifi_netif_set_ip_mode(uint8_t ip_mode)
{
    ip_static_mode = ip_mode;
}

int32_t wifi_netif_is_static_ip_mode(void)
{
    return ((ip_static_mode == IP_MODE_STATIC) ? 1 : 0);
}