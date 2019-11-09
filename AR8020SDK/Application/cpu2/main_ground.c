#include "serial.h"
#include "debuglog.h"
#include "test_bb.h"
#include "sys_event.h"
#include "hal_sys_ctl.h"
#include "hal_bb.h"
#include "hal.h"
#include "hal_gpio.h"
#include "bb_customerctx.h"
#include "test_bb.h"
#include "cmd_line.h"
#include "board_watchdog.h"
#include "test_bb_led_ctrl.h"
#include "test_bb.h"
#include "hal_uart.h"
#include "hal_timer.h"

void console_init(uint32_t uart_num, uint32_t baut_rate)
{
    // HAL_UART_Init(HAL_UART_COMPONENT_3, HAL_UART_BAUDR_115200, NULL);
    DLOG_Init(CMD_exec_cmd, NULL ,DLOG_CLIENT_PROCESSOR);
}

static void Gnd_TIM2_1_IRQHandler(uint32_t u32_vectorNum)
{
    HAL_LED_TOGGLE(CORE_LED_GPIO_BROWN);
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) 
{ 
    STRU_HAL_SYS_CTL_CONFIG *pst_cfg;
    HAL_SYS_CTL_GetConfig(&pst_cfg);
    HAL_SYS_CTL_Init(pst_cfg);

    WATCHDOG_InitCpu2();
    console_init(2, 115200);
    dlog_set_output_level(LOG_LEVEL_WARNING); 
    DLOG_Critical("main ground function start \n");
    HAL_BB_RegisterLnaFuntion(lna_open,lna_bypass);
    
    HAL_BB_InitGround(NULL);

    HAL_TIMER_RegisterTimer(HAL_TIMER_NUM17,1000000, Gnd_TIM2_1_IRQHandler);
    
    for( ;; )
    {
        SYS_EVENT_Process();
        DLOG_Process(NULL);
    }
}
