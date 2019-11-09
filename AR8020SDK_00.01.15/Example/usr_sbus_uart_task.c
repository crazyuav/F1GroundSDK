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
#include "ringbuf.h"
#include "hal_gpio.h"
#include "interrupt.h"
#include "ar8020.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "memory_config.h"
#include "factory.h"
#include "board_watchdog.h"

/*
                rt-uart function introduce

            ground device rt-uart work status introduce:
    
    1.init gpio,enable BaseBand Tx Irq, register a timer;
    2.when receive BaseBand Tx Irq, set gpio to high level to notice extern device send data to me,
    and start the timer;
    3.when timer overflow, set gpio to low level, and write receive data to BaseBand, clear Tx irq Flag
    ,stop the timer;
    4.continue to wait BaseBand Tx Irq, and then goto step 2

            sky device rt-uart work status introduce:

    1.enable BaseBand Rx Irq, register a timer;
    2.when receive BaseBand Rx Irq, start the timer to delay when to read BaseBand data;
    3.when timer overflow, read BaseBand data and Output data from Hal uart, clear RX irq Flag
    ,stop the timer;
    4.continue to wait BaseBand Rx Irq, and then goto step 2

*/
/* define rt uart Hal uart num */
#define BYPASS_UART_SPI_NUM HAL_UART_COMPONENT_4
/* define rt uart BaseBand session num */
#define BYPASS_BB_COM BB_COM_SESSION_SPI
/* define rt uart max vaild data len */
#define MAX_RT_BUF_LEN 25
/* define rt uart gpio pin to notice extern device */
#define SBUS_SYNC_IRQ_GPIO          (HAL_GPIO_NUM81)
/* define rt uart gpio pin high level keep time */
#define SBUS_RX_DATA_TIME_WINDOW    (8*1000)
/* define rt uart Hal timer num */
#define SBUS_DELAY_HAL_TIMER        (HAL_TIMER_NUM14)
/* define rt uart read BaseBasd time in sky */
#define SBUS_SKY_POLLING_DATA_TIME_WINDOW    (5*1000)

static int hal_uart_rx_cnt=0,bb_uart_rx_cnt=0,write_bb_cnt=0;//debug info 
static uint8_t spi_num;
static uint8_t rt_uart_buf[MAX_RT_BUF_LEN];
static volatile uint8_t rt_uart_cnt=0;

/**
* @brief  uart irq callback function,nothing todo
* @param  null
* @retval 
* @note   
*/
static uint32_t todo_nothing(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    return 0;
}
/**
* @brief  from factory setting to get rt-uart max send data len
* @param  null
* @retval max rt-uart send data len,(sdk max support value is 25)
* @note   This function will be called rt-uart task init
*/
uint8_t get_fac_spi_num(void)
{
    STRU_MIMO_MODE *pst_mimo_mode = NULL;
    uint8_t spi_num = SPI_DATA_TRANS_MAX_LEN;
        
    pst_mimo_mode = (STRU_MIMO_MODE *)FCT_GetNodeAndData(FACTORY_SUBNODE_MIMO_MODE_ID,NULL);
    if(pst_mimo_mode != NULL)
    {
        spi_num = pst_mimo_mode->spi_num;
    }
    else
    {
        DLOG_Warning("get mimo spi_num null");
    }

    if(spi_num > SPI_DATA_TRANS_MAX_LEN)
    {
        DLOG_Warning("force to %d",SPI_DATA_TRANS_MAX_LEN);
        spi_num = SPI_DATA_TRANS_MAX_LEN;
    }

    return spi_num;
}
/**
* @brief  user gpio to notice extern device send data to C201-D module,
          start a timer to wait.
* @param  irq number
* @retval null
* @note   This function will be called when BaseBand TX Irq, only called by ground device
*/
static void wimax_irq(uint32_t value)
{
    HAL_RET_T ret;
    
    HAL_GPIO_SetPin(SBUS_SYNC_IRQ_GPIO,HAL_GPIO_PIN_SET);
    NVIC_DisableIRQ(BB_TX_ENABLE_VECTOR_NUM);
    ret = HAL_TIMER_Start(SBUS_DELAY_HAL_TIMER);
    if(ret != HAL_OK)
    {
        DLOG_Error("start timer failed");
    }
}
/**
* @brief  start a timer to wait when to read BaseBand data
* @param  irq number
* @retval null
* @note   This function will be called when BaseBand RX Irq, only called by sky devcie
*/
static void sky_wimax_irq(uint32_t value)
{
    HAL_RET_T ret;
    
    //HAL_GPIO_SetPin(SBUS_SYNC_IRQ_GPIO,HAL_GPIO_PIN_SET);
    NVIC_DisableIRQ(BB_RX_ENABLE_VECTOR_NUM);
    ret = HAL_TIMER_Start(SBUS_DELAY_HAL_TIMER);
    if(ret != HAL_OK)
    {
        DLOG_Error("start timer failed");
    }
}
/**
* @brief  receive extern uart data and put to receive buf
* @param  data and data len
* @retval date len
* @note   This function will be called when Hal Uart Irq, only called by ground device
*/
static uint32_t bypass_hal_uart_irq_handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    uint8_t i;
    
    if(u8_len == 0)
    {
        DLOG_Warning("len == 0");
        return 0;
    }
    i = u8_len + rt_uart_cnt;

    if(i > MAX_RT_BUF_LEN)
    {
        DLOG_Error("%d over max spi num",i);
        rt_uart_cnt += u8_len;
        return 0;
    }

    for(i=0;i<u8_len;i++)
    {
        rt_uart_buf[rt_uart_cnt+i] = pu8_rxBuf[i];
    }
    rt_uart_cnt += u8_len;
    
    hal_uart_rx_cnt += u8_len;
    /*for(int i=0;i<u8_len;i++)
    {
        DLOG_Warning("%02x",pu8_rxBuf[i]);
    }*/
}
/**
* @brief  read BaseBand data,then Output from Hal uart.
* @param  data and data len
* @retval date len
* @note   This function only called by sky
*/
static void bypass_bb_spi_irq_handler(void *p)
{
    #define BB_READ_MAX_LEN 32
    uint32_t cnt;
    uint8_t buffer[BB_READ_MAX_LEN];
    HAL_RET_T ret;
    static uint64_t u32_start;

    cnt = 0;
    ret = HAL_BB_ComReceiveMsg(BYPASS_BB_COM, buffer,BB_READ_MAX_LEN,&cnt);
    if(ret != HAL_OK && cnt != 0)
    {
        DLOG_Error("failed read bbcom %02x,%d",ret,cnt);
        return;
    }

    if(cnt > 0 && cnt <= BB_READ_MAX_LEN)
    {

        if(cnt > 10)
        {
            DLOG_Info("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],\
                buffer[5],buffer[6],buffer[7],buffer[8],buffer[9]);
        }

        ret = HAL_UART_TxData(BYPASS_UART_SPI_NUM, buffer, cnt,0);
        if(ret != HAL_OK)
        {
            DLOG_Error("failed ret %x,%d",ret,(uint32_t)(SysTicks_GetUsDiff(u32_start, SysTicks_GetUsTickCount())));
        }
        u32_start = SysTicks_GetUsTickCount();
        bb_uart_rx_cnt += cnt;

    }
    else
    {
        DLOG_Info("bb read %d",cnt);
    }
    
}
/**
* @brief  write uart recevie buf data to BaseBand
* @param  null
* @retval null
* @note   This function only called by ground
*/
static void hal_uart_tx2bb_spi(void)
{
    HAL_RET_T ret;
    uint32_t len ;

    if(rt_uart_cnt == 0)
    {
        DLOG_Info("rt uart data read null");
        return;
    }
    
    if(rt_uart_cnt != spi_num)
    {
        DLOG_Warning("rt uart data len %d != %d",rt_uart_cnt,spi_num);
    }

    len = rt_uart_cnt > spi_num ? spi_num : rt_uart_cnt;
    ret = HAL_BB_ComSendMsg(BYPASS_BB_COM,rt_uart_buf,len);
    if(ret != HAL_OK)
    {
        DLOG_Error("failed ret %x",ret);
    }
    write_bb_cnt += len;
    rt_uart_cnt = 0;
}
/**
* @brief  print info in debug uart
* @param  null
* @retval null
* @note   
*/
void sbus_spi_log_print(uint32_t value)
{     
    DLOG_Warning("------> hal_uart_rx = %d, bb_uart_rx= %d, write bb = %d",
        hal_uart_rx_cnt,bb_uart_rx_cnt,write_bb_cnt);
}
/**
* @brief  call "hal_uart_tx2bb_spi" function write data to BaseBand,
            then stop receice uart data and set pin to low level
* @param  null
* @retval null
* @note   only called by ground device
*/
void sbus_delay_notice(uint32_t value)
{
    HAL_RET_T ret;
    
    hal_uart_tx2bb_spi();
    HAL_GPIO_SetPin(SBUS_SYNC_IRQ_GPIO,HAL_GPIO_PIN_RESET);
    ret = HAL_TIMER_Stop(SBUS_DELAY_HAL_TIMER);
    if(ret != HAL_OK)
    {
        DLOG_Error("stop timer failed");
    }

    NVIC_ClearPendingIRQ(BB_TX_ENABLE_VECTOR_NUM);
    NVIC_EnableIRQ(BB_TX_ENABLE_VECTOR_NUM);
}
/**
* @brief  call "bypass_bb_spi_irq_handler" function read data from BaseBand,
            and stop timer and clear BaseBand Irq
* @param  Irq number
* @retval null
* @note   only called by sky device
*/
void sky_sbus_delay_notice(uint32_t value)
{
    HAL_RET_T ret;
    
    bypass_bb_spi_irq_handler(NULL);
    ret = HAL_TIMER_Stop(SBUS_DELAY_HAL_TIMER);
    if(ret != HAL_OK)
    {
        DLOG_Error("stop timer failed");
    }

    NVIC_ClearPendingIRQ(BB_RX_ENABLE_VECTOR_NUM);
    NVIC_EnableIRQ(BB_RX_ENABLE_VECTOR_NUM);

}
/**
* @brief  init extern notice gpio pin,enable BaseBand Tx Irq,register a timer to 
            keep gpio high level time
* @param  Irq number
* @retval null
* @note   only called by ground device
*/
void wimax_irq_init(void)
{
    //init gpio to output and default level is low
    HAL_GPIO_SetMode(SBUS_SYNC_IRQ_GPIO, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(SBUS_SYNC_IRQ_GPIO);
    HAL_GPIO_SetPin(SBUS_SYNC_IRQ_GPIO,HAL_GPIO_PIN_RESET);
    //register BaseBand Irq, when BaseBand Tx,will generate TX_ENABLE irq
    reg_IrqHandle(BB_TX_ENABLE_VECTOR_NUM,wimax_irq,NULL);
    NVIC_SetPriority(BB_TX_ENABLE_VECTOR_NUM,NVIC_EncodePriority(NVIC_PRIORITYGROUP_5,INTR_NVIC_PRIORITY_UART_DEFAULT,0)); 
    //register timer to 
    if (HAL_OK != HAL_TIMER_RegisterTimer(SBUS_DELAY_HAL_TIMER, SBUS_RX_DATA_TIME_WINDOW, sbus_delay_notice))
    {
        DLOG_Critical("--->failed register timer");
        return;
    }
    NVIC_SetPriority(TIMER_INTR14_VECTOR_NUM,NVIC_EncodePriority(NVIC_PRIORITYGROUP_5,INTR_NVIC_PRIORITY_UART_DEFAULT,0));

    NVIC_EnableIRQ(BB_TX_ENABLE_VECTOR_NUM);
}
/**
* @brief  enable BaseBand Rx Irq,register a timer to 
            read BaseBand Data cycle
* @param  Irq number
* @retval null
* @note   only called by sky device
*/
void sky_wimax_irq_init(void)
{
    reg_IrqHandle(BB_RX_ENABLE_VECTOR_NUM,sky_wimax_irq,NULL);
    NVIC_SetPriority(BB_RX_ENABLE_VECTOR_NUM,NVIC_EncodePriority(NVIC_PRIORITYGROUP_5,INTR_NVIC_PRIORITY_UART_DEFAULT,0)); 

    if (HAL_OK != HAL_TIMER_RegisterTimer(SBUS_DELAY_HAL_TIMER, SBUS_SKY_POLLING_DATA_TIME_WINDOW, sky_sbus_delay_notice))
    {
        DLOG_Critical("--->failed register timer");
        return;
    }
    NVIC_SetPriority(TIMER_INTR14_VECTOR_NUM,NVIC_EncodePriority(NVIC_PRIORITYGROUP_5,INTR_NVIC_PRIORITY_UART_DEFAULT,0));

    NVIC_EnableIRQ(BB_RX_ENABLE_VECTOR_NUM);

}

/**
* @brief  from factory setting get rt-uart baudrate
* @param  null
* @retval factory setting uart baudrate
* @note   
*/
ENUM_HAL_UART_BAUDR get_fac_uart4_baudrate(void)
{
    STRU_UART_BAUDR *pst_uart_baudr = NULL;
    ENUM_HAL_UART_BAUDR enum_baud = HAL_UART_BAUDR_115200;
        
    pst_uart_baudr = (STRU_UART_BAUDR *)FCT_GetNodeAndData(FACTORY_SUBNODE_UART_BAUDR_ID,NULL);
    if(pst_uart_baudr != NULL)
    {
        if(pst_uart_baudr->st_uartBaudr[0] <= HAL_UART_BAUDR_460800)
        {
            enum_baud = (ENUM_HAL_UART_BAUDR)pst_uart_baudr->st_uartBaudr[0];
            DLOG_Warning("rt-uart4 baud = %d",enum_baud);
        }
        else
        {
            DLOG_Warning("rt-uart4 baud %d ,force to  = %d",pst_uart_baudr->st_uartBaudr[0],enum_baud);
        }

    }
    else
    {
        DLOG_Warning("get pst_uart_baudr null");
    }
    
    return enum_baud;

}
/**
* @brief  use rt-uart function, need call it in main.c
* @param  0:sky device , 1:ground device
* @retval null
* @note   This function must be called when use rt-uart function.
*/
void usr_bypass_sbus_uart_task(uint8_t dev_type)
{
    HAL_RET_T ret;
    uint32_t cur_time,i;

    cur_time = HAL_GetSysMsTick();
    while(cur_time + 2000 > HAL_GetSysMsTick())
    {
        if(i++ > 10000)
        {
            i=0;
            WATCHDOG_Reset();
        }
    }

    //get factory setting rt-uart value, max config value is 25
    spi_num = get_fac_spi_num();
    if(spi_num == 0)
    {
        //HAL_BB_SpiDataTransInit(0);
        DLOG_Warning("0,rt-uart(sbus) is off");
        return;
    }

    //get fatcory setting rt-uart baudrate
    ENUM_HAL_UART_BAUDR enum_baud = get_fac_uart4_baudrate();

    if(dev_type)//grd
    {
        HAL_UART_Init(BYPASS_UART_SPI_NUM,enum_baud,bypass_hal_uart_irq_handler);
    }
    else
    {
        HAL_UART_Init(BYPASS_UART_SPI_NUM,enum_baud,todo_nothing);
    }

    //init BaseBand rt-uart function
    ret = HAL_BB_SpiDataTransInit(spi_num);
    if(ret != HAL_OK)
    {
        DLOG_Error("BB SPI init failed");
    }

    if(dev_type == 0)
    {
        //register BaseBand rt-uart session, will malloc a special session to rt-uart function 
        ret = HAL_BB_ComRegisterSession(BYPASS_BB_COM,
                                       BB_COM_SESSION_PRIORITY_HIGH,
                                       BB_COM_SESSION_DATA_NORMAL,
                                       NULL);
        if(ret != HAL_OK)
        {
            DLOG_Error("BB ComRegister failed");
        }
    }
    
    DLOG_Warning("rt-uart maxLen:%d", spi_num);

    //register timer to log print
    if (HAL_OK != HAL_TIMER_RegisterTimer(HAL_TIMER_NUM15, 1000*1000*5, sbus_spi_log_print))
    {
        DLOG_Error("--->failed log timer");
    }

    if(dev_type)
    {
        wimax_irq_init();
    }
    else
    {
        sky_wimax_irq_init();
    }

}

