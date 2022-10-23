#ifndef __WIFI_NETIF_H__
#define __WIFI_NETIF_H__

#include "app_type.h"
#include "lwip/netif.h"

void wifi_netif_open(void);
void wifi_netif_close(void);
uint8_t wifi_netif_set_hwaddr(uint8_t *mac_addr);
uint8_t *wifi_netif_get_hwaddr(void);
ip_addr_t *wifi_netif_get_ip(void);
#if LWIP_IPV6
ip_addr_t *wifi_netif_get_ip6(uint8_t index);
void wifi_netif_set_ip6addr_invalid(void);
#endif
void wifi_netif_set_ip(ip4_addr_t *ip, ip4_addr_t *netmask, ip4_addr_t *gw);
ip_addr_t *wifi_netif_get_gw(void);
ip_addr_t *wifi_netif_get_netmask(void);
void wifi_netif_set_addr(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
void wifi_netif_set_up(void);
void wifi_netif_set_down(void);
int32_t wifi_netif_is_ip_got(void);
void wifi_netif_start_dhcp(void);
int32_t wifi_netif_polling_dhcp(void);
void wifi_netif_stop_dhcp(void);
void wifi_netif_set_ip_mode(uint8_t ip_mode);
int32_t wifi_netif_is_static_ip_mode(void);

#endif  // __WIFI_NETIF_H__
