#ifndef __DEBUG_TOOL_EXAMPLE_H__
#define __DEBUG_TOOL_EXAMPLE_H__

#include <stdint.h>
#include "wireless_interface.h"


#define DEBUG_USER_HANDLER_NUM          4
#define DEBUG_USER_HANDLER_TEST_01      (WIRELESS_INTERFACE_USER_RESERVED)
#define DEBUG_USER_HANDLER_TEST_02      (WIRELESS_INTERFACE_USER_RESERVED + 1)
#define DEBUG_USER_HANDLER_TEST_03      (WIRELESS_INTERFACE_USER_RESERVED + 2)
#define DEBUG_USER_HANDLER_TEST_04      (WIRELESS_INTERFACE_USER_RESERVED + 3)

uint8_t DEBUG_USER_Test01Handler(void *param, uint8_t id);
uint8_t DEBUG_USER_Test02Handler(void *param, uint8_t id);
uint8_t DEBUG_USER_Test03Handler(void *param, uint8_t id);
uint8_t DEBUG_USER_Test04Handler(void *param, uint8_t id);
void DEBUG_USER_Init(void);


#endif

