#ifndef __TEST_GC2145_H__
#define	__TEST_GC2145_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "../isp_xc6130/xc6130.h"

#define GC2145_I2C_ADDR         (0x78 >> 1)
#define GC2145_COMPONENT        (I2C_Component_0)
#define GC2145_I2C_MAX_DELAY    (2)

#define GC2145_PWDN                 (HAL_GPIO_NUM101)
#define GC2145_RESET                (HAL_GPIO_NUM97)


int32_t GC2145_WriteReg(uint16_t u16_Reg, uint8_t u8_Val);

int32_t GC2145_ReadReg(uint16_t u16_Reg, uint8_t *pu8_Val);

int32_t GC2145_DownloadFirmware(STRU_CAMERA_REG_VALUE *pst_cfg, uint32_t ArySize);

void GC2145_PinSet(void);

void GC2145_Init(uint8_t par_no);


#ifdef __cplusplus
}
#endif 

#endif
