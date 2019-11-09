#include "wireless_interface.h"
#include "debug_tool_example.h"
#include "debuglog.h"

STRU_WIRELESS_USER_HANDLER   g_stDebugUserHandler[DEBUG_USER_HANDLER_NUM] = {
        {DEBUG_USER_HANDLER_TEST_01, DEBUG_USER_Test01Handler},
        {DEBUG_USER_HANDLER_TEST_02, DEBUG_USER_Test02Handler},
        {DEBUG_USER_HANDLER_TEST_03, DEBUG_USER_Test03Handler},
        {DEBUG_USER_HANDLER_TEST_04, DEBUG_USER_Test04Handler},
};


uint8_t DEBUG_USER_Test01Handler(void *param, uint8_t id)
{
    DLOG_Error("Test 01");

    return 0;
}


uint8_t DEBUG_USER_Test02Handler(void *param, uint8_t id)
{
    DLOG_Error("Test 02");

    return 0;
}

uint8_t DEBUG_USER_Test03Handler(void *param, uint8_t id)
{
    DLOG_Error("Test 03");

    return 0;
}

uint8_t DEBUG_USER_Test04Handler(void *param, uint8_t id)
{
    DLOG_Error("Test 04");

    return 0;
}

void DEBUG_USER_Init(void)
{
    WIRELESS_INTERFACE_RegisterUserHandler(g_stDebugUserHandler, DEBUG_USER_HANDLER_NUM);
}




