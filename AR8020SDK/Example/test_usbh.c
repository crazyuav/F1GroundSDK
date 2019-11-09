#include <string.h>
#include <stdlib.h>
#include "test_usbh.h"
#include "debuglog.h"
#include "hal_sram_sky.h"
#include "hal.h"
#include "systicks.h"
#include "auto_test.h"



/* USB Host Global Variables */
USBH_BypassVideoCtrl            g_usbhBypassVideoCtrl;
STRT_USBH_EVENT_MSG             g_usbhAppEventMsg;
FATFS                           g_usbhAppFatFs;


void USBH_USBHostStatus(void const *argument)
{
    STRT_USBH_EVENT_MSG  st_usbhEventMsg;

    HAL_USB_HostProcess(HAL_USB_PORT_0);
    HAL_USB_HostProcess(HAL_USB_PORT_1);

    /* mount udisk */
    USBH_MountUSBDisk();

    while (1)
    {
        st_usbhEventMsg.u8_portId = 0xff;
        st_usbhEventMsg.u8_msgType = 0xff;
        st_usbhEventMsg.u16_reserved = 0xffff;
        
        ar_osUSBHEventMsgQGet((void *)&st_usbhEventMsg);
        
        if(st_usbhEventMsg.u8_msgType == 0)
        {
            HAL_USB_HostProcess(st_usbhEventMsg.u8_portId);
        }
        else
        {
            HAL_Delay(1);
        }
    }
}


void USBH_BypassVideo(uint8_t u8_portId)
{
    FRESULT             fileResult;
    uint32_t            bytesread;
    static uint8_t     *videoBuff;
    static FIL          usbhAppFile;
    uint8_t             i;
    static uint8_t      u8_usbPortId;

    fileResult          = FR_OK;
    bytesread           = 0;

    switch (g_usbhBypassVideoCtrl.taskState)
    {
    case USBH_VIDEO_BYPASS_TASK_IDLE:

        if (g_usbhBypassVideoCtrl.bypassChannel == 0)
        {
            videoBuff           = (uint8_t *)HAL_SRAM_GetVideoBypassChannelBuff(HAL_SRAM_VIDEO_CHANNEL_0);
        }
        else
        {
            videoBuff           = (uint8_t *)HAL_SRAM_GetVideoBypassChannelBuff(HAL_SRAM_VIDEO_CHANNEL_1);
        }

        USBH_MountUSBDisk();

        if (g_usbhBypassVideoCtrl.fileOpened == 0)
        {
            fileResult = f_open(&usbhAppFile, "0:usbtest.264", FA_READ);
        
            if(fileResult == FR_OK)
            {
                g_usbhBypassVideoCtrl.fileOpened = 1;
        
                g_usbhBypassVideoCtrl.taskState = USBH_VIDEO_BYPASS_TASK_TRANS;
            }
            else
            {
                g_usbhBypassVideoCtrl.taskState = USBH_VIDEO_BYPASS_TASK_STOP;
        
                DLOG_Error("open file error: %d\n", (uint32_t)fileResult);
            }
        }
        else
        {
            g_usbhBypassVideoCtrl.taskState = USBH_VIDEO_BYPASS_TASK_TRANS;
        }

        break;

    case USBH_VIDEO_BYPASS_TASK_TRANS:

        fileResult = f_read((&usbhAppFile), videoBuff, USB_VIDEO_BYPASS_SIZE_ONCE, (void *)&bytesread);

        if(fileResult != FR_OK)
        {
            g_usbhBypassVideoCtrl.fileOpened    = 0;

            f_close(&usbhAppFile);

            DLOG_Error("Cannot Read from the file \n");
        }

        if (bytesread < USB_VIDEO_BYPASS_SIZE_ONCE)
        {
            DLOG_Info("a new round!\n");
            g_usbhBypassVideoCtrl.fileOpened    = 0;
            f_close(&usbhAppFile);

            g_usbhBypassVideoCtrl.taskState = USBH_VIDEO_BYPASS_TASK_IDLE;
        }

        break;

    case USBH_VIDEO_BYPASS_TASK_STOP:
        break;

    default:
        break;
    }
}


void USBH_MountUSBDisk(void)
{
    FRESULT             fileResult;
    static uint8_t      s_u8MountFlag = 0;

    if (s_u8MountFlag == 0)
    {
        FATFS_LinkDriver(&USBH_Driver, "0:/");

        fileResult = f_mount(&g_usbhAppFatFs, "0:/", 0);

        if (fileResult != FR_OK)
        {
            DLOG_Error("mount fatfs error: %d\n", fileResult);

            return;
        }

        s_u8MountFlag = 1;
    }
    else
    {
        return;
    }
}


void command_startBypassVideo(uint8_t *bypassChannel)
{
    uint8_t result = 1;
    uint64_t t_diff[1] = {0};

    DLOG_Critical("auto_test>%s %s", __FUNCTION__, bypassChannel);

    g_usbhBypassVideoCtrl.bypassUSBPortId = HAL_USB_GetMSCPort();
    g_usbhBypassVideoCtrl.bypassChannel = strtoul(bypassChannel, NULL, 0);

    /* activate the task */
    if (g_usbhBypassVideoCtrl.bypassChannel == 0)
    {
        HAL_SRAM_EnableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_0);
    }
    else
    {
        HAL_SRAM_EnableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_1);
    }

    HAL_USB_RegisterHostUserHandler(HAL_USB_HOST_CLASS_MSC, USBH_BypassVideo);

    g_usbhAppEventMsg.u8_msgType = 1;
    g_usbhAppEventMsg.u8_portId  = g_usbhBypassVideoCtrl.bypassUSBPortId;
    ar_osUSBHEventMsgQPut((void *)&g_usbhAppEventMsg);

    AUTO_TEST_TimeRecord();
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("osMessagePut", (uint32_t)t_diff[0]);
    DIV_LINE(DIV_LINE_END);
}


void command_stopBypassVideo(void)
{
    if (g_usbhBypassVideoCtrl.bypassChannel == 0)
    {
        HAL_SRAM_DisableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_0);
    }
    else
    {
        HAL_SRAM_DisableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_1);
    }

    HAL_USB_UnRegisterHostUserApp(HAL_USB_HOST_CLASS_MSC, USBH_BypassVideo);
}


