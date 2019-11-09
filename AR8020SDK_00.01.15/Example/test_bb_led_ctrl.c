#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_event.h"
#include "debuglog.h"
#include "test_bb_led_ctrl.h"
#include "hal_gpio.h"
#include "memory_config.h"
#include "debuglog.h"
//#include "hal_bb.h"

#define  BLUE_LED_GPIO      GROUND_LED_GPIO_BLUE
#define  RED_LED_GPIO       GROUND_LED_GPIO_RED


void lna_open(void)
{
    HAL_GPIO_InPut(HAL_GPIO_NUM46);
    HAL_GPIO_InPut(HAL_GPIO_NUM47);
    HAL_GPIO_InPut(HAL_GPIO_NUM48);
    HAL_GPIO_InPut(HAL_GPIO_NUM49);
    //DLOG_Warning("SPI_SS/SCK/RXD6/TXD6 INPUT \n");
}

void lna_bypass(void)
{
    HAL_GPIO_OutPut(HAL_GPIO_NUM46);
    HAL_GPIO_OutPut(HAL_GPIO_NUM49);
    HAL_GPIO_SetPin(HAL_GPIO_NUM46,HAL_GPIO_PIN_SET);
    HAL_GPIO_SetPin(HAL_GPIO_NUM49,HAL_GPIO_PIN_SET);
}

void BB_ledGpioInit(void)
{
    uint8_t pinNo[8] = {81, 82, 83, 84, 85, 86, 87, 88};

    for (size_t i = 0; i < sizeof(pinNo)/sizeof(pinNo[0]); i++)
    {
        HAL_GPIO_SetMode(pinNo[i], HAL_GPIO_PIN_MODE2);
        HAL_GPIO_InPut(pinNo[i]);           
    }


    HAL_GPIO_SetMode(RED_LED_GPIO, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(RED_LED_GPIO);

    HAL_GPIO_SetMode(BLUE_LED_GPIO, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(BLUE_LED_GPIO);

    HAL_GPIO_SetPin(RED_LED_GPIO,  HAL_GPIO_PIN_RESET); //RED LED ON  
    HAL_GPIO_SetPin(BLUE_LED_GPIO, HAL_GPIO_PIN_SET);   //BLUE LED OFF
}

void BB_ledLock(void)
{
    HAL_GPIO_SetPin(BLUE_LED_GPIO, HAL_GPIO_PIN_RESET);     //BLUE LED ON
    HAL_GPIO_SetPin(RED_LED_GPIO, HAL_GPIO_PIN_SET);        //RED LED OFF
}

void BB_ledUnlock(void)
{
    HAL_GPIO_SetPin(BLUE_LED_GPIO, HAL_GPIO_PIN_SET );      //BLUE LED ON
    HAL_GPIO_SetPin(RED_LED_GPIO,  HAL_GPIO_PIN_RESET);     //RED LED OFF
}


void BB_EventHandler(void *p)
{
    STRU_SysEvent_DEV_BB_STATUS *pstru_status = (STRU_SysEvent_DEV_BB_STATUS *)p;

    if (pstru_status->pid == BB_LOCK_STATUS)
    {
        if (pstru_status->lockstatus == 3)
        {
            BB_ledLock();
        }
        else
        {
            BB_ledUnlock();
        }
    }
    else if(pstru_status->pid == BB_GOT_ERR_CONNNECT)
    {
        if (pstru_status->lockstatus)
        {
            DLOG_Info("Got mismatch signal");
        }
        else
        {
            DLOG_Info("not got signal");
        }
    }
}
