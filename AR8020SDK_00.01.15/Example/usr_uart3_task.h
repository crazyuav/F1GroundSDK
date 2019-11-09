#ifndef __TEST_USR_UART3__
#define __TEST_USR_UART3__

#ifdef __cplusplus
extern "C"
{
#endif


void usr_bypass_uart_task(int dev_type);
uint32_t bypass_write_usb2uart(uint8_t *pu8_rxBuf, uint8_t u8_len);
uint32_t bypass_read_uart2usb(uint8_t *pu8_rxBuf,uint32_t max_len);
uint8_t get_fac_bypass_mode(void);
uint32_t nothing_todo(uint8_t *pu8_rxBuf, uint8_t u8_len);


#ifdef __cplusplus
}
#endif

#endif

