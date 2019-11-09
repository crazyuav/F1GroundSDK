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



#define SYS_EVENT_ID_BB_DATA_BUFFER_FULL              (SYS_EVENT_LEVEL_MIDIUM_MASK | 0x00FF | SYS_EVENT_INTER_CORE_MASK)
#define SYS_EVENT_ID_SEND_REMOTE_EVENT_TEST           (SYS_EVENT_LEVEL_MIDIUM_MASK | 0x00FE)

uint32_t g_delayMs = 1;
uint32_t g_totalNum = 100;


static void test_IdleCallback(void * p);
static void test_RemoteEventCallback(void * p);
static void EVENT_CycleSendTask(void const *argument);

static void test_IdleCallback(void * p)
{
    DLOG_Info("idle function ...");
}

void command_TestSysEventIdle(void)
{
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_IDLE, test_IdleCallback);
}

void command_TestSysEventInterCore(char* ptr)
{
    STRU_SysEvent_BB_DATA_BUFFER_FULL_RATIO_Change p;
    p.BB_Data_Full_Ratio = strtoul(ptr, NULL, 0);
    SYS_EVENT_NotifyInterCore(SYS_EVENT_ID_BB_DATA_BUFFER_FULL, (void*)&p);
}



static void test_RemoteEventCallback(void * p)
{
    uint8_t *pd = (uint8_t *)p;
    
    DLOG_Critical("par:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", \
                   pd[0],pd[1],pd[2],pd[3],pd[4],pd[5],pd[6],pd[7],pd[8],pd[9],pd[10],pd[11]);
}

void command_TestRemoteEventInit(void)
{
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_SEND_REMOTE_EVENT_TEST, test_RemoteEventCallback);
}

void command_TestRemoteEvent(void)
{
    uint8_t data[SYS_EVENT_HANDLER_PARAMETER_LENGTH - 4];
    uint8_t i;
    uint64_t t_diff[1] = {0};

    DLOG_Critical("auto_test>%s %s", __FUNCTION__, "--");

    for(i=0; i<(SYS_EVENT_HANDLER_PARAMETER_LENGTH - 4); i++)
    {
        data[i] = ((i << 4) | i);
    }

    AUTO_TEST_TimeRecord();
    SYS_EVENT_NotifyRemote((uint32_t)(SYS_EVENT_ID_SEND_REMOTE_EVENT_TEST), (void *)data);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    DIV_LINE(DIV_LINE_REPORT);
    AUTO_TEST_PrintResult(PASSED);
    AUTO_TEST_PrintApiRunTime("SYS_EVENT_NotifyRemote", (uint32_t)t_diff[0]);
    DIV_LINE(DIV_LINE_END);
}

static void EVENT_CycleSendTask(void const *argument)
{
    uint32_t i;
    uint8_t data[SYS_EVENT_HANDLER_PARAMETER_LENGTH];

    data[0] = ((1 << ENUM_CPU2_ID) | (1 << ENUM_CPU1_ID));
    for(i=1; i<SYS_EVENT_HANDLER_PARAMETER_LENGTH; i++)
    {
        data[i] = i;
    }
    
    while(1)
    {
        if (g_totalNum > 0)
        {
            SYS_EVENT_NotifyInterCore(SYS_EVENT_ID_PERFORMACE_TEST, (void *)(&data));
            g_totalNum -= 1;
        }
        
        HAL_Delay(g_delayMs);
    }
}

void command_TestLoadPerformance(char * delayMs, char * totalNum)
{
    static uint8_t flag = 0x55;
    
    g_delayMs = strtoul(delayMs, NULL, 0);
    g_totalNum = strtoul(totalNum, NULL, 0);

    DLOG_Critical("g_delayMs:%d g_totalNum:%d", g_delayMs, g_totalNum);

    if (0x55 == flag)
    {
        osThreadDef(EVENTSend_Task, EVENT_CycleSendTask, osPriorityIdle, 0, 4 * 128);
        osThreadCreate(osThread(EVENTSend_Task), NULL);
        flag = 0;
    }
}

