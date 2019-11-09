#ifndef __USR_PROTOCOL__
#define __USR_PROTOCOL__

#ifdef __cplusplus
extern "C"
{
#endif

#define CMD_VIDEO_BUF_DEEP 0x81
#define CMD_GRD_MOD_STATUS 0x82
#define CMD_SKY_MOD_STATUS 0x83
#define CMD_WRITE_USB_BYPASS 0x84
#define CMD_READ_USB_BYPASS 0x85
#define CMD_WRITE_UART5_BYPASS 0x86
#define CMD_READ_UART5_BYPASS 0x87
#define CMD_DBUG_INFO 0x88
#define CMD_GET_AUTOTEST_STATUS  0x89

uint8_t cmd_ack_handle(void *msg, uint8_t port_id);

uint8_t cmd_usb_bypass_read_handle(void *msg, uint8_t port_id);

uint8_t cmd_usb_bypass_write_handle(void *msg, uint8_t port_id);

uint8_t cmd_usb_bypass_read_uart5_handle(void *msg, uint8_t port_id);

uint8_t cmd_usb_bypass_write_uart5_handle(void *msg, uint8_t port_id);

unsigned int data_check(char *buffer,int len);

void print_msg(void *msg);


uint8_t get_grd_status_info(unsigned char *buf, int *len);

uint8_t get_sky_status_info(unsigned char *buf, int *len);
#ifdef __cplusplus
}
#endif

#endif

