/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 */

/*!
 * @file
 * Ping sender module
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 */

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
#include "hal_usb_host.h"
#include "hal.h"
#include "etharp.h"
#include "ethernetif.h"
#include "hal_usb_host.h"

// SDK Included Files
#include "test_tcpip.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
// PING_DEBUG: Enable debugging for PING.
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

// ping target - should be a "ip_addr_t" 
#ifndef PING_TARGET
#define PING_TARGET   (netif_default?netif_default->gw:ip_addr_any)
#endif

// ping receive timeout - in milliseconds
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

// ping delay - in milliseconds
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

// ping identifier - must fit on a u16_t
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

// ping additional data size to include in the packet
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

// ping result action - no default action
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

#ifndef PING_STACKSIZE
#define PING_STACKSIZE        3000
#endif

#ifndef PING_PRIORITY
#define PING_PRIORITY          osPriorityNormal
#endif


#ifndef UDPECHO_DBG
#define UDPECHO_DBG     LWIP_DBG_ON
#endif


#ifndef UDPECHO_STACKSIZE
#define UDPECHO_STACKSIZE        3000
#endif

#ifndef UDPECHO_PRIORITY
#define UDPECHO_PRIORITY          osPriorityNormal
#endif


#ifndef TCPECHO_STACKSIZE
#define TCPECHO_STACKSIZE        3000
#endif

#ifndef TCPECHO_PRIORITY
#define TCPECHO_PRIORITY          osPriorityNormal
#endif


///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// ping variables
static u16_t ping_seq_num;
static u32_t ping_time;

struct netif netif0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

typedef struct ip_addr ip_addr_t;

#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)
#define inet_addr_from_ipaddr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))

#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
#define inet_addr_to_ipaddr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))

// Prepare a echo ICMP request
static void ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = htons(++ping_seq_num);

    // fill the additional data buffer with some data
    for(i = 0; i < data_len; i++) 
    {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

static err_t ping_send(int s, ip_addr_t *addr)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho) 
    {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t)ping_size);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ipaddr(&to.sin_addr, addr);

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

    mem_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s)
{
    char buf[64];
    int fromlen, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    fromlen = sizeof(from);
    
    while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) 
    {
        if (len >= (int)(sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) 
        {
            iphdr = (struct ip_hdr *)buf;
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) 
            {
                ip_addr_t fromaddr;
                inet_addr_to_ipaddr(&fromaddr, &from.sin_addr);
                LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
                ip_addr_debug_print(PING_DEBUG, &fromaddr);
                LWIP_DEBUGF( PING_DEBUG, (" ping rcv \r\n"));
                // do some ping result processing
                PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
                return;
            } 
        }
    }

    if (len == 0) 
    {
        LWIP_DEBUGF( PING_DEBUG, ("ping: recv timeout\r\n"));
    }

    // do some ping result processing
    PING_RESULT(0);
}


static void ping_thread(void *arg)
{
    int s;
    int timeout = PING_RCV_TIMEO;
    ip_addr_t ping_target;

    LWIP_UNUSED_ARG(arg);

    netif_set_up(&netif0);

    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) 
    {
        return;
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    while (1) 
    {
        IP4_ADDR(&ping_target, 192,168,199,196);
        
        if (ping_send(s, &ping_target) == ERR_OK) 
        {
            DLOG_Error( "%s", "ping: send OK");
            ip_addr_debug_print(PING_DEBUG, &ping_target);

            ping_recv(s);
            LWIP_DEBUGF( PING_DEBUG, ("ping: received"));
        } 
        else 
        {
            DLOG_Error( "%s", ("ping: send "));
            ip_addr_debug_print(PING_DEBUG, &ping_target);
            LWIP_DEBUGF( PING_DEBUG, (" - error\r\n"));
        }
        sys_msleep(PING_DELAY);

        etharp_query(&netif0, &ping_target, NULL);

    }
}

void ping_init(void)
{
    sys_thread_new("ping_thread", ping_thread, NULL, PING_STACKSIZE, PING_PRIORITY);
}

static void udpecho_thread(void *arg)
{
    static struct netconn *conn;
    static struct netbuf *buf;
    char buffer[1280];
    err_t err;
    uint32_t * pint = (uint32_t * )buffer;

    LWIP_UNUSED_ARG(arg);
    netif_set_up(&netif0);
    conn = netconn_new(NETCONN_UDP);
    LWIP_ASSERT("con != NULL", conn != NULL);
    netconn_bind(conn, NULL, 7);

    while (1) 
    {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) 
        {            
            if(netbuf_copy(buf, buffer, buf->p->tot_len) != buf->p->tot_len) 
            {
                LWIP_DEBUGF(UDPECHO_DBG, ("netbuf_copy failed\r\n"));
            } 
            else 
            {
                buffer[buf->p->tot_len] = '\0';
                
                DLOG_Error("len: %u got: %u", buf->p->tot_len, (*pint));
                
                err = netconn_send(conn, buf);
                if(err != ERR_OK) 
                {
                    LWIP_DEBUGF(UDPECHO_DBG, ("netconn_send failed: %d\r\n", (int)err));
                } 
                else 
                {                    
                    //LWIP_DEBUGF(UDPECHO_DBG, ("got %s\r\n", buffer + 4));
                }
            }
            netbuf_delete(buf);
        }
    }
}

void udp_echo_init(void)
{    
    sys_thread_new("udpecho_thread", udpecho_thread, NULL, UDPECHO_STACKSIZE, UDPECHO_PRIORITY);
}


static void tcpecho_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    /////////wkwk/////////
    DLOG_Error("TCP thread start !\n");
    
    LWIP_UNUSED_ARG(arg);
    netif_set_up(&netif0);
    // Create a new connection identifier
    conn = netconn_new(NETCONN_TCP);

    // Bind connection to well known port number 7
    err = netconn_bind(conn, NULL, 7);
    if(err != ERR_OK)
    {
        DLOG_Error("bind failed !\n"); 
    }

    // Tell connection to go into listening mode
    netconn_listen(conn);

    DLOG_Error("TCP listening !\n");

    while (1) 
    {

        /* Grab new connection. */
        err = netconn_accept(conn, &newconn);
        /* Process the new connection. */
        if (err == ERR_OK) 
        {
            struct netbuf *buf;
            void *data;
            u16_t len;

            DLOG_Error("accept new !\n");

            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) 
            {
                do 
                {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_COPY);
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            /* Close connection and discard connection identifier. */
            netconn_close(newconn);
            netconn_delete(newconn);
        }
    }
}

void tcp_echo_init(void)
{
    DLOG_Error("TCP thread building !\n");

    sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, TCPECHO_STACKSIZE, TCPECHO_PRIORITY);
}


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
        volatile int i;
        DLOG_Error("dhcp state %d !\n", netif0.dhcp->state);
        HAL_Delay(100);
    }
    srcaddr = netif0.ip_addr;

    DLOG_Error("get dhcp offered ip !\n");    

    LWIP_UNUSED_ARG(arg);
    
    IP4_ADDR(&dstaddr, 192, 168, 199, 196);
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
        volatile int i;
        
        netbuf_alloc(buf, 256);
        netbuf_take(buf, buffer, 256);
        
        err = netconn_send(conn, buf);
        if(err != ERR_OK) 
        {
            LWIP_DEBUGF(UDPECHO_DBG, ("netconn_send failed: %d\r\n", (int)err));
        } 
        else 
        {                    
            //LWIP_DEBUGF(UDPECHO_DBG, ("got %s\r\n", buffer + 4));
        }
        netbuf_free(buf);
        HAL_Delay(100);
    }
    netbuf_delete(buf);    
}



void dhcp_init(void)
{
    DLOG_Error("dhcp thread building !\n");
    sys_thread_new("udpsend_thread", udpsend_thread, NULL, TCPECHO_STACKSIZE, TCPECHO_PRIORITY);
}


void test_tcpip( void )
{
    ip_addr_t netif0_ipaddr, netif0_netmask, netif0_gw;

    HAL_USB_NetDeviceUp(ENUM_USB_NETCARD_NORNAL_MODE);

    HAL_USB_NetDeviceRecv(ethernetif_input);

    tcpip_init(NULL,NULL);

    IP4_ADDR(&netif0_ipaddr, 0,0,0,0);
    IP4_ADDR(&netif0_netmask, 0,0,0,0);
    IP4_ADDR(&netif0_gw, 0,0,0,0);

    IP4_ADDR(&netif0_ipaddr, 192,168,1,110);
    IP4_ADDR(&netif0_netmask, 255,255,255,0);
    IP4_ADDR(&netif0_gw, 192,168,1,1);

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
        udp_echo_init();
        tcp_echo_init();
    }
    else
    {
        DLOG_Error("dhcp get error !\n");
    }
}

#endif // LWIP_SOCKET
