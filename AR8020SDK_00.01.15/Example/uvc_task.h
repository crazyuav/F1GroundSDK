#ifndef __UVC_TASK_H
#define __UVC_TASK_H


#include "hal_usb_host.h"


#define UVC_TRANSFER_SIZE_ONCE                  (8192)
#define UVC_ENDPOINT_FOR_TRANSFER               (0x85)



void USBH_UVCInit(uint16_t u16_width, uint16_t u16_height, ENUM_HAL_USB_UVC_DATA_TYPE e_data_type);
void USBH_ProcYUV(uint8_t u8_portId);
void USBH_ProcH264(uint8_t u8_portId);
void command_ViewUVC(void);
void USBH_UVCTask(void const *argument);
void command_startUVC(char *width, char *height);
void command_saveUVC(void);
void command_stopSaveUVC(void);
void command_showUVC(void);
void command_getUVCAttribute(char *index, char *type);
void command_setUVCAttribute(char *index, char *value);
void command_uvchelp(void);
static void USBH_ShowUVC(STRU_UVC_VIDEO_FRAME_FORMAT *stFrameFormat,
                          uint8_t usb_portId,
                          uint8_t *frame_buff,
                          uint32_t frame_size);
static void USBH_SaveUVC(uint8_t *frame_buff, uint32_t frame_size);

#endif
