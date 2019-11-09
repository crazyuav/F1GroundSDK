#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debuglog.h"
#include "hal_ret_type.h"
#include "hal_timer.h"
#include "testhal_timer.h"
#include "auto_test.h"
#include "systicks.h"

static volatile uint32_t g_u32TimCount1 = 0;
static volatile uint32_t g_u32TimCount2 = 0;
static volatile uint32_t g_u32TimCountAuto = 0;

void TIMHAL_IRQHandler0(uint32_t u32_vectorNum)
{ 
    if(((g_u32TimCount1)%1000 == 0) && (0 !=g_u32TimCount1))
    {
        DLOG_Info("g_u32TimCount1 %d\n",u32_vectorNum);
        g_u32TimCountAuto++;
    }
    g_u32TimCount1++;

}

void TIMHAL_IRQHandler1(uint32_t u32_vectorNum)
{

    if(((g_u32TimCount2)%1000 == 0) && (0 !=g_u32TimCount2))
    {

        DLOG_Info("g_u32TimCount2 %d\n",u32_vectorNum);
    }
    g_u32TimCount2++;

}

static void Test_TimerInit(uint8_t u8_timerNum,uint8_t u8_timerCount)
{
    HAL_TIMER_RegisterTimer(u8_timerNum, u8_timerCount, TIMHAL_IRQHandler0);
}

void commandhal_TestTim(uint8_t *pu8_timerNum, uint8_t *pu8_timerCount)
{

    uint32_t u32_TimNum = strtoul(pu8_timerNum, NULL, 0);
    uint32_t u32_TimCount = strtoul(pu8_timerCount, NULL, 0);
    
    HAL_TIMER_RegisterTimer(u32_TimNum, u32_TimCount*1000, TIMHAL_IRQHandler0);
    HAL_TIMER_RegisterTimer(u32_TimNum+1, u32_TimCount*500, TIMHAL_IRQHandler1);
}
                                                              
void commandhal_TestTimAll(void)
{
    DLOG_Critical("auto_test>%s", __FUNCTION__);
    uint32_t i = 0;    
    uint32_t countTmp = 0;    
    uint32_t tick = SysTicks_GetTickCount()/1000;    

    DLOG_Critical("tick %d", tick);
    for(i = 0;i <24; i++)
    {
        DLOG_Critical("test timer %d %d %d",i,countTmp , g_u32TimCount1);
        countTmp = g_u32TimCountAuto; 
        HAL_TIMER_RegisterTimer(i, 1000, TIMHAL_IRQHandler0);
        while(g_u32TimCountAuto == countTmp)
        {
            ;
        }   
        HAL_TIMER_Stop(i);
    }

    tick = (SysTicks_GetTickCount()/1000) - tick;    
    DLOG_Critical("tick %d", tick);
    DIV_LINE(DIV_LINE_REPORT);
    if (23 <= tick || tick <= 25)
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    else
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    DIV_LINE(DIV_LINE_END);
}
