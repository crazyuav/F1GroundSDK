#include "debuglog.h"
#include "sys_event.h"
#include "hal.h"
#include "hal_sys_ctl.h"
#include "hal_uart.h"
#include "cmd_line.h"
#include "board_watchdog.h"
#include "hal_timer.h"


void CONSOLE_Init(void)
{
    DLOG_Init(CMD_exec_cmd, NULL, DLOG_CLIENT_PROCESSOR);
}

static void Gnd_TIM2_0_IRQHandler(uint32_t u32_vectorNum)
{
    HAL_LED_TOGGLE(CORE_LED_GPIO_YELLOW);
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    HAL_SYS_CTL_Init(NULL);
    WATCHDOG_Init();

    /* initialize the uart */
    CONSOLE_Init();
    dlog_set_output_level(LOG_LEVEL_WARNING);
    DLOG_Critical("cpu1 start!!! \n");

    HAL_TIMER_RegisterTimer(HAL_TIMER_NUM16,1000000, Gnd_TIM2_0_IRQHandler);
    
    /* We should never get here as control is now taken by the scheduler */
    for( ;; )
    {
        SYS_EVENT_Process();
        DLOG_Process(NULL);
    }
} 

