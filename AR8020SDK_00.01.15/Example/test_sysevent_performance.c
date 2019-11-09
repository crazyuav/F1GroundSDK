#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sys_event.h"
#include "test_sysevent_performance.h"
#include "debuglog.h"
#include "cpu_info.h"
#include "data_type.h"



uint32_t Eventcnt = 0;
static void EVENT_PerformanceTestCallBack(void * p)
{
    static uint32_t getCnt = 0;
    static uint32_t sendCnt = 0;
    uint8_t *pd = (uint8_t *)p;
    ENUM_CPU_ID cpuId = CPUINFO_GetLocalCpuId();
    uint8_t data[SYS_EVENT_HANDLER_PARAMETER_LENGTH];

    if((pd[0] & (1 << cpuId)) != 0)
    {
        getCnt += 1;
        DLOG_Critical("cpu %d get %d itms", cpuId, getCnt);

        if (ENUM_CPU0_ID == cpuId)
        {
            return;
        }
        
        memset (data, 0x00, SYS_EVENT_HANDLER_PARAMETER_LENGTH);
        data[0] = (1 << ENUM_CPU0_ID);

        if (TRUE == SYS_EVENT_NotifyInterCore(SYS_EVENT_ID_PERFORMACE_TEST, (void *)(&data)))
        {
            sendCnt += 1;
            DLOG_Critical("cpu %d send %d itms", cpuId, sendCnt);
        }
    }
}


uint32_t pre_cnt;
static void CheckCnt(void * p)
{
    Eventcnt ++;
    uint32_t cur_cnt = *(uint32_t *)p;
    if (cur_cnt - pre_cnt == 0)
    {
        DLOG_Critical("same cnt %d %d", cur_cnt, pre_cnt);
    }
    pre_cnt = cur_cnt;
}

void EVENT_PerformanceTestInit(void)
{
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_PERFORMACE_TEST, EVENT_PerformanceTestCallBack);
}


void EVENT_PerformanceCheckCnt(void)
{
    uint8_t ret = SYS_EVENT_RegisterHandler(SYS_EVENT_ID_PERFORMACE_TEST, CheckCnt);
    DLOG_Warning("ret = %d", ret);
}

void EVENT_PerformancePrintCnt(void)
{
    DLOG_Warning("Eventcnt=%d", Eventcnt);
}
