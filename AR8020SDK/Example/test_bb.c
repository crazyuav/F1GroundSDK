#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debuglog.h"
#include "bb_types.h"
#include "hal_bb.h"
#include "hal_gpio.h"
#include "memory_config.h"
#include "debuglog.h"
#include "hal_uart.h"
#include "serial.h"


static void rcvDataHandler(void *p)
{
    uint8_t data_buf_proc[1024];
    uint32_t u32_rcvLen = 0;
    
    HAL_BB_ComReceiveMsg(BB_COM_SESSION_2, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);

    HAL_UART_TxData(HAL_UART_COMPONENT_3, (uint8_t *)&data_buf_proc, u32_rcvLen, 2 * 10);

    // uint8_t flag;
    // for (size_t i = 0; i < u32_rcvLen; i++)
    // {
    //     flag = data_buf_proc[i];
    //     _ar_early_uart_putc(flag, UART3_BASE);
        
    // }
    

    DLOG_Critical("rcv: %d", u32_rcvLen);
    DLOG_Critical("%d %d", data_buf_proc[0], data_buf_proc[1]);

    DLOG_Output(1000);
}


void command_test_BB_uart(char *index_str)
{
    unsigned char opt = strtoul(index_str, NULL, 0);
    uint8_t data_buf[512] ;
    uint32_t i = 0;

    if (opt == 0)
    {
        HAL_BB_ComRegisterSession(BB_COM_SESSION_2,
                                  BB_COM_SESSION_PRIORITY_HIGH,
                                  BB_COM_SESSION_DATA_NORMAL,
                                  rcvDataHandler);
    }
    else if (opt == 1)
    {
        for(i = 0; i < 22; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_2, data_buf, 22);
    }
    else if (opt == 2)
    {
        for(i = 0; i < 128; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_2, data_buf, 128);
    }
    else if (opt == 3)
    {
        for(i = 0; i < 512; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_2, data_buf, 512);
    }
}

static void rcvSPIDataHandler(void *p)
{
#if 0
    uint8_t data_buf_proc[1024];
    uint32_t u32_rcvLen = 0;
    
    HAL_BB_ComReceiveMsg(BB_COM_SESSION_3, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);

    DLOG_Info("rcv spi com: %d", u32_rcvLen);
    DLOG_Info("%d %d %d %d %d %d", data_buf_proc[0],
                                   data_buf_proc[1],
                                   data_buf_proc[2],
                                   data_buf_proc[3],
                                   data_buf_proc[4],
                                   data_buf_proc[5]);
#endif
}


void command_test_BB_spi(char *index_str)
{
#if 0
    unsigned char opt = strtoul(index_str, NULL, 0);
    uint8_t data_buf[512];
    uint32_t i = 0;

    if (opt == 0)
    {
        HAL_BB_ComRegisterSession(BB_COM_SESSION_3,
                                  BB_COM_SESSION_PRIORITY_HIGH,
                                  BB_COM_SESSION_DATA_NORMAL,
                                  rcvSPIDataHandler);
    }
    else if (opt == 1)
    {
        for(i = 0; i < 50; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_3, data_buf, 50);
    }
    else if (opt == 2)
    {
        for(i = 0; i < 128; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_3, data_buf, 128);
    }
    else if (opt == 3)
    {
        for(i = 0; i < 512; i++)
        {
            data_buf[i] = i;
        }

        HAL_BB_ComSendMsg(BB_COM_SESSION_3, data_buf, 512);
    }
#endif
}


void command_BB_add_cmds(char *cmdstr0, char *cmdstr1, char *cmdstr2, char *cmdstr3, char *cmdstr4)
{
    extern int BB_add_cmds(uint8_t type, uint32_t param0, uint32_t param1, 
                                         uint32_t param2, uint32_t param3);

    BB_add_cmds(strtoul(cmdstr0, NULL, 0),  //type
                strtoul(cmdstr1, NULL, 0),  //param0
                strtoul(cmdstr2, NULL, 0),  //param1
                strtoul(cmdstr3, NULL, 0),  //param2
                strtoul(cmdstr4, NULL, 0)   //param3
                );

}

void command_BB_GroundconnectToskyByRcId(char *rcid0, char *rcid1, char *rcid2, char *rcid3, char *rcid4, char *vtid0, char *vtid1)
{
    uint8_t rcid[5] = { strtoul(rcid0, NULL, 0),
                        strtoul(rcid1, NULL, 0),
                        strtoul(rcid2, NULL, 0),
                        strtoul(rcid3, NULL, 0),
                        strtoul(rcid4, NULL, 0),
                        };

    uint8_t vtid[2] = {
                        strtoul(vtid0, NULL, 0),
                        strtoul(vtid1, NULL, 0),
                        };    
    HAL_BB_GroundConnectToSkyByRcId(rcid, vtid);
}

void command_searchRcId(char *following)
{
    HAL_BB_SearchRcId(strtoul(following, NULL, 0));
}


void command_setTargetPower(char *u8_2gpower,char *u8_5gpower)
{
    HAL_BB_SetPower(strtoul(u8_2gpower, NULL, 0),strtoul(u8_5gpower, NULL, 0));
}

void command_setTargetStardard(char *u8_stardard_enum)
{
    HAL_BB_SetPowerWorkMode((ENUM_RF_POWER_WORK_MODE)strtoul(u8_stardard_enum, NULL, 0));
}

void command_SetBbFilter(char *filter)
{
    extern int BB_SetFilterWorkMode(uint8_t value);

    BB_SetFilterWorkMode(strtoul(filter, NULL, 0));
}

void command_setSubBBch(char *ch)
{
    HAL_BB_SubBandSetCH(strtoul(ch, NULL, 0));
}

