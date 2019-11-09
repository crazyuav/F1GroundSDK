#ifndef __XC6130_H__
#define	__XC6130_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include "debuglog.h"

#define XC6130_I2C_ADDR         (0x36 >> 1)
#define XC6130_COMPONENT        (I2C_Component_3)
#define XC6130_I2C_MAX_DELAY    (5)

#define XC6130_PWDN                 (HAL_GPIO_NUM53)


typedef struct
{
    uint16_t u16_regAddr;
    uint8_t  u8_val;
} STRU_CAMERA_REG_VALUE;


void XC6130_I2cByPassOn(void);

void XC6130_I2cByPassOff(void);

int32_t XC6130_WriteReg(uint16_t u16_xc6130Reg, uint8_t u8_xc6130Val);

int32_t XC6130_ReadReg(uint16_t u16_xc6130Reg, uint8_t *pu8_xc6130Val);

int32_t XC6130_DownloadFirmware(STRU_CAMERA_REG_VALUE *pst_xc6130, uint32_t u32_xc6130ArySize);

void Xc6130MipiInit(void);

void XC6130_ReadChipId(void);

void SwitchI2CToXc6130(void);

void XC6130_Init(uint8_t par_no);


#ifdef __cplusplus
}
#endif 

#endif
