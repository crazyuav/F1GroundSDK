#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "hal.h"
#include "cpu_info.h"
#include "test_memory.h"
#include "memory.h"
#include "memory_itcm.h"
#include "auto_test.h"
#include "systicks.h"
#include "memory.h"


void command_TestMemory(char * cpu, char * totalNum, char * len)
{
	uint32_t tmp_cpu = strtoul(cpu, NULL, 0);
	uint32_t local_cpu = (uint32_t)CPUINFO_GetLocalCpuId();
    uint8_t *p;
    uint32_t cnt = 0;
    uint64_t t_diff = 0;
    

    DLOG_Critical("auto_test>%s %s %s %s", __FUNCTION__, cpu, totalNum, len);
	
	if (tmp_cpu == local_cpu)
	{
		uint32_t tmp_num = strtoul(totalNum, NULL, 0);
		uint32_t tmp_len = strtoul(len, NULL, 0);

        DLOG_Critical("%d %d %d",tmp_cpu, tmp_num, tmp_len);
		DLOG_Critical("cpu %d memory test start ...", local_cpu);
		while(cnt < tmp_num)
		{
		    AUTO_TEST_TimeRecord();
			p = (uint8_t *)malloc_safe(tmp_len);
            t_diff += (uint32_t)AUTO_TEST_CalcTimeDiff();
            DLOG_Critical("%d 0x%x", cnt++, (uint32_t)p);
            if (NULL == p)
            {
                break;
            }
			//memset(p, 0x00, tmp_len);
			//free(p);
		}
		DLOG_Critical("cpu %d memory test end ...", local_cpu);
    }
    t_diff /= cnt;
    
    DIV_LINE(DIV_LINE_REPORT);
    if (NULL == p)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("malloc", (uint32_t)t_diff);
    DIV_LINE(DIV_LINE_END);
}

void command_TestItcmMemory(char * cpu, char * totalNum, char * len)
{
	uint32_t tmp_cpu = strtoul(cpu, NULL, 0);
	uint32_t local_cpu = (uint32_t)CPUINFO_GetLocalCpuId();
    
	
	if (tmp_cpu == local_cpu)
	{
		uint32_t tmp_num = strtoul(totalNum, NULL, 0);
		uint32_t tmp_len = strtoul(len, NULL, 0);
		uint32_t cnt = 0;
		uint8_t *p;

        DLOG_Critical("%d %d %d",tmp_cpu, tmp_num, tmp_len);
		DLOG_Critical("cpu %d itcm memory test start ...", local_cpu);
		while(cnt < tmp_num)
		{
			p = (uint8_t *)malloc_itcm_safe(tmp_len);
            DLOG_Critical("%d 0x%x", cnt++, (uint32_t)p);
            if (NULL == p)
            {
                break;
            }
			//memset(p, 0x00, tmp_len);
			//free_itcm(p);
		}
		DLOG_Critical("cpu %d itcm memory test end ...", local_cpu);
    }
}

