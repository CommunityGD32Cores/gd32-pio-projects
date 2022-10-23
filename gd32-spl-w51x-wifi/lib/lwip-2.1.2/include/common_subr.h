/*
 * common_subr.h
 *
 * Created: 2013/1/7 14:13:41
 *  Author: Administrator
 */


#ifndef COMMON_SUBR_H_
#define COMMON_SUBR_H_


#include <dhcpd.h>


/*****************************************************************/
/* Do not modify below here unless you know what you are doing!! */
/*****************************************************************/

/* DHCP protocol -- see RFC 2131 */
#define SERVER_PORT             (67)
#define CLIENT_PORT             (68)

#define DHCP_MAGIC              0x63825363

/* DHCP option codes (partial list) */
#define DHCP_PADDING            0x00
#define DHCP_SUBNET             0x01
#define DHCP_TIME_OFFSET        0x02
#define DHCP_ROUTER             0x03
#define DHCP_TIME_SERVER        0x04
#define DHCP_NAME_SERVER        0x05
#define DHCP_DNS_SERVER         0x06
#define DHCP_LOG_SERVER         0x07
#define DHCP_COOKIE_SERVER      0x08
#define DHCP_LPR_SERVER         0x09
#define DHCP_HOST_NAME          0x0c
#define DHCP_BOOT_SIZE          0x0d
#define DHCP_DOMAIN_NAME        0x0f
#define DHCP_SWAP_SERVER        0x10
#define DHCP_ROOT_PATH          0x11
#define DHCP_IP_TTL             0x17
#define DHCP_MTU                0x1a
#define DHCP_BROADCAST          0x1c
#define DHCP_NTP_SERVER         0x2a
#define DHCP_WINS_SERVER        0x2c
#define DHCP_REQUESTED_IP       0x32
#define DHCP_LEASE_TIME         0x33
#define DHCP_OPTION_OVER        0x34     // 52
#define DHCP_MESSAGE_TYPE       0x35
#define DHCP_SERVER_ID          0x36
#define DHCP_PARAM_REQ          0x37
#define DHCP_MESSAGE            0x38
#define DHCP_MAX_SIZE           0x39
#define DHCP_T1                 0x3a
#define DHCP_T2                 0x3b
#define DHCP_VENDOR             0x3c
#define DHCP_CLIENT_ID          0x3d
#define    DHCP_TFTP            0x42
#define    DHCP_BOOTFILE        0x43
#define DHCP_END                0xFF


#define BOOTREQUEST             1
#define BOOTREPLY               2

#define ETH_10MB                1
#define ETH_10MB_LEN            6

#define DHCPDISCOVER            1
#define DHCPOFFER               2
#define DHCPREQUEST             3
#define DHCPDECLINE             4
#define DHCPACK                 5
#define DHCPNAK                 6
#define DHCPRELEASE             7
#define DHCPINFORM              8

#define BROADCAST_FLAG          0x8000

#define OPTION_FIELD            0
#define FILE_FIELD              1
#define SNAME_FIELD             2

/* miscellaneous defines */
#define MAC_BCAST_ADDR          (unsigned char *) "\xff\xff\xff\xff\xff\xff"

#define OPT_CODE                0
#define OPT_LEN                 1
#define OPT_DATA                2

struct dhcps_option
{
    unsigned char code;
    unsigned char len;
    unsigned char *option;
};

#endif /* COMMON_SUBR_H_ */
