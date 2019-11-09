#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_bb_com.h"
#include "debuglog.h"
#include "hal.h"
#include "hal_bb.h"
#include "hal_gpio.h"
#include "memory_config.h"
#include "debuglog.h"
#include "cmsis_os.h"
#include "systicks.h"
#include "hal_nvic.h"
#include "board_watchdog.h"

uint32_t g_count_rt = 0;
uint32_t g_count_normal = 0;

#define     SPI_DATA_TRANS_TEST_PIN         (HAL_GPIO_NUM97) //CAN_TXD3

uint8_t sendBuff[SPI_DATA_TRANS_MAX_LEN];
uint8_t sendLen = 0;
static volatile uint32_t cnt = 0;
static uint8_t grd_pin = 0;


/* Low priority High priority */
static void rcvDataHandler(void *p)
{
    //DLOG_Error("rcvDataHandler\n");

    HAL_RET_T ret;
    uint8_t   data_buf_proc[512];
    uint32_t  u32_rcvLen = 0;

    //Low priority
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_1, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        DLOG_Error("Low cnt %d", data_buf_proc[0]);
    }

    //High priority
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_2, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        DLOG_Error("high cnt %d", data_buf_proc[0]);
    }
}



/* normal mode rt mode */
static void rcvDataHandler2(void *p)
{
    //DLOG_Error("rcvDataHandler2\n");
    
    HAL_RET_T ret;
    uint8_t   data_buf_proc[512];
    uint32_t  u32_rcvLen = 0;
    uint32_t * info_id = (uint32_t * )data_buf_proc;

    //normal mode
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_1, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        ++g_count_normal;
        DLOG_Error("%d normal cnt %u", g_count_normal, (*info_id));
    }

    //rt mode
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_2, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        ++g_count_rt;
        DLOG_Error("%d rt cnt %u", g_count_rt, (*info_id));
    }
}

/* normal mode rate test */
static void rcvDataHandler3(void *p)
{
    HAL_RET_T ret;
    uint8_t   data_buf_proc[512];
    uint32_t  u32_rcvLen = 0;
    uint32_t * info_id = (uint32_t * )data_buf_proc;

    //normal mode
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_1, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {
        ++g_count_normal;
        DLOG_Error("%d normal cnt %u", g_count_normal, (*info_id));
    }
}

void command_TestRcv(void const *argument)
{
    /* low priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_3, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }

    /* high priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_HIGH, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    } 
    
}


void command_TestRevRT(void const *argument)
{
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }

    /* rt mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_RT, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }    
}

void command_TestRcvRate(void const *argument)
{
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler3))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }
}

void command_TestComTask(void const *argument)
{
    uint8_t high_cnt = 0;
    uint8_t low_cnt  = 0;
    uint32_t cnt = 0;

    uint8_t low_buf[2];
    uint8_t high_buf[2];
    
    /* low priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_3, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_3);
    }

    /* high priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_HIGH, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }    

    //send case 1:
    //high low priority test: 10 : 1 Low session datum: High session datum

    while(1)
    {
        HAL_RET_T ret = 0;
        
        cnt = 0;
        while (cnt < 10)
        {
            low_buf[0] = low_cnt;

            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_3, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            cnt ++;
            low_cnt ++;
        }

        do
        {
            high_buf[0] = high_cnt;

            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_2, high_buf, sizeof(high_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            high_cnt ++;
        }while(HAL_OK != ret);

        HAL_Delay(1000);
    }
}


//send case 2:
//test rt mode
//test the difference between normal mode and rt mode
void command_TestRT(void const *argument)
{
    uint8_t high_cnt = 0;
    uint8_t low_cnt  = 0;
    uint32_t cnt = 0;

    uint8_t low_buf[32];
    uint8_t high_buf[32];
    uint32_t * info_id = (uint32_t *)low_buf;
    
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }

    /* rt mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_RT, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }    

    while(1)
    {
        HAL_RET_T ret = 0;
        *info_id = (cnt++);// big or little ?

        while(1)
        {
            /* normal mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_1, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        }

        while(1)
        {
            /* rt mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_2, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        }
        
    }
}


//send case 3:
//data include header format
void command_TestHead(void const *argument)
{
    uint8_t high_cnt = 0;
    uint8_t low_cnt  = 0;
    uint32_t cnt = 0;

    uint8_t low_buf[32];
    uint8_t high_buf[32];

    uint32_t * info_id = (uint32_t *)low_buf;
    uint8_t header[] = {0xFF, 0x5A, 0x0F};
    
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }

    /* rt mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_RT, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }  

    while(1)
    {
        HAL_RET_T ret = 0;        
        *info_id = (cnt++);// big or little ?;
        memcpy(&(low_buf[4]), header, sizeof(header));// include head format

        while(1)
        {
            /* normal mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_1, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        } 

        while(1)
        {
            /* rt mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_2, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

//send case 4:
//data period include header format
void command_TestPeriodHead(void const *argument)
{
    uint8_t high_cnt = 0;
    uint8_t low_cnt  = 0;
    uint32_t cnt = 0;
    uint32_t period = 0;
    uint8_t low_buf[32];
    uint8_t high_buf[32];

    uint32_t * info_id = (uint32_t *)low_buf;
    uint8_t header[] = {0xFF, 0x5A, 0x0F};
    
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }

    /* rt mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_RT, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }  

    while(1)
    {
        HAL_RET_T ret = 0;        
        *info_id = (cnt++);// big or little ?;
        if (0 == ((period++)%5))
        {
            memcpy(&(low_buf[4]), header, sizeof(header));// include head format
        }
        else
        {
            memset(&(low_buf[4]), 0x00, sizeof(header));// include head format
        }

        while(1)
        {
            /* normal mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_1, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        } 

        while(1)
        {
            /* rt mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_2, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

//send case 5:
//normal mode rate test.
void command_TestRate(void const *cycle)
{
    uint32_t cnt = 0;
    uint8_t low_buf[32];
    HAL_RET_T ret = 0;        
    uint32_t * info_id = (uint32_t *)low_buf;
    uint32_t start, end, diff, send_num;
    uint32_t cycle_cnt = strtoul(cycle, NULL, 0);
    
    DLOG_Error("cycle:%d\n", cycle_cnt);
    
    /* normal mode */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, rcvDataHandler2))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_1);
    }
    
    start = SysTicks_GetTickCount();
    while(cnt < cycle_cnt)
    {
        *info_id = (cnt++);// big or little ?;

        while(1)
        {
            /* normal mode */
            ret = HAL_BB_ComSendMsg(BB_COM_SESSION_1, low_buf, sizeof(low_buf));
            if(HAL_OK != ret)
            {
                HAL_Delay(10);
                continue;
            }
            else
            {
                break;
            }
        } 
    }
    end = SysTicks_GetTickCount();
    diff = SysTicks_GetDiff(start, end);
    send_num = cycle_cnt * sizeof(low_buf) / (diff / 14);
    DLOG_Error("start:%d end:%d diff:%d send:%d", start, end, diff, send_num);
}


static void TestComInitHandler(void *p)
{
    HAL_RET_T   ret;
    uint8_t     data_buf_proc[64];
    uint32_t    u32_rcvLen          = 0;
    uint8_t *   info_id             = (uint8_t * )data_buf_proc;

    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_2, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    if (ret == HAL_OK && u32_rcvLen > 0)
    {        
        DLOG_Error("%d normal cnt %u Message len %d", g_count_normal, (*info_id), u32_rcvLen);
        ++g_count_normal;
    }
}

static void TestComInitSend()
{
    uint32_t    i;
    uint8_t     high_buf[25];
    HAL_RET_T   ret = 0;  
    uint32_t    loop = 0;

    for(i = 0; i < 25; i++)
    {
        high_buf[i] = i;
    }

    do
    {
        high_buf[0] = (uint8_t)(cnt & 0xff);

        ret = HAL_BB_ComSendMsg(BB_COM_SESSION_2, high_buf, sizeof(high_buf));
        if(HAL_OK != ret)
        {
            HAL_Delay(100);
            loop++;
            continue;
        }
        ++cnt;
    }while(HAL_OK != ret);

    HAL_NVIC_DisableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);

    DLOG_Warning("Send index:%d loop:%d", cnt, loop);

}

void command_TestComInitSend(unsigned char * initflag, unsigned char *initlen)
{   
    uint8_t     flag;
    uint8_t     len;

    flag = (uint8_t)strtoul(initflag, NULL, 0);
    len = (uint8_t)strtoul(initlen, NULL, 0);

    if(1 == flag)
    {
        HAL_BB_SpiDataTransInit(len);
    }
    
    /* high priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_HIGH, BB_COM_SESSION_DATA_NORMAL, TestComInitHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }           

    HAL_NVIC_RegisterHandler(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM, TestComInitSend, NULL);
    HAL_NVIC_SetPriority(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM, 6, 0);
    HAL_NVIC_DisableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM); 

    DLOG_Warning("Test start!");
    HAL_NVIC_EnableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
}


void command_TestComInitRcv(unsigned char * initflag, unsigned char *initlen)
{
    uint8_t     flag;
    uint8_t     len;

    flag = (uint8_t)strtoul(initflag, NULL, 0);
    len = (uint8_t)strtoul(initlen, NULL, 0);

    if(1 == flag)
    {
        HAL_BB_SpiDataTransInit(len);
    }
    
    /* high priority */
    if (HAL_OK != HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_HIGH, BB_COM_SESSION_DATA_NORMAL, TestComInitHandler))
    {
        DLOG_Error("Fail register session %d", BB_COM_SESSION_2);
    }     
}






static void SpiDataRcvHandler(void *p)
{
    static uint32_t handler_cnt = 0;
    static uint8_t pin = 0;
    uint32_t rcv_cnt = 0;
    uint8_t rcvBuff[SPI_DATA_TRANS_MAX_LEN];
    uint32_t rcvLen = 0;
    uint8_t i = 0;

    memset(rcvBuff, 0x00, SPI_DATA_TRANS_MAX_LEN);
    
    HAL_BB_ComReceiveMsg(BB_COM_SESSION_SPI, rcvBuff,SPI_DATA_TRANS_MAX_LEN,&rcvLen);
    
    HAL_GPIO_SetPin(SPI_DATA_TRANS_TEST_PIN, (pin++)%2);	
    
    rcv_cnt = *(uint32_t *)(&rcvBuff[0]);
    DLOG_Warning("%d %d %d", rcvLen, rcv_cnt, handler_cnt++);

    for(i=0; i<rcvLen; i++)
    {
        DLOG_Warning("[%d]:%d", i, rcvBuff[i]);
    }
}

void command_TestSpiSkyInit(unsigned char *maxLen)
{
    uint8_t len = (uint8_t)strtoul(maxLen, NULL, 0);

    HAL_BB_SpiDataTransInit(len);

    HAL_BB_ComRegisterSession(BB_COM_SESSION_SPI,
                                   BB_COM_SESSION_PRIORITY_HIGH,
                                   BB_COM_SESSION_DATA_NORMAL,
                                   SpiDataRcvHandler);
    
    DLOG_Warning("maxLen:%d", len);

    
    HAL_GPIO_SetMode(SPI_DATA_TRANS_TEST_PIN,HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(SPI_DATA_TRANS_TEST_PIN);
    HAL_GPIO_SetPin(SPI_DATA_TRANS_TEST_PIN, 0);	
}

void grd_send_isr(uint32_t vec)
{
    DLOG_Warning("%d", SysTicks_GetTickCount());
    HAL_NVIC_DisableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
    HAL_BB_ComSendMsg(BB_COM_SESSION_SPI, sendBuff, (uint32_t)sendLen);
    HAL_GPIO_SetPin(SPI_DATA_TRANS_TEST_PIN, (grd_pin++)%2);	
    DLOG_Warning("%d ", cnt);
    HAL_NVIC_ClearPendingIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
}


void command_TestSpiGrdInit(unsigned char *maxLen)
{
    uint8_t len = (uint8_t)strtoul(maxLen, NULL, 0);

    HAL_BB_SpiDataTransInit(len);

    DLOG_Warning("maxLen:%d", len);
    
    HAL_GPIO_SetMode(SPI_DATA_TRANS_TEST_PIN,HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(SPI_DATA_TRANS_TEST_PIN);
    HAL_GPIO_SetPin(SPI_DATA_TRANS_TEST_PIN, 0);	

    
    HAL_NVIC_RegisterHandler(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM, grd_send_isr, NULL);
    HAL_NVIC_SetPriority(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM, 6, 0);
    HAL_NVIC_DisableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
}

void command_TestSpiGrdSend(unsigned char *start_value, unsigned char *len)
{
    uint8_t i = 0;
    uint8_t value = (uint8_t)strtoul(start_value, NULL, 0);
    sendLen = (uint8_t)strtoul(len, NULL, 0);
    
    HAL_NVIC_ClearPendingIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);

    sendLen = (sendLen <=  SPI_DATA_TRANS_MAX_LEN) ? sendLen : SPI_DATA_TRANS_MAX_LEN;
    
    for(i = 0; i < sendLen; i++)
    {
        sendBuff[i] = value + i;
    }
    
    *(uint32_t *)(&sendBuff[0]) = cnt++;
    HAL_NVIC_EnableIrq(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
    DLOG_Warning("%d", SysTicks_GetTickCount());
    //grd_send_isr(HAL_NVIC_BB_TX_ENABLE_VECTOR_NUM);
}

