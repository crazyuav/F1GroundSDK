/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: testhal_gpio.c
Description: test gpio
Author: SW
Version: 1.0
Date: 2016/12/19
History: test gpio
*****************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "debuglog.h"
#include "hal_ret_type.h"
#include "hal_gpio.h"
#include "testhal_pwm.h"
#include "auto_test.h"
#include "systicks.h"

static uint32_t volatile g_u32GpioCount = 0;
static uint32_t volatile g_u32GpioCount2 = 0;
static uint32_t volatile g_u32GpioCountAuto = 0;

void commandhal_TestGetGpio(uint8_t *gpionum)
{
    uint8_t u8_GpioNum = strtoul(gpionum, NULL, 0);
    uint32_t u32_Gpioval=0;
    HAL_GPIO_InPut(u8_GpioNum);
    HAL_GPIO_GetPin(u8_GpioNum,&u32_Gpioval);
    DLOG_Info("gpio %d state=%d\n",u8_GpioNum,u32_Gpioval);

}

void commandhal_TestSetGpio(uint8_t *gpionum, uint8_t *val)
{
    uint8_t u8_GpioNum = strtoul(gpionum, NULL, 0);
    uint8_t u8_GpioVal = strtoul(val, NULL, 0);

    HAL_GPIO_SetMode(u8_GpioNum, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(u8_GpioNum);
    HAL_GPIO_SetPin(u8_GpioNum, u8_GpioVal);

    DLOG_Info("gpio %d state=%d\n",u8_GpioNum, u8_GpioVal);

}

static void GPIOhal_IRQHandler(uint32_t u32_vectorNum)
{ 
    if(((g_u32GpioCount)%1000 == 0) && (0 !=g_u32GpioCount))
    {

        DLOG_Info("gpio interrupt0 %d\n",u32_vectorNum);
        DLOG_Output(1000);
    }
    g_u32GpioCount++;    
}

static void GPIOhal_IRQHandler1(uint32_t u32_vectorNum)
{ 
    
    if(((g_u32GpioCount2)%1000 == 0) && (0 !=g_u32GpioCount2))
    {

        DLOG_Info("gpio interrupt1 %d\n",u32_vectorNum);
        DLOG_Output(1000);
    }
    g_u32GpioCount2++;    
}

static void GPIOhal_IRQHandlerAutoTest(uint32_t u32_vectorNum)
{ 
    g_u32GpioCountAuto++; 
}
void commandhal_TestGpioNormal(uint8_t *gpionum, uint8_t *highorlow)
{
	uint8_t u8_GpioNum = strtoul(gpionum, NULL, 0);
    uint8_t u8_GpioValue = strtoul(highorlow, NULL, 0);
    
    //HAL_GPIO_InPut(65);
    HAL_GPIO_SetMode(u8_GpioNum,HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(u8_GpioNum);
    HAL_GPIO_SetPin(u8_GpioNum, u8_GpioValue);	
}

void commandhal_TestGpioInterrupt(uint8_t *gpionum, uint8_t *inttype, uint8_t *polarity)
{
	uint8_t u8_GpioNum = strtoul(gpionum, NULL, 0);
	uint8_t u8_GpioIntType = strtoul(inttype, NULL, 0);
    uint8_t u8_GpioPolarity = strtoul(polarity, NULL, 0);
	
    if(0==HAL_GPIO_RegisterInterrupt(u8_GpioNum, u8_GpioIntType, u8_GpioPolarity, GPIOhal_IRQHandler))
    {
        DLOG_Info("ok %d",u8_GpioNum);
    }
    else
    {
        DLOG_Error("fail %d",u8_GpioNum);
    }
    DLOG_Output(100);
}

void commandhal_TestGpioAndPwm(uint8_t *gpionum, uint8_t *inttype, uint8_t *polarity)
{
	uint8_t u8_GpioNum = strtoul(gpionum, NULL, 0);
	uint8_t u8_GpioIntType = strtoul(inttype, NULL, 0);
    uint8_t u8_GpioPolarity = strtoul(polarity, NULL, 0);
    uint32_t tick = SysTicks_GetTickCount();
    g_u32GpioCountAuto = 0;
    HAL_GPIO_RegisterInterrupt(u8_GpioNum, u8_GpioIntType, u8_GpioPolarity, GPIOhal_IRQHandlerAutoTest);
    commandhal_TestPwmAll();

    while(1000 > SysTicks_GetDiff(tick, SysTicks_GetTickCount()))
    {
        ;
    }
    DIV_LINE(DIV_LINE_REPORT);

    if(g_u32GpioCountAuto > 2)
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    else
    {
        AUTO_TEST_PrintResult(FAILURE);
    }

    DIV_LINE(DIV_LINE_END);
}
