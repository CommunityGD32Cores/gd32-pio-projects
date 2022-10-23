cmake_minimum_required(VERSION 3.7)

add_library(lwIP_api INTERFACE)
add_library(lwIP)

set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# The minimum set of files needed for lwIP.
set(lwipcore_SRCS
    ${LWIP_DIR}/src/core/init.c
    ${LWIP_DIR}/src/core/def.c
    ${LWIP_DIR}/src/core/dns.c
    ${LWIP_DIR}/src/core/inet_chksum.c
    ${LWIP_DIR}/src/core/ip.c
    ${LWIP_DIR}/src/core/mem.c
    ${LWIP_DIR}/src/core/memp.c
    ${LWIP_DIR}/src/core/netif.c
    ${LWIP_DIR}/src/core/pbuf.c
    ${LWIP_DIR}/src/core/raw.c
    ${LWIP_DIR}/src/core/stats.c
    ${LWIP_DIR}/src/core/sys.c
    ${LWIP_DIR}/src/core/altcp.c
    ${LWIP_DIR}/src/core/altcp_alloc.c
    ${LWIP_DIR}/src/core/altcp_tcp.c
    ${LWIP_DIR}/src/core/tcp.c
    ${LWIP_DIR}/src/core/tcp_in.c
    ${LWIP_DIR}/src/core/tcp_out.c
    ${LWIP_DIR}/src/core/timeouts.c
    ${LWIP_DIR}/src/core/udp.c
)

set(lwipcore4_SRCS
    ${LWIP_DIR}/src/core/ipv4/autoip.c
    ${LWIP_DIR}/src/core/ipv4/dhcp.c
    ${LWIP_DIR}/src/core/ipv4/etharp.c
    ${LWIP_DIR}/src/core/ipv4/icmp.c
    ${LWIP_DIR}/src/core/ipv4/igmp.c
    ${LWIP_DIR}/src/core/ipv4/ip4_frag.c
    ${LWIP_DIR}/src/core/ipv4/ip4.c
    ${LWIP_DIR}/src/core/ipv4/ip4_addr.c
)

# APIFILES: The files which implement the sequential and socket APIs.
set(lwipapi_SRCS
    ${LWIP_DIR}/src/api/api_lib.c
    ${LWIP_DIR}/src/api/api_msg.c
    ${LWIP_DIR}/src/api/err.c
    ${LWIP_DIR}/src/api/if_api.c
    ${LWIP_DIR}/src/api/netbuf.c
    ${LWIP_DIR}/src/api/netdb.c
    ${LWIP_DIR}/src/api/netifapi.c
    ${LWIP_DIR}/src/api/sockets.c
    ${LWIP_DIR}/src/api/tcpip.c
)

# PortFiles

set(lwipport_SRCS
    ${LWIP_DIR}/port/dhcpd.c
    ${LWIP_DIR}/port/ethernetif.c
    ${LWIP_DIR}/port/sys_arch.c
    ${LWIP_DIR}/port/wifi_netif.c
)

# Netif Files

set(lwipnetif_SRCS
    ${LWIP_DIR}/src/netif/ethernet.c
)

# AppFiles

set(lwipapp_SRCS
    ${LWIP_DIR}/apps/ping/ping.c
)

target_sources(lwIP
    PRIVATE
        ${lwipcore_SRCS}
        ${lwipcore4_SRCS}
        ${lwipapi_SRCS}
        ${lwipnetif_SRCS}
        ${lwipport_SRCS}
        ${lwipapp_SRCS}
)

target_include_directories(lwIP_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include/lwip>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/port>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/apps>
)

target_include_directories(lwIP
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/port/arch>
)

target_link_libraries(lwIP
    lwIP_api
    gd32w51x_wifi_api
    os_api
    wifi_api
)