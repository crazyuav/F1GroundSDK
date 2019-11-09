#include <stdlib.h>
#include "debuglog.h"
#include "hal.h"
#include "sleep_mode.h"
#include "hal_bb.h"
#include "hal_gpio.h"

static uint8_t FemPin[] = {92, 96};

static void FemOn(uint8_t ch);
static void FemOff(uint8_t ch);
static void EnterSleep(uint8_t level);


static void FemOn(uint8_t ch)
{
    if(ch > 1)
    {
        return;
    }
    
    HAL_GPIO_SetMode(FemPin[ch], HAL_GPIO_PIN_MODE2);    
    HAL_GPIO_OutPut(FemPin[ch]);
    HAL_GPIO_SetPin(FemPin[ch],  HAL_GPIO_PIN_SET);
}

static void FemOff(uint8_t ch)
{
    if(ch > 1)
    {
        return;
    }
    
    HAL_GPIO_SetMode(FemPin[ch], HAL_GPIO_PIN_MODE2);    
    HAL_GPIO_OutPut(FemPin[ch]);
    HAL_GPIO_SetPin(FemPin[ch],  HAL_GPIO_PIN_RESET);
}

static void EnterSleep(uint8_t level)
{
    HAL_PwrCtrlSet(level);
    
    if(0 == level)
    {
        FemOn(0);
        FemOn(1);
    }
    if(1 == level)
    {
        FemOn(0);
        FemOff(1);
    }
    else if((2 == level) || (3 == level))
    {
        FemOff(0);
        FemOff(1);
    }
}
    
void command_EnterSleep(uint8_t *level)
{
    uint8_t tmpLevel = strtoul(level, NULL, 0);

    EnterSleep(tmpLevel);
    DLOG_Warning("sleep level:%x", HAL_GetPwrCtrlLevel());
}

void command_ExitSleep(void)
{
    EnterSleep(0);
    DLOG_Warning("exit sleep");
}


void SleepModeProcess(void *p)
{
    uint8_t *value = (uint8_t *)p;

    EnterSleep(value[1]);
}
