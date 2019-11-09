///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include "lwip/opt.h"

#if LWIP_SOCKET // don't build if not configured for use in lwipopts.h
// Standard C Included Files
#include <stdio.h>
// lwip Included Files
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/pbuf.h"
#include "lwip/netbuf.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
//#include "etharp.h"
// SDK Included Files
#include "hal_usb_host.h"
#include "hal.h"

#include "test_dhcp.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// DHCP_DEBUG: Enable debugging for DHCP.
#ifndef DHCP_DBG
#define DHCP_DBG                    LWIP_DBG_ON
#endif

#ifndef DHCP_STACKSIZE
#define DHCP_STACKSIZE              3000
#endif

#ifndef DHCP_PRIORITY
#define DHCP_PRIORITY               osPriorityNormal
#endif



///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

static struct netif netif0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

typedef struct ip_addr ip_addr_t;

#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)
#define inet_addr_from_ipaddr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))

#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
#define inet_addr_to_ipaddr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))


static void udpsend_thread(void *arg)
{
    struct ip_addr          dstaddr;
    struct ip_addr          srcaddr;
    struct netconn *        conn;
    struct netbuf *         buf = NULL;
    u8_t                    buffer[256];
    u32_t *                 ipaddr;
    u16_t                   dst_port = 8888;
    int                     i = 0;
    err_t                   err;

    while(netif0.dhcp->state != DHCP_BOUND)
    {
        LWIP_DEBUGF(DHCP_DBG, ("dhcp state %d !\n", netif0.dhcp->state));
        HAL_Delay(100);
    }
    srcaddr = netif0.ip_addr;

    LWIP_DEBUGF(DHCP_DBG, ("get dhcp offered ip !\n"));    

    LWIP_UNUSED_ARG(arg);
    
    IP4_ADDR(&dstaddr, 255, 255, 255, 255);// send source ip address to all
    conn = netconn_new(NETCONN_UDP);
    LWIP_ASSERT("con != NULL", conn != NULL);
    //netconn_bind(conn, NULL, 8888);
    netconn_connect(conn, &dstaddr, dst_port);

    ipaddr = (u32_t *)buffer;

    *ipaddr = srcaddr.addr;
    
    for(i = 4; i < sizeof(buffer) - 1; i++)
    {
        buffer[i] = i;
    }
    buffer[sizeof(buffer) - 1] = '\0';
    
    buf = netbuf_new();    

    while (1) 
    {        
        netbuf_alloc(buf, 256);
        netbuf_take(buf, buffer, 256);
        
        err = netconn_send(conn, buf);
        if(err != ERR_OK) 
        {
            DLOG_Error("netconn_send failed: %d\r\n", (int)err);
        } 
        else 
        {                    
            DLOG_Info("netconn_send success\r\n");
        }
        netbuf_free(buf);
        HAL_Delay(100);
    }
    netbuf_delete(buf);
}


void dhcp_init(void)
{
    LWIP_DEBUGF(DHCP_DBG, ("dhcp thread building !\n"));
    sys_thread_new("udpsend_thread", udpsend_thread, NULL, DHCP_STACKSIZE, DHCP_PRIORITY);
}


extern err_t ethernetif_init(struct netif *netif);
extern void HAL_USB_NetDeviceRecv(void (*net_device_recv_handler)(void *, uint32_t));
extern void ethernetif_input(void *data, uint32_t size);


void command_TestDhcp( void )
{
    ip_addr_t netif0_ipaddr, netif0_netmask, netif0_gw;

    HAL_USB_NetDeviceUp(ENUM_USB_NETCARD_NORNAL_MODE);
    HAL_USB_NetDeviceRecv(ethernetif_input);

    tcpip_init(NULL,NULL);

    LWIP_DEBUGF(DHCP_DBG, ("dhcp client start!\n"));

    IP4_ADDR(&netif0_ipaddr, 0,0,0,0);
    IP4_ADDR(&netif0_netmask, 0,0,0,0);
    IP4_ADDR(&netif0_gw, 0,0,0,0);

    netif_add(&netif0, &netif0_ipaddr, &netif0_netmask, &netif0_gw, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&netif0);
    netif_set_up(&netif0);

    if(ERR_OK == dhcp_start(&netif0))
    {
        dhcp_init();
        while(netif0.dhcp->state != DHCP_BOUND)
        {
            HAL_Delay(100);
        }
        LWIP_DEBUGF(DHCP_DBG, ("dhcp get success !\n"));
        LWIP_DEBUGF(DHCP_DBG, ("IP address: %u.%u.%u.%u\n", ip4_addr1(&(netif0.ip_addr)), ip4_addr2(&(netif0.ip_addr)), \
                                                ip4_addr3(&(netif0.ip_addr)), ip4_addr4(&(netif0.ip_addr))));
    }
    else
    {
        DLOG_Error("dhcp get error !\n");
    }
}


#endif // LWIP_SOCKET


