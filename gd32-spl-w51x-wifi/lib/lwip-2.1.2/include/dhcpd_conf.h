#ifndef DHCPD_CONF_H_
#define DHCPD_CONF_H_


#define DHCPD_MAX_LEASES            5

#define DEFAULT_START           "192.168.0.2"
#define DEFAULT_END             "192.168.0.253"
#define DEFAULT_LEASE_TIME      3600
#define DEFAULT_AUTO_TIME       3
#define DEFAULT_CONFLICT_TIME   3600
#define DEFAULT_DECLINE_TIME    3600
#define DEFAULT_MIN_LEASE_TIME  60
#define DEFAULT_SNAME           "FreeThink"
#define DEFAULT_BOOT_FILE       ""
#define DEFAULT_SERVER_IP       "192.168.0.1"
#define DEFAULT_BOOT_IP         "192.168.0.5"
#define DEFAULT_MASK            "255.255.255.0"
#define DEFAULT_GATEWAY         "192.168.0.1"
#define DEFAULT_DNS             "168.95.1.1"
#define DEFAULT_DOMAIN          "www.freethink.cn"
#define DHCPD_DEBUG
//#undef     DHCPD_DEBUG

//#define DHCPD_DIAG(c...)      SysLog(LOG_INFO|LOGM_DHCPD, ##c)
//#define DHCPD_LOG(c...)       SysLog(LOG_USER|LOG_INFO|LOGM_DHCPD, ##c)

//#define IGNORE_BROADCASTFLAG

#define _string(s)  #s
#define STR(s)      _string(s)

#endif /* DHCPD_CONF_H_ */
