#ifndef __USR_USB__
#define __USR_USB__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_sram_sky.h"

typedef void(*Handler)(uint8_t *dataBuffer, uint32_t datalen);


void bypass_encoder_init(void);
void usr_usb0_interface(void);
uint32_t get_usb_recv_size0(void);
uint32_t get_usb_recv_size1(void);
uint32_t usb_bypass_write(void *pbuf, uint32_t data_len);
uint32_t usb_bypass_read(void *pbuf, uint32_t max_len);


void usb_cdc_registerReceiveHandler(Handler handler);
void usb_cdc_sendData(uint8_t *dataBuffer, uint16_t datalen);

#ifdef __cplusplus
}
#endif

#endif

