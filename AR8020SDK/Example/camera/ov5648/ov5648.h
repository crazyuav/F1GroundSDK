#ifndef __OV5648_H__
#define	__OV5648_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "../isp_xc6130/xc6130.h"


#define OV5648_I2C_ADDR         (0x6C >> 1)
#define OV5648_COMPONENT        (I2C_Component_3)
#define OV5648_I2C_MAX_DELAY    (2)

#define OV5648_RESET                (HAL_GPIO_NUM52)
#define OV5648_PWDN                 (HAL_GPIO_NUM51)


int32_t OV5648_WriteReg(uint16_t u16_ov5648Reg, uint8_t u8_Val);

int32_t OV5648_ReadReg(uint16_t u16_Reg, uint8_t *pu8_Val);

int32_t OV5648_DownloadFirmware(STRU_CAMERA_REG_VALUE *pst_cfg, uint32_t u32_Size);

void OV5648_ReadChipId(void);

void XC6130_OV5648_PinSet(void);

void SwitchI2CToOv5648(void);

void OV5648_Init(uint8_t par_no);

#ifdef __cplusplus
}
#endif 

#endif
