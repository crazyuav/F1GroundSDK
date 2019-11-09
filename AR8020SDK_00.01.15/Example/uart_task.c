#include <stdint.h>
#include "cmsis_os.h"
#include "hal_nvic.h"
#include "hal_bb.h"
#include "hal_uart.h"
#include "debuglog.h"
#include "systicks.h"
#include "hal.h"


static osMessageQId com_queue0_id = 0;
static uint8_t tx_buf0_index = 0;
static uint8_t tx_buf0[1024];
static uint8_t rx_buf0[1024];


static volatile uint8_t tx_last_byte_timeout0 = 0;
static osMutexId tx_buf_mutex0_id = 0;


uint8_t old_data;

uint32_t receive_size = 0;


static void rcvDataHandler(void *p)
{
	uint8_t cnt = 0;
	uint8_t i;

	cnt = BB_ComReceiveMsg(BB_COM_SESSION_1, rx_buf0, 23 * 30);
	if ( cnt % 23 != 0)
	{
		DLOG_Critical("-get size = %d", cnt);
	}
	receive_size += cnt;
	if(cnt >= 23)
	{
		if (rx_buf0[0] == 0 && old_data == 0xff)
		{
		}
		else if((rx_buf0[0] - old_data) != 1)
		{
			DLOG_Critical("----drop------buf0 = %d---old_data = %d %d %d",rx_buf0[0],old_data, rx_buf0[1], rx_buf0[2]);
		}

		old_data = rx_buf0[0];
		int j;
		for(j = 0; j < cnt / 23 -1; j++)
		{
			if (rx_buf0[j*23] == 0xff && rx_buf0[ (j+1)*23] == 0x00)
			{
			}
			else if (rx_buf0[j*23] + 1 != rx_buf0[ (j+1)*23])
			{
				DLOG_Critical("----drop 1------buf0 = %d---old_data = %d %d %d",rx_buf0[0], old_data, rx_buf0[1], rx_buf0[2]);
			}
			old_data = rx_buf0[(j+1)*23];
		}
	}
}



static uint32_t Uart3IrqHandler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    uint8_t u8_i;
	if(u8_len > 0)
	{
		DLOG_Critical("----uart3_len = %d-------\n",u8_len);
		u8_len = 23;
		HAL_BB_ComSendMsg(BB_COM_SESSION_1,pu8_rxBuf,u8_len);
	}
}

static void Uart3_Init(void)
{
	HAL_UART_Init(HAL_UART_COMPONENT_3,HAL_UART_BAUDR_115200,Uart3IrqHandler);
}


int send_cnt = 0;
static void COMTASK_Tx0Function(void const *argument)
{
	volatile char buffer[512];
	char i;

	for(i = 0;i < 64;i ++)
		buffer[i] = i + 1;
 
    while(1)
    {        
        if (HAL_BB_ComSendMsg(BB_COM_SESSION_1, (uint8_t *)buffer, 23) != 0 )
                HAL_Delay(100);
        else
        {
            buffer[0] = buffer[0] + 1;
            send_cnt += 23;
        }
    }
}



void Usr_Uart_Task(int send)
{
    int ret = 0;
	if ( (ret =HAL_BB_ComRegisterSession(BB_COM_SESSION_1,
									  BB_COM_SESSION_PRIORITY_HIGH,
									  BB_COM_SESSION_DATA_NORMAL,
									  rcvDataHandler)) != 0 )
    {
        DLOG_Error("error = %d \r\n", ret);
    }

    if (send)
    {
        osThreadDef(COMTASK_Tx0, COMTASK_Tx0Function, osPriorityIdle, 0, 4 * 128);
        osThreadCreate(osThread(COMTASK_Tx0), NULL);
    }

	DLOG_Critical("---------------test uart task---------------\n");
}


