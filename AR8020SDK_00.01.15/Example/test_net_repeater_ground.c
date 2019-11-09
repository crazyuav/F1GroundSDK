///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// SDK Included Files
#include "debuglog.h"
#include "hal_bb.h"
#include "hal_gpio.h"
#include "memory_config.h"
#include "debuglog.h"
#include "cmsis_os.h"
#include "hal_usb_host.h"
#include "hal.h"
#include "test_net_repeater_ground.h"
#include "hal_sram_ground.h"
#include "memory.h"

void repeater_ground_input(void *data, uint32_t size);

static void rcvDataHandler(void *p)
{
    // do nothing
}

/* Call by USB NET Recv */

#define MAX_BUFFER (50)

typedef struct _ring_buf
{
    void  *buffer[MAX_BUFFER];
    uint16_t  buffer_size[MAX_BUFFER];    
    uint16_t  rd_ptr;
    uint16_t  wr_ptr;
}ring_buf_t;


volatile ring_buf_t session_ring;

uint32_t start_send_tick = 0;
uint32_t pre_log_tick = 0;
uint32_t total_size = 0;
uint8_t  ipcamera_mac_address[6];
uint8_t  ipcamera_mac_address_valid = 0;

void sessionSend_task(void const *argument)
{
    while(1)
    {
        if (ipcamera_mac_address_valid == 0)
        {
            while (HAL_USB_GetIPCameraMacAddress(ipcamera_mac_address) != HAL_OK)
            {
                HAL_Delay(2);
            }

            DLOG_Critical("get ipaddress: %02x, %02x, %02x, %02x, %02x, %02x",
                                          ipcamera_mac_address[0],
                                          ipcamera_mac_address[1],
                                          ipcamera_mac_address[2],
                                          ipcamera_mac_address[3],
                                          ipcamera_mac_address[4],
                                          ipcamera_mac_address[5]);

            HAL_USB_NetDeviceUp(ENUM_USB_NETCARD_NORNAL_MODE);
            HAL_USB_NetDeviceRecv(repeater_ground_input);
            HAL_USB_NetDeviceSetMacAddr(ipcamera_mac_address);

            ipcamera_mac_address_valid = 1;
        }

        int rd_idx = session_ring.rd_ptr;
        int wr_idx = session_ring.wr_ptr;

        if (rd_idx >= MAX_BUFFER || wr_idx >=MAX_BUFFER)
        {
            DLOG_Error("race condition");
            HAL_Delay(2);
            continue;
        }

        if (rd_idx != wr_idx) //not empty
        {
            HAL_RET_T ret = HAL_BB_ComSendMsg(BB_COM_SESSION_3, session_ring.buffer[rd_idx], session_ring.buffer_size[rd_idx]);
            if (HAL_OK == ret)
            {
                //free(session_ring.buffer[rd_idx]);
                rd_idx = (rd_idx + 1) % (MAX_BUFFER);

                session_ring.rd_ptr = rd_idx;
            }
            else
            {
                HAL_Delay(session_ring.buffer_size[rd_idx] * 14 / 50);
            }
        }
        else
        {
            HAL_Delay(5);
        }
    }
}


void create_send_thread(void)
{
    uint8_t i;
    session_ring.rd_ptr = 0;
    session_ring.wr_ptr = 0;

    for(i = 0; i < MAX_BUFFER; i++)
    {
        session_ring.buffer_size[i] = 0;
    }

    osThreadDef(sessionSend_Task, sessionSend_task, osPriorityHigh, 0, 16 * 128);
    osThreadCreate(osThread(sessionSend_Task), NULL);
}


void repeater_ground_input(void *data, uint32_t size)
{
    uint8_t rd_idx = session_ring.rd_ptr;
    uint8_t wr_idx = session_ring.wr_ptr;
    uint8_t cnt;

    total_size += size;
    if (start_send_tick == 0)
    {
        start_send_tick = HAL_GetSysMsTick();
    }

    if (wr_idx >= rd_idx)
    {
        cnt = wr_idx - rd_idx;
    }
    else
    {
        cnt = MAX_BUFFER - (rd_idx - wr_idx);
    }

    if (memcmp(ipcamera_mac_address, data, 6) != 0)
    {
        return;
    }

    uint32_t cur_tick = HAL_GetSysMsTick();
    if (cur_tick - pre_log_tick > 5000 || cnt > 5)
    {
        pre_log_tick = cur_tick;
        DLOG_Error("send=%d tick=%d cnt=%d %d %d", total_size, (cur_tick-start_send_tick), cnt, rd_idx, wr_idx);
    }

    if (rd_idx >= MAX_BUFFER || wr_idx >=MAX_BUFFER )
    {
        DLOG_Error("race condition");
        return;
    }

    if ( (wr_idx + 1) % MAX_BUFFER == rd_idx)
    {
        DLOG_Error("No ring buffer !!!");
        return;
    }

    if (size > 2000)
    {
        DLOG_Error("size =%d overflow !!!", size);
        return;
    }

    if (size > session_ring.buffer_size[wr_idx])
    {
        if (session_ring.buffer[wr_idx] == NULL)
        {
            session_ring.buffer[wr_idx] = malloc_safe(size);
        }
        else
        {
            session_ring.buffer[wr_idx] = realloc_safe(session_ring.buffer[wr_idx], size);
        }
    }

    if (session_ring.buffer[wr_idx] != NULL)
    {
        memcpy(session_ring.buffer[wr_idx], data, size);
        session_ring.buffer_size[wr_idx] = size;
        wr_idx = (wr_idx + 1) % MAX_BUFFER;
        session_ring.wr_ptr = wr_idx;
    }

    return;
}

void command_TestNetRepeaterGnd( void )
{
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_3, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_3);
    }

    ipcamera_mac_address_valid = 0;

    create_send_thread();

    HAL_BB_UpgradeMode(1);

    HAL_SRAM_ChannelConfig(ENUM_HAL_SRAM_CHANNEL_TYPE_RTSP_BYPASS,
                           1);
}

