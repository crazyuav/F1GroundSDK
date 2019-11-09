#ifndef __HAL_USB_OTG_H__
#define __HAL_USB_OTG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_ret_type.h"


typedef enum
{
    HAL_USB_PORT_0 = 0,
    HAL_USB_PORT_1,
    HAL_USB_PORT_NUM,
} ENUM_HAL_USB_PORT;


typedef enum
{
    HAL_USB_DR_MODE_DEVICE  = 0,
    HAL_USB_DR_MODE_HOST    = 1,
    HAL_USB_DR_MODE_OTG     = 2,
} ENUM_HAL_USB_DR_MODE;


/**
* @brief    initiate the USB Port
* @param  void
* @retval   void
* @note  
*/
void HAL_USB_Init(ENUM_HAL_USB_PORT e_usbPort, ENUM_HAL_USB_DR_MODE e_usbDrMode);

/**
* @brief    Configure the parameters optimized by IC Designer
* @param  void
* @retval   void
* @note  
*/
void HAL_USB_ConfigPHY(void);

HAL_RET_T HAL_USB_SendCtrl(uint8_t *buff, uint32_t u32_len, uint8_t u8_portId);


#ifdef __cplusplus
}
#endif

#endif

