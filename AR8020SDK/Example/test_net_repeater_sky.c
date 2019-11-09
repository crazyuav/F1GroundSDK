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
#include "test_net_repeater_sky.h"
#include "hal_sram_sky.h"


static void rcvDataHandler(void *p)
{
    /* receive from repeater ground SPI send to IP Camera */
    HAL_RET_T ret;
    uint8_t   data_buf_proc[1520];// 1500 + 6 + 6 + 2 + 4 = 1518
    uint32_t  u32_rcvLen = 0;

    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_3, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        HAL_RET_T send_ret;
        uint32_t cnt = 0;

        send_ret = HAL_USB_NetDeviceSend(u32_rcvLen, data_buf_proc);
        if(HAL_OK != send_ret)
        {
            DLOG_Error("NetDeviceSend fail");
            return;
        }
    }
    else
    {
        DLOG_Error("ComReceiveMsg fail");
        return;
    }

    return;
}

void repeater_sky_input(void *data, uint32_t size)
{
    HAL_SRAM_IPCameraPassThrough(data, size, HAL_SRAM_VIDEO_CHANNEL_1);

    return;
}


void command_TestNetRepeaterSky( void )
{
    uint8_t PC_MAC_addr[6] = {0xf4, 0x8e, 0x38, 0x94, 0x76, 0x1f};

    HAL_USB_NetDeviceUp(ENUM_USB_NETCARD_PROMISCUOUS_MODE);
    HAL_USB_NetDeviceSetMacAddr(PC_MAC_addr);
    HAL_USB_NetDeviceRecv(repeater_sky_input);

    HAL_SRAM_EnableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_1);

    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_3, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_3);
    }
}



