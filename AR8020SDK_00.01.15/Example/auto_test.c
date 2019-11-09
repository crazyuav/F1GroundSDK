#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "systicks.h"
#include "auto_test.h"
#include "hal.h"


static uint64_t t_start = 0;
static uint64_t t_end = 0;

void AUTO_TEST_PrintResult(uint8_t *result)
{
	DLOG_Critical("auto_test>%s",result);
}

void AUTO_TEST_PrintApiRunTime(uint8_t *apiName, uint32_t timeUs)
{
	DLOG_Critical("auto_test>%s:%dUs",apiName, timeUs);
}

void AUTO_TEST_TimeRecord(void)
{
    t_start = HAL_GetSysUsTick();	
}

uint64_t AUTO_TEST_CalcTimeDiff(void)
{
    t_end = HAL_GetSysUsTick();

    return SysTicks_GetUsDiff(t_start, t_end);
}

