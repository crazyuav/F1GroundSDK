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

#include "test_ping.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// PING_DBG: Enable debugging for PING.
#ifndef PING_DBG
#define PING_DBG                    LWIP_DBG_ON
#endif

#ifndef PING_STACKSIZE
#define PING_STACKSIZE              3000
#endif

#ifndef PING_PRIORITY
#define PING_PRIORITY               osPriorityNormal
#endif

// ping target - should be a "ip_addr_t" 
#ifndef PING_TARGET
#define PING_TARGET                 (netif_default?netif_default->gw:ip_addr_any)
#endif

// ping receive timeout - in milliseconds
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO              1000
#endif

// ping delay - in milliseconds
#ifndef PING_DELAY
#define PING_DELAY                  1000
#endif

// ping identifier - must fit on a u16_t
#ifndef PING_ID
#define PING_ID                     0xAFAF
#endif

// ping additional data size to include in the packet
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE              32
#endif

// ping result action - no default action
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif



///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

static struct netif netif0;

/* ping variables */
static u16_t ping_seq_num;
static u32_t ping_time;
static struct raw_pcb *ping_pcb = NULL;
static ip_addr_t ping_dst;


///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

typedef struct ip_addr ip_addr_t;

#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)
#define inet_addr_from_ipaddr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))

#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
#define inet_addr_to_ipaddr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))


/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) 
    {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the raw ip */
static u8_t
ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);

    //we can also check src ip here, but just egnore it
    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))))
    {
        iecho = (struct icmp_echo_hdr *)((u8_t*)p->payload + PBUF_IP_HLEN);
        if ((iecho->type == ICMP_ER) && (iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
        {
            LWIP_DEBUGF(PING_DBG, ("ping: recv "));
            ip_addr_debug_print(PING_DBG, addr);
            LWIP_DEBUGF(PING_DBG, (" time=%"U32_F" ms\n", (sys_now()-ping_time)));

            /* do some ping result processing */
            PING_RESULT(1);
            pbuf_free(p);
            return 1; /* eat the packet */
        }
    }

    return 0; /* don't eat the packet */
}

static void
ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
    struct pbuf *p;
    struct icmp_echo_hdr *iecho;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

    p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
    if (!p) 
    {
        return;
    }
    if ((p->len == p->tot_len) && (p->next == NULL)) 
    {
        iecho = (struct icmp_echo_hdr *)p->payload;

        ping_prepare_echo(iecho, (u16_t)ping_size);

        raw_sendto(raw, p, addr);
        ping_time = sys_now();

        LWIP_DEBUGF(PING_DBG, ("ping:[%"U32_F"] send ", ping_seq_num));
        ip_addr_debug_print(PING_DBG, addr);
        LWIP_DEBUGF(PING_DBG, ("\n"));
    }
    pbuf_free(p);
}

/*static void
ping_timeout(void *arg)
{
  struct raw_pcb *pcb = (struct raw_pcb*)arg;
  
  LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

  ping_send(pcb, &ping_dst);

  sys_timeout(PING_DELAY, ping_timeout, pcb);
}*/



static void 
ping_thread(void *arg)
{
    int timeout = PING_RCV_TIMEO;
    ip_addr_t ping_target;

    LWIP_UNUSED_ARG(arg);
    IP4_ADDR(&ping_target, 192,168,1,102);

    while (1) 
    {
        ping_send(ping_pcb, &ping_target);
        
        sys_msleep(PING_DELAY);

        etharp_query(&netif0, &ping_target, NULL);
    }
}

static void 
ping_init(void)
{
    ping_pcb = raw_new(IP_PROTO_ICMP);
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

    raw_recv(ping_pcb, ping_recv, NULL);
    raw_bind(ping_pcb, IP_ADDR_ANY);
    //sys_timeout(PING_DELAY, ping_timeout, ping_pcb);

    sys_thread_new("ping_thread", ping_thread, NULL, PING_STACKSIZE, PING_PRIORITY);
}

extern err_t ethernetif_init(struct netif *netif);
extern void HAL_USB_NetDeviceRecv(void (*net_device_recv_handler)(void *, uint32_t));
extern void ethernetif_input(void *data, uint32_t size);

void command_TestPing( void )
{
    ip_addr_t netif0_ipaddr, netif0_netmask, netif0_gw;

    HAL_USB_NetDeviceUp(ENUM_USB_NETCARD_NORNAL_MODE);
    HAL_USB_NetDeviceRecv(ethernetif_input);

    tcpip_init(NULL,NULL);

    IP4_ADDR(&netif0_ipaddr, 192,168,1,110);
    IP4_ADDR(&netif0_netmask, 255,255,255,0);
    IP4_ADDR(&netif0_gw, 192,168,1,1);

    netif_add(&netif0, &netif0_ipaddr, &netif0_netmask, &netif0_gw, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&netif0);
    netif_set_up(&netif0);

    ping_init();
}

#endif // LWIP_SOCKET



