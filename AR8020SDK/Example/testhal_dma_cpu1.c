/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: testhal_pwm.c
Description: 
Author: Wumin @ Artosy Software Team
Version: 0.0.1
Date: 2016/12/19
History:
         0.0.1    2016/12/19    test pwm
         0.0.2    2017/3/27     seperated to share
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "debuglog.h"
#include "hal_dma.h"
#include "md5.h"
#include "data_type.h"
#include "cmsis_os.h"
#include "systicks.h"
#include "auto_test.h"
#include "hal.h"
#include "cpu_info.h"
#include "memory.h"


uint8_t src0_buffer_task0[1024*3];
uint8_t dst0_buffer_task0[1024*3];

uint8_t src1_buffer_task0[1024*3];
uint8_t dst1_buffer_task0[1024*3];


void test_dma_cpu1(void)
{
    uint32_t i    = 0;
    uint32_t loop = 0;
    uint32_t ret0, ret1;

    HAL_DMA_init(DMA_CHAN2 | DMA_CHAN3);
    uint32_t start = SysTicks_GetTickCount();

    for(i = 0 ; i < sizeof(src0_buffer_task0); i++)
    {
        src0_buffer_task0[i] = ((i + loop) & 0xff);
    }

    for(i = 0 ; i < sizeof(src1_buffer_task0); i++)
    {
        src1_buffer_task0[i] = ((i + loop) & 0xff);
    }

    ret0 = HAL_DMA_Transfer(DMA_CHAN2, (uint32_t)src0_buffer_task0, (uint32_t)dst0_buffer_task0, sizeof(src0_buffer_task0), 0);
    if (ret0 != HAL_OK)
    {
        DLOG_Error("HAL_DMA_Transfer DMA_CHAN2 %x", ret0);
    }

    ret1 = HAL_DMA_Transfer(DMA_CHAN3, (uint32_t)src1_buffer_task0, (uint32_t)dst1_buffer_task0, sizeof(src1_buffer_task0), 0);
    if (ret1 != HAL_OK)
    {
        DLOG_Error("HAL_DMA_Transfer DMA_CHAN3 %x", ret1);
    }

    HAL_Delay(1000);

    //check data
    for(i = 0 ; i < sizeof(src0_buffer_task0); i++)
    {
        if (src0_buffer_task0[i] != dst0_buffer_task0[i])
        {
            DLOG_Warning("dma Error src0");
            break;
        }
    }

    for(i = 0 ; i < sizeof(src1_buffer_task0); i++)
    {
        if (src1_buffer_task0[i] != dst1_buffer_task0[i])
        {
            DLOG_Warning("dma Error src1");
            break;
        }
    }
}
