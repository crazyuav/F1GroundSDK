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

void command_dma(char * u32_src, char *u32_dst, char *u32_byteNum)
{
    unsigned int iSrcAddr;
    unsigned int iDstAddr;
    unsigned int iNum;
    uint8_t    md5_value[MD5_SIZE];
    uint8_t    md5_valueDst[MD5_SIZE];
    int i = 0;
    MD5_CTX md5;
    HAL_RET_T ret;
    uint8_t result = 1;
    uint64_t t_diff[3] = {0, 0};

    DLOG_Critical("auto_test>%s %s %s %s", __FUNCTION__, u32_src, u32_dst, u32_byteNum);

    iDstAddr    = strtoul(u32_dst, NULL, 0);
    iSrcAddr    = strtoul(u32_src, NULL, 0);
    iNum        = strtoul(u32_byteNum, NULL, 0);

    AUTO_TEST_TimeRecord();
    ret = HAL_DMA_Transfer(DMA_CHAN0, iSrcAddr, iDstAddr, iNum, 10);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();   
    MD5Init(&md5);
    
    AUTO_TEST_TimeRecord();
    MD5Update(&md5, (uint8_t *)iSrcAddr, iNum);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    
    AUTO_TEST_TimeRecord();
    MD5Final(&md5, md5_value);
    t_diff[2] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    i = 0;
    DLOG_Info("src MD5 = 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++],
                                    md5_value[i++]);

    memset(&md5, 0, sizeof(MD5_CTX));
    MD5Init(&md5);
    MD5Update(&md5, (uint8_t *)iDstAddr, iNum);
    memset(&md5_valueDst, 0, sizeof(md5_valueDst));
    MD5Final(&md5, md5_valueDst);
    i = 0;
    DLOG_Info("dst MD5 = 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++],
                                    md5_valueDst[i++]);
    for(i = 0; i < MD5_SIZE; i++)
    {
        if (md5_value[i] != md5_valueDst[i])
        {
            result = 0;
            break;
        }
    }
    
    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_DMA_Transfer", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("MD5Update", (uint32_t)t_diff[1]);
    AUTO_TEST_PrintApiRunTime("MD5Final", (uint32_t)t_diff[1]);
    DIV_LINE(DIV_LINE_END);
}

void command_dma_asyn(char *cycle, char *u32_byteNum)
{
    unsigned char *pSrcAddr;
    unsigned char *pDstAddr;
    unsigned int iNum;
    unsigned int iCycle;
    uint8_t result = 1;
    uint32_t i;
    uint32_t j;
    HAL_RET_T ret;

    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, cycle, u32_byteNum);

    iNum        = strtoul(u32_byteNum, NULL, 0);
    iCycle      = strtoul(cycle, NULL, 0);

    for(i=0; i<iCycle; i++)
    {
        pSrcAddr = malloc_safe(iNum);
        pDstAddr = malloc_safe(iNum);

        DLOG_Info("src:0x%x dst:0x%x", pSrcAddr, pDstAddr);

        if((NULL == pSrcAddr) || (NULL == pDstAddr))
        {
            result = 0;
            DLOG_Error("mallo failed.");
        }

        ret = HAL_DMA_Transfer(DMA_CHAN0, (uint32_t)(pSrcAddr), (uint32_t)(pDstAddr), iNum, 0);

        if (HAL_OK != ret)
        {
            result = 0;
            DLOG_Error("cycle %d dam transmission fail.", i);
            break;
        }

        if(result)
        {
            DLOG_Info("delay 10*%dms", iCycle);
            HAL_Delay(1);

            for(j=0; j<iNum; j++)
            {
                if (pSrcAddr[j] != pDstAddr[j])
                {
                    result = 0;
                    DLOG_Error("cycle %d dam compare fail.%d", i, j);
                    break;
                }
            }
        }

        free_safe(pSrcAddr);
        free_safe(pDstAddr);
    }
    
    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    DIV_LINE(DIV_LINE_END);
}


void command_test_dma_loop(char * u32_src, char *u32_dst, char *u32_byteNum)
{
	unsigned int i = 0;
	
	while(1)
	{
		command_dma(u32_src, u32_dst, u32_byteNum);
		DLOG_Info("i = %d\n", i++);
	}
}

typedef enum {
	DMA_blocked = 0,
	DMA_noneBlocked,
	DMA_blockTimer
} ENUM_blockMode;

static void delay_us(uint64_t us)
{
	uint64_t start;
	uint64_t end;
	start = SysTicks_GetUsTickCount();

	while(1)
	{
		if (SysTicks_GetUsTickCount() >= start + us)
		{
			break;
		}
	}
}

extern uint32_t DMA_forDriverTransfer(uint32_t u32_srcAddr, uint32_t u32_dstAddr, uint32_t u32_transByteNum, 
											ENUM_blockMode e_blockMode, uint32_t u32_ms);


void command_test_dma_driver(char * u32_src, char *u32_dst, char *u32_byteNum, 
											char* u32_ms)
{
	unsigned int iSrcAddr;
	unsigned int iDstAddr;
	unsigned int iNum;
	unsigned int iTimeout;

	iDstAddr = strtoul(u32_dst, NULL, 0);
	iSrcAddr = strtoul(u32_src, NULL, 0);
	iNum = strtoul(u32_byteNum, NULL, 0);
	iTimeout = strtoul(u32_ms, NULL, 0);

	while(1)
	{
		DMA_forDriverTransfer(iSrcAddr, iDstAddr, iNum, DMA_noneBlocked, iTimeout);
		delay_us(125);
		
		HAL_DMA_Transfer(DMA_CHAN0, iSrcAddr, iDstAddr, iNum, 0);
		delay_us(125);
	}
}


void command_test_dma_user(char * u32_src, char *u32_dst, char *u32_byteNum, 
											char* u32_ms)
{
	unsigned int iSrcAddr;
	unsigned int iDstAddr;
	unsigned int iNum;
	unsigned int iTimeout;

	iDstAddr = strtoul(u32_dst, NULL, 0);
	iSrcAddr = strtoul(u32_src, NULL, 0);
	iNum = strtoul(u32_byteNum, NULL, 0);
	iTimeout = strtoul(u32_ms, NULL, 0);
	
	HAL_DMA_Transfer(DMA_CHAN0, iSrcAddr, iDstAddr, iNum, iTimeout);
}




uint8_t src0_buffer_task0[1024*3];
uint8_t dst0_buffer_task0[1024*3];

uint8_t src1_buffer_task0[1024*3];
uint8_t dst1_buffer_task0[1024*3];


void dma_task_cpu0(void const *argument)
{
    uint32_t i    = 0;
    uint32_t loop = 0;
    uint32_t ret0, ret1;

    HAL_DMA_init(DMA_CHAN0 | DMA_CHAN1);

    uint32_t start = SysTicks_GetTickCount();
    while (1)
    {
        //input the src data
        for(i = 0 ; i < sizeof(src0_buffer_task0); i++)
        {
            src0_buffer_task0[i] = ((i + loop) & 0xff);
        }

        for(i = 0 ; i < sizeof(src1_buffer_task0); i++)
        {
            src1_buffer_task0[i] = ((i + loop) & 0xff);
        }

        //DMA source data to DMA data
        ret0 = HAL_DMA_Transfer(DMA_CHAN0, (uint32_t)src0_buffer_task0, (uint32_t)dst0_buffer_task0, sizeof(src0_buffer_task0), 0);
        if (ret0 != HAL_OK)
        {
            DLOG_Error("HAL_DMA_Transfer DMA_CHAN0 %x", ret0);
        }
    
        ret1 = HAL_DMA_Transfer(DMA_CHAN1, (uint32_t)src1_buffer_task0, (uint32_t)dst1_buffer_task0, sizeof(src1_buffer_task0), 0);
        if (ret1 != HAL_OK)
        {
            DLOG_Error("HAL_DMA_Transfer DMA_CHAN1 %x", ret1);
        }

        //check data
        for(i = 0 ; i < sizeof(src0_buffer_task0); i++)
        {
            if (src0_buffer_task0[i] != dst0_buffer_task0[i])
            {
                DLOG_Warning("src0 Error");
                break;
            }
        }

        for(i = 0 ; i < sizeof(src1_buffer_task0); i++)
        {
            if (src1_buffer_task0[i] != dst1_buffer_task0[i])
            {
                DLOG_Warning("src1 Error");
                break;
            }
        }

        HAL_Delay(2000);
    }
}

void test_dma_task_cpu0(void)
{
    HAL_DMA_init(DMA_CHAN0 | DMA_CHAN1);

    osThreadDef(DMATask0, dma_task_cpu0, osPriorityNormal, 0, 16 * 128);
    osThreadCreate(osThread(DMATask0), NULL);
}
