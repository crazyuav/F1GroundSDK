#include "test_bypass_encoder.h"
#include "sys_event.h"
#include "debuglog.h"
#include "hal_usb_device.h"


static uint8_t                      g_bitrate_info[8];
static ENUM_HAL_SRAM_VIDEO_CHANNEL  g_eBypassChannel;
static uint8_t                      g_usb_plug_out_flag;

void bitrate_change_callback(void* p)
{
    uint8_t br_idx  = ((STRU_SysEvent_BB_ModulationChange *)p)->encoder_brcidx;
    uint8_t ch      = ((STRU_SysEvent_BB_ModulationChange *)p)->u8_bbCh;

    if (0 == ch)
    {
        g_bitrate_info[0] = br_idx;

        DLOG_Info("H264 bitidx ch0: %d \r\n", br_idx);
    }
}

void usb_plug_out_callback(void* p)
{
    g_usb_plug_out_flag = 1;
    HAL_SRAM_DisableSkyBypassVideo(g_eBypassChannel);
    DLOG_Warning("usb plug out");
}

void bypass_encoder_init(ENUM_HAL_SRAM_VIDEO_CHANNEL e_bypassVideoCh)
{
    g_eBypassChannel = e_bypassVideoCh;
    g_usb_plug_out_flag = 1;

    /* register receive callback to receive video stream from USB host */
    HAL_USB_RegisterEncoderBypassVideo(receive_video_stream);

    /* register callback to receive bitrate info */
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_BB_SUPPORT_BR_CHANGE, bitrate_change_callback);

    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_USB_PLUG_OUT,usb_plug_out_callback);
}


void receive_video_stream(void *video_buff, uint32_t length, uint8_t port_id)
{
    if(g_usb_plug_out_flag)
    {
        g_usb_plug_out_flag = 0;
        /* open channel to transfer */
        HAL_SRAM_EnableSkyBypassVideo(g_eBypassChannel);

    }
    /* received video, now transfer video stream to ground */
    HAL_SRAM_TransferBypassVideoStream(g_eBypassChannel, video_buff, length);

    DLOG_Info("len: %d, port: %d, g_eBypassChannel: %d", length, port_id, g_eBypassChannel);
}


