/*****************************************************************************

*****************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "test_time_delay.h"
#include "debuglog.h"
#include "hal_ret_type.h"
#include "hal_gpio.h"
#include "systicks.h"
#include "hal.h"


void command_TestUsDelayPrecision(char *cnt, char *us, char *gpio)
{
    unsigned int u32_cnt = strtoul(cnt, NULL, 0);
    unsigned int u32_us = strtoul(us, NULL, 0);
    unsigned int e_gpio = strtoul(gpio, NULL, 0);
    unsigned int tmpCnt = 0;

    DLOG_Critical("cnt:%d us:%d gpio:%d", u32_cnt, u32_us, e_gpio);
    
    HAL_GPIO_OutPut((ENUM_HAL_GPIO_NUM)e_gpio);

    while(tmpCnt++ < u32_cnt)
    {
        HAL_GPIO_SetPin((ENUM_HAL_GPIO_NUM)e_gpio, HAL_GPIO_PIN_SET);
        SysTicks_DelayUS((uint64_t)u32_us);
        HAL_GPIO_SetPin((ENUM_HAL_GPIO_NUM)e_gpio, HAL_GPIO_PIN_RESET);
        SysTicks_DelayUS((uint64_t)u32_us);
    }
}

void command_TestUsTick(char *cnt, char *us, char *gpio)
{
    unsigned int u32_cnt = strtoul(cnt, NULL, 0);
    uint64_t u64_us = (uint64_t)strtoul(us, NULL, 0);
    unsigned int e_gpio = strtoul(gpio, NULL, 0);
    unsigned int tmpCnt = 0;
    uint64_t pre_tick;
    unsigned int i = 0;
    

    DLOG_Critical("cnt:%d us:%d gpio:%d", u32_cnt, u64_us, e_gpio);
    
    HAL_GPIO_OutPut((ENUM_HAL_GPIO_NUM)e_gpio);

    while(HAL_GetSysUsTick() % u64_us);
    pre_tick = HAL_GetSysUsTick();
    
    while(tmpCnt++ < u32_cnt)
    {
        while(HAL_GetSysUsTick() < (pre_tick + u64_us));
        
        pre_tick = HAL_GetSysUsTick();
    
        HAL_GPIO_SetPin((ENUM_HAL_GPIO_NUM)e_gpio, (ENUM_HAL_GPIO_PinState)(i++ % 2));
    }
}


