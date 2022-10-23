#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "net_intf.h"

#define ETH_RX_BUF_SIZE     1514

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct netif *netif;
  void *adapter;
  uint8_t mac_addr[NETIF_MAX_HWADDR_LEN];
  wifi_output_fn wifi_output;
};

err_t ethernetif_init(struct netif *netif);
void ethernetif_deinit(struct ethernetif *eth_if);
void ethernetif_input(struct ethernetif *eth_if,  struct tcpip_packet_info *rx_packet);
void ethernetif_register_wifi(void *ethif, void *adapter, uint8_t *mac_addr, wifi_output_fn xmit_func);
#endif
