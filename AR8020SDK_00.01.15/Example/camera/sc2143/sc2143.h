#ifndef __SC2143_H__
#define	__SC2143_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "../isp_xc6130/xc6130.h"

#define SC2143_I2C_ADDR         (0x60 >> 1)
#define SC2143_COMPONENT        (I2C_Component_3)
#define SC2143_I2C_MAX_DELAY    (2)

#define SC2143_PWDN                 (HAL_GPIO_NUM53)
#define SC2143_RESET_B              (HAL_GPIO_NUM52)


int32_t SC2143_WriteReg(uint16_t u16_sc2143Reg, uint8_t u8_sc2143Val);

int32_t SC2143_ReadReg(uint16_t u16_sc2143Reg, uint8_t *pu8_sc2143Val);

int32_t SC2143_DownloadFirmware(STRU_CAMERA_REG_VALUE *pst_sc2143, uint32_t u32_sc2143ArySize);

void SC2143_ReadChipId(void);

void XC6130_SC2143_PinSet(void);

void SwitchI2CToSc2143(void);

void SC2143_Init(uint8_t par_no);


#ifdef __cplusplus
}
#endif 

#endif
