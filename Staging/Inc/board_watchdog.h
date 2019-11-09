#ifndef __BOARD_WATCHDOG_STM6321_H__
#define __BOARD_WATCHDOG_STM6321_H__
#include "hal_gpio.h"

void WATCHDOG_Init(void);
void WATCHDOG_InitUpgrade(void);
void WATCHDOG_Reset(void);
void WATCHDOG_InitCpu2(void);
void WATCHDOGUPGRADE_Reset(void);
uint32_t WATCHDOG_InitStatus(void);

#endif
