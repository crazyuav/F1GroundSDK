#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "hal_nvic.h"
#include "hal_bb.h"
#include "hal_uart.h"
#include "debuglog.h"
#include "systicks.h"
#include "hal.h"
#include "factory.h"
#include "usr_sbus_uart_task.h"

#if 0
#define Max_Rx_Uart 16

typedef struct UART_RX{

    uint32_t    rec_fifo_size;
    char        rec_fifo_buf[20000];
    uint32_t    rec_max_wr_counter;
    uint32_t    rec_buf_filled_len[100];
    uint32_t    rec_rd_index;
    uint32_t    rec_wr_index;

}UART_RX;

typedef struct UART_TX{

    uint32_t    rec_fifo_size;
    char        rec_fifo_buf[16000];
    uint32_t    rec_max_wr_counter;
    uint32_t    rec_buf_filled_len[1000];
    uint32_t    rec_rd_index;
    uint32_t    rec_wr_index;

}UART_TX;

UART_RX uart_rx3;
UART_TX uart_tx3;
int bb_dev_type;
extern osSemaphoreId bb_send_semaphore_id;
static int bypass_mode;
static int wir_rec = 0;
int uart_rec = 0;
int uart_send = 0;
static int wir_send = 0;
#define BYPASS_UART_NUM HAL_UART_COMPONENT_5

uint8_t get_fac_bypass_mode(void)
{
    STRU_RF_BAND_MCS_OPT *pst_bp_mode = NULL;
    uint8_t bp_mode = 0;
        
    pst_bp_mode = (STRU_RF_BAND_MCS_OPT *)FCT_GetNodeAndData(FACTORY_SUBNODE_BAND_MCS_ID,NULL);
    if(pst_bp_mode != NULL)
    {
        bp_mode = pst_bp_mode->bypass_ch_mode;
        if(bp_mode == 0)
        {
            bp_mode = 1;
        }
    }
    else
    {
        DLOG_Warning("get bp mode null");
    }

    DLOG_Warning("bypass mode %d",bp_mode);
    return bp_mode;
}
uint32_t nothing_todo(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    return 0;
}
uint32_t bypass_write_usb2uart(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    //ringbuf_memcpy_into(ring_hal_uart_rx,pu8_rxBuf,u8_len);
    uart_rec += u8_len;
    return u8_len;
}

uint32_t bypass_read_uart2usb(uint8_t *pu8_rxBuf,uint32_t max_len)
{
    /*
    uint32_t len;
    
    len = ringbuf_bytes_used(ring_bb_uart_rx);
    if(len == 0)
    {
        return 0;
    }

    if(len > max_len)
    {
        len = max_len;
    }

    ringbuf_memcpy_from(pu8_rxBuf,ring_bb_uart_rx,len);
    
    return len;*/
    return 0;
}

void Uart_Rx3_Init(void)
{
    memset(&uart_rx3,0,sizeof(uart_rx3));
    uart_rx3.rec_max_wr_counter = 100;
    uart_rx3.rec_fifo_size = 200;
}

void Uart_Tx3_Init(void)
{
    memset(&uart_tx3,0,sizeof(uart_tx3));
    uart_tx3.rec_max_wr_counter = 1000;
    uart_tx3.rec_fifo_size = Max_Rx_Uart;
}


void Rx3_Uart_Buffer_Push(char *buffer,uint32_t count)
{
    int index;

    if((uart_rx3.rec_rd_index < (uart_rx3.rec_wr_index + 10)) && (uart_rx3.rec_rd_index > uart_rx3.rec_wr_index))
    {
        DLOG_Critical("uart rec full\n");
            return ;
    }

    index = uart_rx3.rec_wr_index;

    memcpy((void *)(uart_rx3.rec_fifo_buf + index * uart_rx3.rec_fifo_size),buffer,count);
    uart_rx3.rec_buf_filled_len[index] = count;

    if (++uart_rx3.rec_wr_index == uart_rx3.rec_max_wr_counter)
        uart_rx3.rec_wr_index= 0;

}

void Tx3_Uart_Buffer_Push(char *buffer,uint32_t count)
{
    int index;

    if((uart_rx3.rec_rd_index < (uart_rx3.rec_wr_index + 10)) && (uart_rx3.rec_rd_index > uart_rx3.rec_wr_index))
    {
        DLOG_Critical("uart send full\n");
        return;
    }

    index = uart_tx3.rec_wr_index;

    memcpy((void *)(uart_tx3.rec_fifo_buf + index * uart_tx3.rec_fifo_size),buffer,count);
    uart_tx3.rec_buf_filled_len[index] = count;

    if (++uart_tx3.rec_wr_index == uart_tx3.rec_max_wr_counter)
        uart_tx3.rec_wr_index= 0;

}

uint32_t Uart_Rx3_Read(char *buffer,uint32_t count)
{
    uint32_t cnt = count;
    int index;

    while(cnt > 0)
    {

        if(uart_rx3.rec_rd_index == uart_rx3.rec_wr_index)
        	break;
        index = uart_rx3.rec_rd_index;

        if(cnt >= uart_rx3.rec_buf_filled_len[index])
        {
            memcpy(buffer,&uart_rx3.rec_fifo_buf[index*uart_rx3.rec_fifo_size],uart_rx3.rec_buf_filled_len[index]);
            buffer += uart_rx3.rec_buf_filled_len[index];
            cnt -= uart_rx3.rec_buf_filled_len[index];
            if (++uart_rx3.rec_rd_index == uart_rx3.rec_max_wr_counter)
                uart_rx3.rec_rd_index=0;
        }
        else
        {
            memcpy(buffer,(const void *)(&uart_rx3.rec_fifo_buf[index*uart_rx3.rec_fifo_size]),cnt);
            buffer += cnt;
            uart_rx3.rec_buf_filled_len[index] -= cnt;
            memcpy(&uart_rx3.rec_fifo_buf[index*uart_rx3.rec_fifo_size],&uart_rx3.rec_fifo_buf[index*uart_rx3.rec_fifo_size+cnt],uart_rx3.rec_buf_filled_len[index]);
            cnt = 0;
        }
    }

    return (count - cnt);
}

uint32_t Uart_Tx3_Read(char *buffer,uint32_t count)
{
    uint32_t cnt = count;
    int index;

    while(cnt > 0)
    {

        if(uart_tx3.rec_rd_index == uart_tx3.rec_wr_index)
        	break;
        index = uart_tx3.rec_rd_index;

        if(cnt >= uart_tx3.rec_buf_filled_len[index])
        {
            memcpy(buffer,&uart_tx3.rec_fifo_buf[index*uart_tx3.rec_fifo_size],uart_tx3.rec_buf_filled_len[index]);
            buffer += uart_tx3.rec_buf_filled_len[index];
            cnt -= uart_tx3.rec_buf_filled_len[index];
            if (++uart_tx3.rec_rd_index == uart_tx3.rec_max_wr_counter)
                uart_tx3.rec_rd_index=0;
        }
        else
        {
            memcpy(buffer,(const void *)(&uart_tx3.rec_fifo_buf[index*uart_tx3.rec_fifo_size]),cnt);
            buffer += cnt;
            uart_tx3.rec_buf_filled_len[index] -= cnt;
            memcpy(&uart_tx3.rec_fifo_buf[index*uart_tx3.rec_fifo_size],&uart_tx3.rec_fifo_buf[index*uart_tx3.rec_fifo_size+cnt],uart_tx3.rec_buf_filled_len[index]);
            cnt = 0;
        }
    }

    return (count - cnt);
}



static void Rcv3_Data_Handler(void *p)
{
    uint32_t cnt = 0;
    uint8_t i;
    uint8_t buffer[200];
    HAL_RET_T ret;

    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_3, buffer, sizeof(buffer),&cnt);
    if(ret != HAL_OK)
    {
        DLOG_Error("failed read bbcom");
        return;
    }
    if(cnt > 0)
    {
        wir_rec += cnt;
        Rx3_Uart_Buffer_Push(buffer,cnt);
    }
}


static uint32_t Uart3_Irq_Handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    char u8_cnt = u8_len;
    uint32_t pbuf;
    pbuf = (uint32_t)pu8_rxBuf;

    if(u8_cnt > 0)
    {
	uart_rec += u8_cnt;
        if(u8_cnt >= Max_Rx_Uart)
        {
            Tx3_Uart_Buffer_Push((uint8_t *)pbuf,Max_Rx_Uart);
            pbuf += Max_Rx_Uart;
            u8_cnt -= Max_Rx_Uart;
        }
        else
        {
            Tx3_Uart_Buffer_Push((uint8_t *)pbuf,u8_cnt);
            u8_cnt = 0;
        }

    }
}

static void Uart3_Init(void)
{
    STRU_UART_BAUDR *pst_uart_baudr = NULL;
    ENUM_HAL_UART_BAUDR enum_baud = HAL_UART_BAUDR_115200;
    
    pst_uart_baudr = (STRU_UART_BAUDR *)FCT_GetNodeAndData(FACTORY_SUBNODE_UART_BAUDR_ID,NULL);
    if(pst_uart_baudr != NULL)
    {
        if(pst_uart_baudr->st_uartBaudr[1] <= HAL_UART_BAUDR_460800)
        {
            enum_baud = (ENUM_HAL_UART_BAUDR)pst_uart_baudr->st_uartBaudr[1];
            DLOG_Warning("bypass uart5 baud = %d",enum_baud);
        }
        else
        {
            DLOG_Warning("bypass uart5 baud %d ,force to  = %d",pst_uart_baudr->st_uartBaudr[1],enum_baud);
        }
    }
    else
    {
        DLOG_Warning("get pst_uart_baudr null");
    }
    
    if(bb_dev_type)
    {
        if(bypass_mode == 1 || bypass_mode == 2 || bypass_mode == 4)
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,Uart3_Irq_Handler);
        }
        else
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,nothing_todo);
        }
    }
    else
    {
        if(bypass_mode == 1 || bypass_mode == 3 || bypass_mode == 5)
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,Uart3_Irq_Handler);
        }
        else
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,nothing_todo);
        }

    }

    //NVIC_SetPriority(HAL_NVIC_UART_INTR5_VECTOR_NUM, NVIC_EncodePriority(NVIC_PRIORITYGROUP_5,INTR_NVIC_PRIORITY_UART_DEFAULT,0));
    //HAL_NVIC_SetPriority(HAL_NVIC_UART_INTR5_VECTOR_NUM, 6, 0);//low rt-uart priority
}


static void COMTASK_Tx3Function(void const *argument)
{

    char buffer[200];
    char i;
    uint32_t ret;
    uint32_t r_cnt;

    HAL_Delay(20);
    while(1)
    {

        r_cnt = Uart_Rx3_Read(buffer,128);
        if(r_cnt > 0 && r_cnt <= 128)
        {  
		    uart_send += r_cnt;
            ret = HAL_UART_TxData(BYPASS_UART_NUM, buffer, r_cnt, HAL_UART_DEFAULT_TIMEOUTMS * 1000);
            if(ret != HAL_OK)
            {
                DLOG_Error("--->tx failed ret %d",ret);
            }
        }
        else if(r_cnt == 0)
        {
            HAL_Delay(5);
        }
        else
        {
            DLOG_Error("> 128");
            HAL_Delay(10);
        }
        
    }

}


static void COMTASK_Rx3Function(void const *argument)
{
    int ret;
    char buffer[200];
    HAL_RET_T ret_t;
    int len;

    if(bb_dev_type)
    {
        len = 70 - get_fac_spi_num() ;//gnd
        DLOG_Warning("uplink pack len %d",len);
    }
    else
    {
        len = 200;
    }

    HAL_Delay(20);
    while (1)
    {
        ret = Uart_Tx3_Read(buffer,len);
        if(ret > 0)
        {
            wir_send += ret;

            osSemaphoreWait(bb_send_semaphore_id,0);
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_3,buffer,len);
            osSemaphoreRelease(bb_send_semaphore_id);
            if(ret != HAL_OK)
            {
                DLOG_Error("failed ret %d",ret);
            }


            /*
            while(HAL_BB_ComSendMsg(BB_COM_SESSION_3,buffer,ret) != 0 )
                HAL_Delay(10);*/

            if(ret < len)
                HAL_Delay(4);
        }
        else
        {
            HAL_Delay(8);
        }
    }
}

static void COMTASK_DebugFunction(void const *argument)
{
    while (1)
    {
        HAL_Delay(2000);
        DLOG_Warning("------uart_send = %d-----uart_rec = %d-----\n",uart_send,uart_rec);
        DLOG_Warning("------wir_send = %d-----wir_rec = %d-----\n",wir_send,wir_rec);
    }
}

void usr_bypass_uart_task(int dev_type)
{
    bb_dev_type = dev_type;
    bypass_mode = get_fac_bypass_mode();

    Uart_Rx3_Init();
    Uart_Tx3_Init();

    osThreadDef(COMTASK_Tx1, COMTASK_Tx3Function, osPriorityHigh, 0, 8 * 128);
    osThreadCreate(osThread(COMTASK_Tx1), NULL);

    osThreadDef(COMTASK_Rx1, COMTASK_Rx3Function, osPriorityHigh, 0, 8 * 128);
    osThreadCreate(osThread(COMTASK_Rx1), NULL);

    osThreadDef(COMTASK_Debug, COMTASK_DebugFunction, osPriorityIdle, 0, 8 * 128);
    osThreadCreate(osThread(COMTASK_Debug), NULL);

    HAL_BB_ComRegisterSession(BB_COM_SESSION_3,
                              BB_COM_SESSION_PRIORITY_LOW,
                              BB_COM_SESSION_DATA_NORMAL,
                              Rcv3_Data_Handler);

    Uart3_Init();

}

#else
#include "ringbuf.h"

#define BYPASS_UART_NUM HAL_UART_COMPONENT_1
#define BYPASS_BB_COM BB_COM_SESSION_3
static ringbuf_t ring_hal_uart_rx = NULL;
static ringbuf_t ring_bb_uart_rx = NULL;
static int hal_uart_rx_cnt,bb_uart_rx_cnt;
static int bb_dev_type;
extern osSemaphoreId bb_send_semaphore_id;
static int bypass_mode;

uint32_t bypass_write_usb2uart(uint8_t *pu8_rxBuf, uint16_t u16_len)
{
    ringbuf_memcpy_into(ring_hal_uart_rx,pu8_rxBuf,u16_len);
    hal_uart_rx_cnt += u16_len;
}

uint32_t bypass_read_uart2usb(uint8_t *pu8_rxBuf,uint32_t max_len)
{
    uint32_t len;
    
    len = ringbuf_bytes_used(ring_bb_uart_rx);
    if(len == 0)
    {
        return 0;
    }

    if(len > max_len)
    {
        len = max_len;
    }

    ringbuf_memcpy_from(pu8_rxBuf,ring_bb_uart_rx,len);
    
    return len;
}


uint8_t get_fac_bypass_mode(void)
{
    STRU_RF_BAND_MCS_OPT *pst_bp_mode = NULL;
    uint8_t bp_mode = 0;
        
    pst_bp_mode = (STRU_RF_BAND_MCS_OPT *)FCT_GetNodeAndData(FACTORY_SUBNODE_BAND_MCS_ID,NULL);
    if(pst_bp_mode != NULL)
    {
        bp_mode = pst_bp_mode->bypass_ch_mode;
        if(bp_mode == 0)
        {
            bp_mode = 1;
        }
    }
    else
    {
        DLOG_Warning("get bp mode null");
    }

    DLOG_Warning("bypass mode %d",bp_mode);
    return bp_mode;
}

ENUM_HAL_UART_BAUDR get_fac_uart5_baudrate(void)
{
    STRU_UART_BAUDR *pst_uart_baudr = NULL;
    ENUM_HAL_UART_BAUDR enum_baud = HAL_UART_BAUDR_115200;
    
    pst_uart_baudr = (STRU_UART_BAUDR *)FCT_GetNodeAndData(FACTORY_SUBNODE_UART_BAUDR_ID,NULL);
    if(pst_uart_baudr != NULL)
    {
        if(pst_uart_baudr->st_uartBaudr[1] <= HAL_UART_BAUDR_460800)
        {
            enum_baud = (ENUM_HAL_UART_BAUDR)pst_uart_baudr->st_uartBaudr[1];
            DLOG_Warning("bypass uart5 baud = %d",enum_baud);
        }
        else
        {
            DLOG_Warning("bypass uart5 baud %d ,force to  = %d",pst_uart_baudr->st_uartBaudr[1],enum_baud);
        }
    }
    else
    {
        DLOG_Warning("get pst_uart_baudr null");
    }
    return enum_baud;

}
uint32_t nothing_todo(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    return 0;
}

static uint32_t bypass_hal_uart_irq_handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    ringbuf_memcpy_into(ring_hal_uart_rx,pu8_rxBuf,u8_len);
    hal_uart_rx_cnt += u8_len;
}

static void bypass_bb_uart_irq_handler(void *p)
{
    #define BB_READ_MAX_LEN 200
    uint32_t cnt;
    uint8_t buffer[BB_READ_MAX_LEN];
    HAL_RET_T ret;

    ret = HAL_BB_ComReceiveMsg(BYPASS_BB_COM, buffer, BB_READ_MAX_LEN,&cnt);
    if(ret != HAL_OK)
    {
        DLOG_Error("failed read bbcom");
        return;
    }

    if(cnt > 0 && cnt <= BB_READ_MAX_LEN)
    {
        ringbuf_memcpy_into(ring_bb_uart_rx,buffer,cnt);
        bb_uart_rx_cnt += cnt;
    }
    else
    {
        DLOG_Info("bb read %d",cnt);
    }
    
}

static void hal_uart_tx2bb_task(void const *argument)
{
    #define MAX_BB_UPLINK_SEND_SIZE 70
    #define MAX_BB_DOWNLINK_SEND_SIZE 200
    #define MAX(a,b) (((a) > (b)) ?  (a) :  (b) )
    HAL_RET_T ret;
    char buffer[MAX(MAX_BB_UPLINK_SEND_SIZE,MAX_BB_DOWNLINK_SEND_SIZE)];
    int len;
    int max_send_len;
    
    if(bb_dev_type)
    {
        max_send_len = MAX_BB_UPLINK_SEND_SIZE - get_fac_spi_num();
        DLOG_Warning("uplink pack len %d",max_send_len);

    }
    else
    {
        max_send_len = MAX_BB_DOWNLINK_SEND_SIZE;
    }

    while (1)
    {
        len = ringbuf_bytes_used(ring_hal_uart_rx);
        if(len == 0)
        {
            HAL_Delay(8);
            continue;
        }

        if(len > 1024)
        {
            DLOG_Error("len error %d,%08x,%08x",len,ringbuf_head(ring_hal_uart_rx),ringbuf_tail(ring_hal_uart_rx));
        }
         
        if(len > max_send_len)
        {
            len = max_send_len;
        }
        
        ringbuf_memcpy_from(buffer,ring_hal_uart_rx,len);
        osSemaphoreWait(bb_send_semaphore_id,0);
        ret = HAL_BB_ComSendMsg(BYPASS_BB_COM,buffer,len);
        osSemaphoreRelease(bb_send_semaphore_id);
        if(ret != HAL_OK)
        {
            DLOG_Error("failed ret %d",ret);
        }
        
        HAL_Delay(4);
    }
}

static void bb_uart_tx2hal_task(void const *argument)
{
    HAL_RET_T ret;
    char buffer[1024];
    int len;

    while (1)
    {
        len = ringbuf_bytes_used(ring_bb_uart_rx);
        if(len == 0)
        {
            HAL_Delay(14);
            continue;
        }

        if(len > 1024)
        {
            DLOG_Error("len error %d,%08x,%08x",len,ringbuf_head(ring_bb_uart_rx),ringbuf_tail(ring_bb_uart_rx));
            len = 1024;
        }
        
        ringbuf_memcpy_from(buffer,ring_bb_uart_rx,len);
        ret = HAL_UART_TxData(BYPASS_UART_NUM, buffer, len, HAL_UART_DEFAULT_TIMEOUTMS * 1000);
        if(ret != HAL_OK)
        {
            DLOG_Error("failed ret %d",ret);
        }
        
        HAL_Delay(4);
    }
}

static void bypass_uart_log_task(void const *argument)
{
    while (1)
    {
        DLOG_Warning("------> hal_uart_rx = %d",hal_uart_rx_cnt);
        DLOG_Warning("------> bb_uart_rx = %d",bb_uart_rx_cnt);
        HAL_Delay(5000);
    }
}

void usr_bypass_uart_task(int dev_type)
{
    bb_dev_type = dev_type;
    bypass_mode = get_fac_bypass_mode();
    ENUM_HAL_UART_BAUDR enum_baud =  get_fac_uart5_baudrate();
    
    ring_hal_uart_rx = ringbuf_new(0);
    if(!ring_hal_uart_rx)
    {
        DLOG_Critical("malloced failed");
    }
    
    ring_bb_uart_rx = ringbuf_new(1);
    if(!ring_bb_uart_rx)
    {
        DLOG_Critical("malloced failed");
    }

    if(dev_type)
    {
        if(bypass_mode == 1 || bypass_mode == 2 || bypass_mode == 4)
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,bypass_hal_uart_irq_handler);
        }
        else
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,nothing_todo);
        }
    }
    else
    {
        if(bypass_mode == 1 || bypass_mode == 3 || bypass_mode == 5)
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,bypass_hal_uart_irq_handler);
        }
        else
        {
            HAL_UART_Init(BYPASS_UART_NUM,enum_baud,nothing_todo);
        }

    }

    HAL_BB_ComRegisterSession(BYPASS_BB_COM,
                              BB_COM_SESSION_PRIORITY_LOW,
                              BB_COM_SESSION_DATA_NORMAL,
                              bypass_bb_uart_irq_handler);
    
    osThreadDef(HAL_UART_TX2BB_TASK, hal_uart_tx2bb_task, osPriorityHigh, 0, 8 * 128);
    osThreadCreate(osThread(HAL_UART_TX2BB_TASK), NULL);
    
    if(dev_type)
    {
        if(bypass_mode == 1 || bypass_mode == 2 || bypass_mode == 4)
        {
            osThreadDef(BB_UART_TX2HAL_TASK, bb_uart_tx2hal_task, osPriorityHigh, 0, 8 * 128);
            osThreadCreate(osThread(BB_UART_TX2HAL_TASK), NULL);
        }
    }
    else
    {
        if(bypass_mode == 1 || bypass_mode == 3 || bypass_mode == 5)
        {
            osThreadDef(BB_UART_TX2HAL_TASK, bb_uart_tx2hal_task, osPriorityHigh, 0, 8 * 128);
            osThreadCreate(osThread(BB_UART_TX2HAL_TASK), NULL);

        }
    }

    osThreadDef(BYPASS_UART_LOG_TASK, bypass_uart_log_task, osPriorityIdle, 0, 8 * 128);
    osThreadCreate(osThread(BYPASS_UART_LOG_TASK), NULL);

}

#endif
