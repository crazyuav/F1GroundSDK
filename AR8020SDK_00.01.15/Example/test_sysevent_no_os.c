#include <stdint.h>
#include <stdlib.h>
#include "sys_event.h"
#include "test_sysevent.h"
#include "debuglog.h"
#include "cmsis_os.h"
#include "hal.h"
#include "cpu_info.h"
#include "systicks.h"
#include "auto_test.h"
#include "data_type.h"


uint32_t g_delayMs = 1;
uint32_t g_totalNum = 100;

void command_TestLoadPerformance(char * delayMs, char * totalNum)
{
    uint32_t dataloop = 0;
    uint32_t i;

    g_delayMs  = strtoul(delayMs, NULL, 0);
    g_totalNum = strtoul(totalNum, NULL, 0);

    DLOG_Critical("delayMs:%d totalNum:%d", g_delayMs, g_totalNum);

    while(g_totalNum > 0)
    {
        while (0 == SYS_EVENT_NotifyInterCore(SYS_EVENT_ID_PERFORMACE_TEST, (void *)(&dataloop)))
        {
            HAL_Delay(10);
        }
        dataloop ++;
        g_totalNum -= 1;

        HAL_Delay(g_delayMs);
    }

    DLOG_Critical("done");
}

