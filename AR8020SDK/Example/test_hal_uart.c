#include <stdlib.h>
#include "test_hal_uart.h"
#include "hal_nvic.h"
#include "debuglog.h"
#include "hal.h"
#include "cmsis_os.h"
#include "systicks.h"
#include "auto_test.h"
#include "memory.h"


uint8_t s_u8_uartRxBuf[64];
uint8_t s_u8_uartRxLen = 0;

static uint32_t uartRxCallBack(uint8_t *pu8_rxBuf, uint8_t u8_len);

void command_TestHalUartIntSet(unsigned char *ch, unsigned char *flag)
{
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_flag = strtoul(flag, NULL, 0);

    if(0 == u32_flag)
    {
       HAL_NVIC_DisableIrq(u32_ch + HAL_NVIC_UART_INTR0_VECTOR_NUM);
    }
    else
    {
        HAL_NVIC_EnableIrq(u32_ch + HAL_NVIC_UART_INTR0_VECTOR_NUM);
    }
}

void command_TestHalUartInit(unsigned char *ch, unsigned char *br)
{
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_br = strtoul(br, NULL, 0);

    memset(s_u8_uartRxBuf, sizeof(s_u8_uartRxBuf), 0x55);

    HAL_UART_Init((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                  (ENUM_HAL_UART_BAUDR)(u32_br), 
                  uartRxCallBack);
}
void command_TestHalSbusInit(unsigned char *ch, unsigned char *br)
{
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_br = strtoul(br, NULL, 0);

    memset(s_u8_uartRxBuf, sizeof(s_u8_uartRxBuf), 0x55);

    HAL_SBUS_Init((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                  u32_br, 
                  uartRxCallBack);
}


void command_TestHalUartTx(unsigned char *ch, unsigned char *len)
{
    uint8_t result = 1;
    uint64_t t_diff[1] = {0};

    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, ch, len);

    uint16_t u16_i;
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_len = strtoul(len, NULL, 0);
    uint8_t *u8_data = malloc_safe(u32_len);

    if (NULL == u8_data)
    {
        DLOG_Error("malloc error");
        result = 0;
    }

    for(u16_i = 0; u16_i < (uint16_t)u32_len; u16_i++)
    {
        u8_data[u16_i] = u16_i;
    }

    AUTO_TEST_TimeRecord();
    HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                    u8_data, 
                    u16_i,
                    HAL_UART_DEFAULT_TIMEOUTMS);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    AUTO_TEST_TimeRecord();
    free_safe(u8_data);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_UART_TxData", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("free", (uint32_t)t_diff[1]);
    DIV_LINE(DIV_LINE_END);
}

void command_TestHalSbusTx(unsigned char *ch, unsigned char *len)
{
    uint8_t result = 1;
    uint64_t t_diff[1] = {0};

    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, ch, len);

    uint16_t u16_i;
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_len = strtoul(len, NULL, 0);
    uint8_t *u8_data = malloc_safe(u32_len);

    if (NULL == u8_data)
    {
        DLOG_Error("malloc error");
        result = 0;
    }

    for(u16_i = 0; u16_i < (uint16_t)u32_len; u16_i++)
    {
        u8_data[u16_i] = u16_i;
    }

    AUTO_TEST_TimeRecord();
    HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                    u8_data, 
                    u16_i,
                    HAL_UART_DEFAULT_TIMEOUTMS);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    AUTO_TEST_TimeRecord();
    free_safe(u8_data);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_UART_TxData", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("free", (uint32_t)t_diff[1]);
    DIV_LINE(DIV_LINE_END);
}

void command_TestHalUartAsynTx(unsigned char *ch, unsigned char *len)
{
    uint8_t result = 1;
    uint64_t t_diff[1] = {0};

    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, ch, len);

    uint16_t u16_i;
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_len = strtoul(len, NULL, 0);
    uint8_t *u8_data = malloc_safe(u32_len);
    uint8_t *u8_data2 = malloc_safe(u32_len);

    if ((NULL == u8_data) || (NULL == u8_data2))
    {
        DLOG_Error("malloc error");
        result = 0;
    }

    for(u16_i = 0; u16_i < (uint16_t)u32_len; u16_i++)
    {
        u8_data[u16_i] = u16_i;
    }

    DLOG_Info("send the first data.");
    AUTO_TEST_TimeRecord();
    HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                    u8_data, 
                    u32_len,
                    0);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    DLOG_Info("send the second data.");
    memset(u8_data2, 0x55, u32_len);
    HAL_RET_T ret = HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                    u8_data2, 
                    u32_len,
                    0);
    if(HAL_BUSY == ret)
    {
        DLOG_Info("***uart %d is busy.", u32_ch);
    }
    else
    {
        result = 0;
    }
    
    AUTO_TEST_TimeRecord();
    free_safe(u8_data);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    free_safe(u8_data2);

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_UART_TxData", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("free", (uint32_t)t_diff[1]);
    DIV_LINE(DIV_LINE_END);
}


void command_TestHalUartRx(unsigned char *ch)
{
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    uint16_t u16_i;
    
    HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                     s_u8_uartRxBuf, 
                     s_u8_uartRxLen,
                     HAL_UART_DEFAULT_TIMEOUTMS);

    s_u8_uartRxLen = 0;
}
void command_TestHalSbusRx(unsigned char *ch)
{
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    uint16_t u16_i;
    
    HAL_UART_TxData((ENUM_HAL_UART_COMPONENT)(u32_ch), 
                     s_u8_uartRxBuf, 
                     s_u8_uartRxLen,
                     HAL_UART_DEFAULT_TIMEOUTMS);

    s_u8_uartRxLen = 0;
}


static uint32_t uartRxCallBack(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    if (u8_len > 64)
    {
        u8_len = 64;
    }
    
    memcpy(s_u8_uartRxBuf + s_u8_uartRxLen, pu8_rxBuf, u8_len);

    s_u8_uartRxLen += u8_len;
}

static void uart_send_hander( void const * argument)
{
    static uint8_t flag_inited = 0;
    uint32_t len;
    uint32_t i;

    uint8_t* u8_data = NULL;

    if (flag_inited == 0)
    {
        HAL_UART_Init(1, 4, uartRxCallBack);
    }
 
    while(1)
    {
        for (len = 1; len < 4096; len++)
        {
            u8_data = malloc_safe(len);
            if (NULL == u8_data)
            {
                DLOG_Error("malloc fail");
            }
            else
            {
                for (i = 0;i < len; i++)
                {
                    u8_data[i] = (uint8_t)i;
                }
            }

            HAL_UART_TxData(1, 
                            u8_data, 
                            len,
                            HAL_UART_DEFAULT_TIMEOUTMS);

            free_safe(u8_data);
            HAL_Delay(10);
			if (len % 100 ==  0)
			{
				DLOG_Info("send len = %d", len);
			}
        }
        HAL_Delay(1);
    }
}

void test_uart_with_os()
{
	osThreadDef(UART_SEND, uart_send_hander, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(UART_SEND), NULL);
}
