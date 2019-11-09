#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "uvc_task.h"
#include "test_usbh.h"
#include "hal_usb_host.h"
#include "hal_usb_device.h"
#include "hal_sram_sky.h"
#include "debuglog.h"
#include "hal.h"
#include "memory.h"

volatile uint8_t                g_u8ViewUVC = 0;
volatile uint8_t                g_u8SaveUVC = 0;
volatile uint8_t                g_u8ShowUVC = 0;
STRU_UVC_VIDEO_FRAME_FORMAT     g_stFrameFormat;
static volatile uint8_t         g_u8UVCRunning = 0;


void USBH_UVCInit(uint16_t u16_width, uint16_t u16_height, ENUM_HAL_USB_UVC_DATA_TYPE e_data_type)
{
    g_stFrameFormat.u16_width       = u16_width;
    g_stFrameFormat.u16_height      = u16_height;
    g_stFrameFormat.e_dataType      = e_data_type;

    if (g_stFrameFormat.e_dataType == ENUM_UVC_DATA_YUV)
    {
        HAL_USB_RegisterHostUserHandler(HAL_USB_HOST_CLASS_UVC, USBH_ProcYUV);
    }
    else
    {
        HAL_USB_RegisterHostUserHandler(HAL_USB_HOST_CLASS_UVC, USBH_ProcH264);
    }
}


void USBH_ProcH264(uint8_t u8_portId)
{
    uint32_t                    u32_UVCFrameSize;

    if (HAL_USB_GetHostAppState(u8_portId) != HAL_USB_HOST_STATE_READY)
    {
        g_u8UVCRunning = 0;

        return;
    }

    if (g_u8UVCRunning == 0)
    {
        if (HAL_OK == HAL_USB_StartUVC(&g_stFrameFormat, &u32_UVCFrameSize, u8_portId))
        {
            g_u8UVCRunning = 1;
        }
        else
        {
            DLOG_Error("app start H264 fail");
        }
    }

    return;
}


void USBH_ProcYUV(uint8_t u8_portId)
{
    uint32_t                    u32_uvcFrameNum;
    uint32_t                    u32_UVCFrameSize;
    static uint8_t             *u8_FrameBuff = NULL;

    if (HAL_USB_GetHostAppState(u8_portId) != HAL_USB_HOST_STATE_READY)
    {
        g_u8UVCRunning = 0;

        return;
    }

    if (g_u8UVCRunning)
    {
        if (HAL_OK == HAL_USB_UVCCheckFrameReady(&u32_uvcFrameNum, &u32_UVCFrameSize))
        {
            /* user transfer frame data from SKY to GROUND */
            if (g_u8ViewUVC == 1)
            {
                HAL_USB_TransferUVCToGrd(u8_FrameBuff, u32_UVCFrameSize, &g_stFrameFormat);
            }

            /* user save frame data to udisk in SKY */
            USBH_SaveUVC(u8_FrameBuff, u32_UVCFrameSize);

            /* user send frame data to PAD in SKY */
            if (g_u8ShowUVC == 1)
            {
                USBH_ShowUVC(&g_stFrameFormat,
                             (HAL_USB_PORT_1 - u8_portId),
                             u8_FrameBuff,
                             u32_UVCFrameSize);
            }

            /* prepare to receive next frame */
            HAL_USB_UVCGetVideoFrame(u8_FrameBuff);
        }
    }
    else
    {
        if (HAL_OK == HAL_USB_StartUVC(&g_stFrameFormat, &u32_UVCFrameSize, u8_portId))
        {
            if (NULL != u8_FrameBuff)
            {
                free_safe(u8_FrameBuff);
                u8_FrameBuff = NULL;
            }

            u8_FrameBuff = (uint8_t *)malloc_safe(u32_UVCFrameSize);
            if (NULL == u8_FrameBuff)
            {
                DLOG_Error("malloc user UVC buffer error");

                return;
            }

            /* start UVC Camera OK, prepare to receive frame to process */
            if (HAL_OK == HAL_USB_UVCGetVideoFrame(u8_FrameBuff))
            {
                g_u8UVCRunning = 1;
            }
        }
    }
}


static void USBH_ShowUVC(STRU_UVC_VIDEO_FRAME_FORMAT *stFrameFormat,
                          uint8_t usb_portId,
                          uint8_t *frame_buff,
                          uint32_t frame_size)
{
    uint32_t    u32_packetCount = 0;
    uint32_t    u32_lastPacketLen = 0;
    uint32_t    i;
    uint8_t     *u8_uvcHeader;
    uint32_t    u32_headerSize;

    /* usb transfer 8K Byte */
    u32_packetCount     = (frame_size >> 13);
    u32_lastPacketLen   = (frame_size & (UVC_TRANSFER_SIZE_ONCE - 1));

    u8_uvcHeader        = HAL_USB_GetUVCHeader(stFrameFormat, &u32_headerSize);

    /* send header */
    HAL_USB_SendData(u8_uvcHeader, u32_headerSize, usb_portId, UVC_ENDPOINT_FOR_TRANSFER);

    /* send frame buffer, 8K every time */
    for (i = 0; i < u32_packetCount; i++)
    {
        HAL_USB_SendData(frame_buff, UVC_TRANSFER_SIZE_ONCE, usb_portId, UVC_ENDPOINT_FOR_TRANSFER);

        frame_buff += UVC_TRANSFER_SIZE_ONCE;
    }

    /* the last packet, if remain some data */
    if (u32_lastPacketLen > 0)
    {
        HAL_USB_SendData(frame_buff, u32_lastPacketLen, usb_portId, UVC_ENDPOINT_FOR_TRANSFER);
    }
}


static void USBH_SaveUVC(uint8_t *frame_buff, uint32_t frame_size)
{
    static FIL         *uvcSaveFile = NULL;
    FRESULT             fileResult;
    uint32_t            u32_savedSize;

    if (g_u8SaveUVC)
    {
        if (uvcSaveFile == NULL)
        {
            uvcSaveFile = (FIL *)malloc_safe(sizeof(FIL));

            if (uvcSaveFile != NULL)
            {
                fileResult = f_open(uvcSaveFile, "0:uvcdata.yuv", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

                if (fileResult != FR_OK)
                {
                    free_safe(uvcSaveFile);
                    uvcSaveFile = NULL;

                    DLOG_Error("create uvc file error: %d", fileResult);
                }
                else
                {
                    f_write(uvcSaveFile, frame_buff, frame_size, (void *)&u32_savedSize);
                }
            }
        }
        else
        {
            f_write(uvcSaveFile, frame_buff, frame_size, (void *)&u32_savedSize);
        }
    }
    else
    {
        if (uvcSaveFile != NULL)
        {
            f_close(uvcSaveFile);

            free_safe(uvcSaveFile);
            uvcSaveFile = NULL;
        }
    }
}


void command_ViewUVC(void)
{
    HAL_SRAM_EnableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_1);

    g_u8ViewUVC = 1;
}


void command_startUVC(char *width, char *height)
{
    uint16_t        u16_width   = (uint16_t)strtoul(width, NULL, 0);
    uint16_t        u16_height  = (uint16_t)strtoul(height, NULL, 0);
    uint32_t        u32_UVCFrameSize;
    uint8_t         u8_portId;

    u8_portId       = HAL_USB_GetUVCPortId();

    g_u8UVCRunning  = 0;

    USBH_UVCInit(u16_width, u16_height, ENUM_UVC_DATA_YUV);

    HAL_USB_StartUVC(&g_stFrameFormat, &u32_UVCFrameSize, u8_portId);
}


void command_saveUVC(void)
{
    g_u8SaveUVC = 1;
}


void command_stopSaveUVC(void)
{
    g_u8SaveUVC = 0;
}


void command_showUVC(void)
{
    g_u8ShowUVC++;

    if (g_u8ShowUVC >= 2)
    {
        g_u8ShowUVC = 0;
    }
}


void command_getUVCAttribute(char *index, char *type)
{
    uint8_t     uvc_attr_index = 0;
    uint8_t     uvc_attr_type = 0;
    int32_t     uvc_attr_value = 0;

    uvc_attr_index = (uint8_t)strtoul(index, NULL, 0);
    uvc_attr_type = (uint8_t)strtoul(type, NULL, 16);

    if ((uvc_attr_index >= ENUM_HAL_UVC_MAX_NUM) ||
        (uvc_attr_type < HAL_UVC_GET_CUR)||
        (uvc_attr_type > HAL_UVC_GET_DEF))
    {
        DLOG_Error("invalid attribute: %d");

        command_uvchelp();

        return;
    }

    if (HAL_OK == HAL_USB_HOST_GetUVCAttr(uvc_attr_index, uvc_attr_type, &uvc_attr_value))
    {
        DLOG_Error("attr_value: %d", uvc_attr_value);
    }
    else
    {
        DLOG_Error("get attribution is not supported");
    }

    return;
}


void command_setUVCAttribute(char *index, char *value)
{
    uint8_t     uvc_attr_index = 0;
    int32_t     uvc_attr_value = 0;

    uvc_attr_index = (uint8_t)strtol(index, NULL, 0);
    uvc_attr_value = strtol(value, NULL, 0);

    if (uvc_attr_index >= ENUM_HAL_UVC_MAX_NUM)
    {
        DLOG_Error("invalid attribute");

        command_uvchelp();

        return;
    }

    DLOG_Error("attr_index: %d, attr_value: %d", uvc_attr_index, uvc_attr_value);

    if (HAL_OK != HAL_USB_HOST_SetUVCAttr(uvc_attr_index, uvc_attr_value))
    {
        DLOG_Error("set attribution is not supported");
    }

    return;
}


void command_uvchelp(void)
{
    uint32_t     uvc_supported_attr_bitmap;

    uvc_supported_attr_bitmap = HAL_USB_GetUVCProcUnitControls();

    DLOG_Critical("UVC Attribution Usage: setuvcattr <index> <value>");
    DLOG_Critical("UVC Attribution Usage: getuvcattr <index> <type>");

    DLOG_Critical("supported Attribution Bitmap: 0x%08x", uvc_supported_attr_bitmap);

    DLOG_Critical("UVC Attribution Index");
    DLOG_Critical("%d:    BRIGHTNESS", ENUM_HAL_UVC_BRIGHTNESS);
    DLOG_Critical("%d:    CONTRAST", ENUM_HAL_UVC_CONTRAST);
    DLOG_Critical("%d:    HUE", ENUM_HAL_UVC_HUE);
    DLOG_Critical("%d:    SATURATION", ENUM_HAL_UVC_SATURATION);
    DLOG_Critical("%d:    SHARPNESS", ENUM_HAL_UVC_SHARPNESS);
    DLOG_Critical("%d:    GAMMA", ENUM_HAL_UVC_GAMMA);
    DLOG_Critical("%d:    WHITE_BALANCE_TEMP", ENUM_HAL_UVC_WHITE_BALANCE_TEMP);
    DLOG_Critical("%d:    WHITE_BALANCE_COMP", ENUM_HAL_UVC_WHITE_BALANCE_COMP);
    DLOG_Critical("%d:    BACKLIGHT_COMP", ENUM_HAL_UVC_BACKLIGHT_COMP);
    DLOG_Critical("%d:    GAIN", ENUM_HAL_UVC_GAIN);
    DLOG_Critical("%d:   PWR_LINE_FREQ", ENUM_HAL_UVC_PWR_LINE_FREQ);
    DLOG_Critical("%d:   HUE_AUTO", ENUM_HAL_UVC_HUE_AUTO);
    DLOG_Critical("%d:   WHITE_BALANCE_TEMP_AUTO", ENUM_HAL_UVC_WHITE_BALANCE_TEMP_AUTO);
    DLOG_Critical("%d:   WHITE_BALANCE_COMP_AUTO", ENUM_HAL_UVC_WHITE_BALANCE_COMP_AUTO);
    DLOG_Critical("%d:   DIGITAL_MULTI", ENUM_HAL_UVC_DIGITAL_MULTI);
    DLOG_Critical("%d:   DIGITAL_MULTI_LIMIT", ENUM_HAL_UVC_DIGITAL_MULTI_LIMIT);

    DLOG_Critical("UVC Attribution Type");
    DLOG_Critical("0x%02x: GET_CUR", HAL_UVC_GET_CUR);
    DLOG_Critical("0x%02x: GET_MIN", HAL_UVC_GET_MIN);
    DLOG_Critical("0x%02x: GET_MAX", HAL_UVC_GET_MAX);
    DLOG_Critical("0x%02x: GET_RES", HAL_UVC_GET_RES);
    DLOG_Critical("0x%02x: GET_LEN", HAL_UVC_GET_LEN);
    DLOG_Critical("0x%02x: GET_INFO", HAL_UVC_GET_INFO);
    DLOG_Critical("0x%02x: GET_DEF", HAL_UVC_GET_DEF);

    DLOG_Output(200);
}

