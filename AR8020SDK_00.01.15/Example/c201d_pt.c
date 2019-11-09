#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "debuglog.h"
#include "hal_gpio.h"
#include "systicks.h"
#include "hal_spi.h"
#include "hal_adc.h"
#include "hal_pwm.h"
#include "c201d_pt.h"
#include "hal.h"
#include "hal_uart.h"
#include "hal_i2c.h"
#include "cmsis_os.h"
/*
系统程序结构
*/
int Autotest_status =0;
typedef int (*fun)(char);
typedef void (*fun2)(char);

#define BLUE  62
#define RED   58
#define ON    1
#define OFF   0

#define TAR_24C64_ADDR           (0x51)
#define TEST_24C64_BUFSIZE       8
#define I2C_24C64_TIMEOUT_VALUE  10


typedef struct _TEST_INFO{

	char testnum;
	fun testfun;
	char *name;
	int flag;
	int result;

}TEST_INFO;

typedef struct _TEST_PIN{

    unsigned char pin_A;
    unsigned char pin_B;

#define pin_sg  2
#define pin_ok  1
#define pin_err 0

    unsigned char flag;


}TEST_PIN;

typedef struct _TEST_ADC{

    unsigned char chnl;

#define adc_ok  1
#define adc_err 0

    unsigned char flag;


}TEST_ADC;

typedef struct _TEST_PWM{

    unsigned char chnl;
    unsigned char gpio;
    
#define pwm_ok  1
#define pwm_err 0

    unsigned char flag;


}TEST_PWM;


typedef struct _GPIO_INTER{

    unsigned char gpio;
    unsigned char level;
    unsigned char polarity;
    fun2 testfun;
    
}GPIO_INTER;

int bringup_test_init(char num);
int bringup_gpio_test(char num);
int bringup_i2c_test(char num);
int bringup_pwm_test(char num);
int bringup_usb0_test(char num);
int bringup_usb1_test(char num);
int bringup_uart_test(char num);
int bringup_adc_test(char num);
int bringup_test_finish(char num);

void Gpio_Irq_Hig_Handler1(char num);
void Gpio_Irq_Hig_Handler2(char num);
void Gpio_Irq_Hig_Handler3(char num);
void Gpio_Irq_Hig_Handler4(char num);
void Gpio_Irq_Low_Handler1(char num);
void Gpio_Irq_Low_Handler2(char num);
void Gpio_Irq_Low_Handler3(char num);
void Gpio_Irq_Low_Handler4(char num);


TEST_INFO test_info[] = {
    
    {1, bringup_test_init,      "sys_init",     0,  0},
    {2, bringup_gpio_test,      "gpio test",    0,  0},
    {3, bringup_pwm_test,       "pwm test",     0,  0},     
//    {5, bringup_usb0_test,      "usb0 test",    0,  0},
//    {6, bringup_usb1_test,      "usb1 test",    0,  0},
    {4, bringup_uart_test,      "uart test",    0,  0},
    {5, bringup_adc_test,       "adc_test",     0,  0},
    {6, bringup_i2c_test,       "i2c test",     0,  0},
    {7, bringup_test_finish,    "test_finish",  0,  0},   
    {0, 0,                        "finished",     0,  0}

};

TEST_PIN test_pin[] = {

//pin_A     pin_B   flag

    {0,     34,     pin_err},
    {1,     3,      pin_err},
    {5,     6,      pin_err},
    {2,     32,     pin_err},
    {4,     7,      pin_err},
    
    {66,    58,     pin_err},
    {62,    70,     pin_err},
    {101,   97,     pin_err},

//pwm
    {80,    81,     pin_sg},
    {82,    83,     pin_sg},


};


TEST_ADC test_adc[] = {


    {1,     adc_ok},
    {2,     adc_ok},
    {3,     adc_ok},
    {4,     adc_ok},
    {5,     adc_ok},
    {6,     adc_ok},
    {7,     adc_ok},

};


TEST_PWM test_pwm[] = {

    {0,     80,     pwm_ok},
    {1,     81,     pwm_ok},
    {2,     82,     pwm_ok},
    {3,     83,     pwm_ok},

};

GPIO_INTER test_io_inter[] = {
    
//hig
    {0, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_HIGH,   Gpio_Irq_Hig_Handler1},
    {1, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_HIGH,   Gpio_Irq_Hig_Handler2},
    {5, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_HIGH,   Gpio_Irq_Hig_Handler3},
    {2, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_HIGH,   Gpio_Irq_Hig_Handler4},

//low
    {34, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_LOW,   Gpio_Irq_Low_Handler1},
    {3, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_LOW,   Gpio_Irq_Low_Handler2},
    {6, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_LOW,   Gpio_Irq_Low_Handler3},
    {32, HAL_GPIO_EDGE_SENUMSITIVE,  HAL_GPIO_ACTIVE_LOW,   Gpio_Irq_Low_Handler4},

};

unsigned int pwm_data1[80];
unsigned int pwm_data2[80];
unsigned int pwm_data3[80];
unsigned int pwm_data4[80];

unsigned int pwm_cnt1 = 100;
unsigned int pwm_cnt2 = 100;
unsigned int pwm_cnt3 = 100;
unsigned int pwm_cnt4 = 100;


void Gpio_Irq_Hig_Handler1(char num)
{
    uint64_t time;


    time = HAL_GetSysUsTick();

    if(pwm_cnt1 < 80)
    {
        pwm_data1[pwm_cnt1] = (unsigned int)time;
        if((pwm_cnt1 % 2) == 0)
        pwm_cnt1 ++;
    }
}
void Gpio_Irq_Hig_Handler2(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt2 < 80)
    {
        pwm_data2[pwm_cnt2] = (unsigned int)time;
        if((pwm_cnt2 % 2) == 0)
        pwm_cnt2 ++;
    }

}
void Gpio_Irq_Hig_Handler3(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt3 < 80)
    {
        pwm_data3[pwm_cnt3] = (unsigned int)time;
        if((pwm_cnt3 % 2) == 0)
        pwm_cnt3 ++;
    }

}
void Gpio_Irq_Hig_Handler4(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt4 < 80)
    {
        pwm_data4[pwm_cnt4] = (unsigned int)time;
        if((pwm_cnt4 % 2) == 0)
        pwm_cnt4 ++;
    }

}
void Gpio_Irq_Low_Handler1(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt1 < 80)
    {
        pwm_data1[pwm_cnt1] = (unsigned int)time;
        if((pwm_cnt1 % 2) == 1)
            pwm_cnt1 ++;
    }

}
void Gpio_Irq_Low_Handler2(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt2 < 80)
    {
        pwm_data2[pwm_cnt2] = (unsigned int)time;
        if((pwm_cnt2 % 2) == 1)
            pwm_cnt2 ++;
    }

}
void Gpio_Irq_Low_Handler3(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt3 < 80)
    {
        pwm_data3[pwm_cnt3] = (unsigned int)time;
        if((pwm_cnt3 % 2) == 1)
            pwm_cnt3 ++;
    }

}
void Gpio_Irq_Low_Handler4(char num)
{
    uint64_t time;
    time = HAL_GetSysUsTick();

    if(pwm_cnt4 < 80)
    {
        pwm_data4[pwm_cnt4] = (unsigned int)time;
        if((pwm_cnt4 % 2) == 1)
            pwm_cnt4 ++;
    }

}

void Gpio_Interrupt_Init(void)
{
    int i;
    
    for(i = 0; i < (sizeof(test_io_inter)/sizeof(GPIO_INTER)); i ++)
    {
        HAL_GPIO_RegisterInterrupt(test_io_inter[i].gpio, test_io_inter[i].level, test_io_inter[i].polarity,test_io_inter[i].testfun);
    }
}

char test_uart_buffer[256];
char test_uart_cnt = 0;
static uint32_t Uart4_Irq_Handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    char u8_cnt = u8_len;
    char pbuf[256];
    
    if(u8_len > 0)
    {
        memcpy(&test_uart_buffer[test_uart_cnt],pu8_rxBuf,u8_len);
        test_uart_cnt += u8_len;
    }
}

static uint32_t Uart5_Irq_Handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    char u8_cnt = u8_len;
    char pbuf[256];
    
    char i;

    if(u8_len > 0)
    {
        memcpy(&test_uart_buffer[test_uart_cnt],pu8_rxBuf,u8_len);
        test_uart_cnt += u8_len;
    }
}

static uint32_t Uart6_Irq_Handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    char u8_cnt = u8_len;
    char pbuf[256];
    
    char i;

    if(u8_len > 0)
    {
        memcpy(&test_uart_buffer[test_uart_cnt],pu8_rxBuf,u8_len);
        test_uart_cnt += u8_len;
    }
}

static uint32_t Uart7_Irq_Handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    char u8_cnt = u8_len;
    char pbuf[256];
    
    char i;

    if(u8_len > 0)
    {
        memcpy(&test_uart_buffer[test_uart_cnt],pu8_rxBuf,u8_len);
        test_uart_cnt += u8_len;
    }
}



int bringup_test_init(char num)
{
    int i;
    int vadc;
//    DLOG_Critical("now is in sys_init");

//adc init
    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
    {
        HAL_ADC_Read(test_adc[i].chnl, test_adc[i].chnl);
    }

//io init include pwm
    for(i = 0; i < (sizeof(test_pin)/sizeof(TEST_PIN)); i ++)
    {
        //A,B设置为gpio模式
        HAL_GPIO_SetMode(test_pin[i].pin_A, HAL_GPIO_PIN_MODE2);
        HAL_GPIO_InPut(test_pin[i].pin_A);

        HAL_GPIO_SetMode(test_pin[i].pin_B, HAL_GPIO_PIN_MODE2);
        HAL_GPIO_InPut(test_pin[i].pin_B);
    }

//uart init
    HAL_UART_Init(HAL_UART_COMPONENT_4,HAL_UART_BAUDR_115200,Uart4_Irq_Handler);
    HAL_UART_Init(HAL_UART_COMPONENT_5,HAL_UART_BAUDR_115200,Uart5_Irq_Handler);
    HAL_UART_Init(HAL_UART_COMPONENT_6,HAL_UART_BAUDR_115200,Uart6_Irq_Handler);
    HAL_UART_Init(HAL_UART_COMPONENT_7,HAL_UART_BAUDR_115200,Uart7_Irq_Handler);
//i2c init
    HAL_I2C_MasterInit(HAL_I2C_COMPONENT_0,TAR_24C64_ADDR, HAL_I2C_FAST_SPEED);
   HAL_I2C_MasterInit(HAL_I2C_COMPONENT_2,TAR_24C64_ADDR, HAL_I2C_FAST_SPEED);

    return 0;

}


int bringup_gpio_test(char num)
{
    uint8_t i;
    uint32_t val;

    for(i = 0; i < (sizeof(test_pin)/sizeof(TEST_PIN)); i ++)
    {

        if(test_pin[i].flag != pin_err)
            continue;
        
//A,B设置为gpio模式
        HAL_GPIO_SetMode(test_pin[i].pin_A, HAL_GPIO_PIN_MODE2);
        HAL_GPIO_SetMode(test_pin[i].pin_B, HAL_GPIO_PIN_MODE2);

//A为输出
//B为输入
        HAL_GPIO_InPut(test_pin[i].pin_B);
        SysTicks_DelayMS(2);
        HAL_GPIO_OutPut(test_pin[i].pin_A);

    //A输出高
        HAL_GPIO_SetPin(test_pin[i].pin_A, 1);
        SysTicks_DelayMS(2);
        HAL_GPIO_GetPin(test_pin[i].pin_B,&val);
        if(val != 1)
        {
            HAL_GPIO_InPut(test_pin[i].pin_A);
            HAL_GPIO_InPut(test_pin[i].pin_B);

            continue;
        }
        
    //A输出低
        HAL_GPIO_SetPin(test_pin[i].pin_A, 0);
        SysTicks_DelayMS(2);
        HAL_GPIO_GetPin(test_pin[i].pin_B,&val);
        if(val != 0)
        {
            HAL_GPIO_InPut(test_pin[i].pin_A);
            HAL_GPIO_InPut(test_pin[i].pin_B);

            continue;
        }

//B为输出
//A为输入
        HAL_GPIO_InPut(test_pin[i].pin_A);
        SysTicks_DelayMS(2);
        HAL_GPIO_OutPut(test_pin[i].pin_B);


    //输出高
        HAL_GPIO_SetPin(test_pin[i].pin_B, 1);
        SysTicks_DelayMS(2);
        HAL_GPIO_GetPin(test_pin[i].pin_A,&val);
        if(val != 1)
        {
            HAL_GPIO_InPut(test_pin[i].pin_A);
            HAL_GPIO_InPut(test_pin[i].pin_B);

            continue;
        }

    //输出低
        HAL_GPIO_SetPin(test_pin[i].pin_B, 0);
        SysTicks_DelayMS(2);
        HAL_GPIO_GetPin(test_pin[i].pin_A,&val);
        if(val != 0)
        {
            HAL_GPIO_InPut(test_pin[i].pin_A);
            HAL_GPIO_InPut(test_pin[i].pin_B);

            continue;
        }
        
//A,B均输入
        HAL_GPIO_InPut(test_pin[i].pin_A);
        HAL_GPIO_InPut(test_pin[i].pin_B);

//测试均通过，当前状态设置ok
        test_pin[i].flag = pin_ok;

    }

    for(i = 0; i < (sizeof(test_pin)/sizeof(TEST_PIN)); i ++)
        if(test_pin[i].flag != pin_ok && test_pin[i].flag != pin_sg)
            return -1;
    return 0;
}

int bringup_i2c_test(char num)
{
    //DLOG_Critical("now is in i2c_test");
    unsigned char i=0;
    unsigned char data_src1[TEST_24C64_BUFSIZE+2]={0x00,0x00};
    unsigned char data_chk[TEST_24C64_BUFSIZE];
    
    //I2C0
    for(i=0;i<TEST_24C64_BUFSIZE;i++)
    {
        data_src1[i+2] =i;
    }
    HAL_I2C_MasterWriteData(0,TAR_24C64_ADDR,data_src1,10,I2C_24C64_TIMEOUT_VALUE);
    HAL_Delay(20);
    memset((void*)data_chk,0,sizeof(data_chk));
    HAL_I2C_MasterReadData(0,TAR_24C64_ADDR,data_src1,2,data_chk,TEST_24C64_BUFSIZE,I2C_24C64_TIMEOUT_VALUE);
    
     for(i=0;i<TEST_24C64_BUFSIZE;i++)
    {
        if(data_src1[i+2] !=data_chk[i])
        {
            return -1;
        }
    }
    
    //I2C2
    
    for(i=0;i<TEST_24C64_BUFSIZE;i++)
    {
        data_src1[i+2] =i;
    }
    HAL_I2C_MasterWriteData(2,TAR_24C64_ADDR,data_src1,10,I2C_24C64_TIMEOUT_VALUE);
    HAL_Delay(20);
    memset((void*)data_chk,0,sizeof(data_chk));
    HAL_I2C_MasterReadData(2,TAR_24C64_ADDR,data_src1,2,data_chk,TEST_24C64_BUFSIZE,I2C_24C64_TIMEOUT_VALUE);
    
     for(i=0;i<TEST_24C64_BUFSIZE;i++)
    {
        if(data_src1[i+2] !=data_chk[i])
        {
            return -1;
        }
    }
    
    
    
    return 0;
}

int bringup_pwm_test(char num)
{
    int i;
//    DLOG_Critical("now is in pwm_test");
    
    memset(pwm_data1,0,sizeof(pwm_data1));
    memset(pwm_data2,0,sizeof(pwm_data2));
    memset(pwm_data3,0,sizeof(pwm_data3));
    memset(pwm_data4,0,sizeof(pwm_data4));

    Gpio_Interrupt_Init();

    for(i = 0; i < (sizeof(test_pwm)/sizeof(TEST_PWM)); i ++)
    {
        HAL_GPIO_SetMode(test_pwm[i].gpio, HAL_GPIO_PIN_MODE0);
        HAL_PWM_RegisterPwm(test_pwm[i].chnl, 1000, 1000);
        HAL_PWM_Start(test_pwm[i].chnl);
    }

    HAL_Delay(20);

    pwm_cnt1 = 0;
    pwm_cnt2 = 0;
    pwm_cnt3 = 0;
    pwm_cnt4 = 0;

    HAL_Delay(200);

    //采集第一路
    for(i = 1;i < 70;i ++)
    {
        if((pwm_data1[i] - pwm_data1[i - 1]) < 980 || (pwm_data1[i] - pwm_data1[i - 1]) > 1020)
            test_pwm[0].flag= pwm_err;
    }

    for(i = 1;i < 70;i ++)
    {   
        if((pwm_data2[i] - pwm_data2[i - 1]) < 980 || (pwm_data2[i] - pwm_data2[i - 1]) > 1020)
            test_pwm[1].flag = pwm_err;
    }

    for(i = 1;i < 70;i ++)
    {
        if((pwm_data3[i] - pwm_data3[i - 1]) < 980 || (pwm_data3[i] - pwm_data3[i - 1]) > 1020)
            test_pwm[2].flag = pwm_err;
    }

    for(i = 1;i < 70;i ++)
    {
        if((pwm_data4[i] - pwm_data4[i - 1]) < 980 || (pwm_data4[i] - pwm_data4[i - 1]) > 1020)
        {
            test_pwm[3].flag = pwm_err;
        }
    }

    for(i = 0; i < (sizeof(test_pwm)/sizeof(TEST_PWM)); i ++)
        if(test_pwm[i].flag == pwm_err)
            return -1;

    return 0;
    
}

int bringup_usb0_test(char num)
{
    DLOG_Critical("now is in usb0_test");
    return 0;
}

int bringup_usb1_test(char num)
{
    DLOG_Critical("now is in usb1_test");
    return 0;
}

int bringup_uart_test(char num)
{
    char buffer[256];
    char i;
    int timeout = 0;
    char state;
    
    for(i = 0;i < 100;i ++)
        buffer[i] = i;
    
    memset(test_uart_buffer,0,100);
    test_uart_cnt = 0;
    state = 7;
    
//    DLOG_Critical("now is in uart_test");
    HAL_UART_TxData(state, buffer, 100, HAL_UART_DEFAULT_TIMEOUTMS * 10);

    
    while(1)
    {
        
        if(test_uart_cnt >= 100)
        {
            switch(state)
            {
                case 7:
                    state = 4;
                    break;
                case 4:
                    state = 5;
                    break;
                case 5:
                    state = 6;
                    break;
                case 6:
                    state = 7;
                    break;
            }
            memcpy(buffer,test_uart_buffer,100);
            memset(test_uart_buffer,0,100);
            test_uart_cnt = 0;
            if(state == 7)
            {
                for(i = 0;i < 10;i ++)
                {
                    if(buffer[i] != i)
                    {
                        return -1;
                    }
                }
                break;
            }
            else
                HAL_UART_TxData(state, buffer, 100, HAL_UART_DEFAULT_TIMEOUTMS * 10);
        }
                
        SysTicks_DelayMS(20);
        timeout ++;
        if(timeout > 100)
        {
            return -1;
            break;
        }
    }

    return 0;
}

int bringup_adc_test(char num)
{
    int vadc;
    int i;
    
//    DLOG_Critical("now is adc_test");

    HAL_GPIO_SetMode(33, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(33);

    //设置为高
    HAL_GPIO_SetPin(33, 1);
    SysTicks_DelayMS(20);

    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
    {
        HAL_ADC_Read(test_adc[i].chnl, test_adc[i].chnl);
    }
    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
    {
        vadc = HAL_ADC_Read(test_adc[i].chnl, test_adc[i].chnl);
        if(vadc < 930)
        {
            test_adc[i].flag = adc_err;
            continue;
        }
    }
    
    //设置为低
    HAL_GPIO_SetPin(33, 0);
    SysTicks_DelayMS(20);

    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
    {
        HAL_ADC_Read(test_adc[i].chnl, test_adc[i].chnl);
    }
        
    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
    {
        vadc = HAL_ADC_Read(test_adc[i].chnl, test_adc[i].chnl);
        if(vadc > 94)
        {
            test_adc[i].flag = adc_err;
            continue;
        }
    }

    for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
        if(test_adc[i].flag == adc_err)
            return -1;
        
    return 0;
}

int bringup_test_finish(char num)
{
    int i;

    DLOG_Critical("now is in test_finish");

    for(i = 0; test_info[i].testnum; i++)
    {
        if(test_info[i].result < 0)
        {
            for(i = 0; i < (sizeof(test_pin)/sizeof(TEST_PIN)); i ++)
            {
                if(test_pin[i].flag == pin_err)
                {
                    DLOG_Critical("gpio err : num1 : %d,num2 : %d ",test_pin[i].pin_A,test_pin[i].pin_B);
                }
            }
            
            for(i = 0; i < (sizeof(test_adc)/sizeof(TEST_ADC)); i ++)
            {
            
                if(test_adc[i].flag ==adc_err)
                {
                    DLOG_Critical("adc err : adc num : %d ",test_adc[i].chnl);
                }
            }
            
            for(i = 0; i < (sizeof(test_pwm)/sizeof(TEST_PWM)); i ++)
            {
            
                if(test_pwm[i].flag == pwm_err)
                {
                    DLOG_Critical("pwm err :  %d ",test_pwm[i].chnl);
                }
            }

            if(test_info[6].result < 0)
                DLOG_Critical("uart test failed ");

            break;

        }
    }
    return 0;    
}

int is_test_mode(void)
{
    int i;
    uint32_t val;
//set gpio mode
    HAL_GPIO_SetMode(4, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_SetMode(7, HAL_GPIO_PIN_MODE2);
//set out mode
    HAL_GPIO_OutPut(4);
    HAL_GPIO_InPut(7);

    for(i = 0;i < 10;i ++)
    {
        HAL_GPIO_SetPin(4, 1);
        HAL_Delay(1);
        HAL_GPIO_GetPin(7,&val);
        if(val != 1)
            break;
		
        HAL_GPIO_SetPin(4, 0);
        HAL_Delay(1);
        HAL_GPIO_GetPin(7,&val);
        if(val != 0)
            break;
    }
    
    HAL_GPIO_SetMode(4, HAL_GPIO_PIN_MODE0);
    HAL_GPIO_SetMode(7, HAL_GPIO_PIN_MODE0);

    if(i > 8)
        return 1;
    else
        return 0;
}

void set_led(unsigned char pin,unsigned char state)
{

    HAL_GPIO_SetMode(pin, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(pin);
    HAL_GPIO_SetPin(pin, state);
}

static void c201d_auto_test(void const *argument)
{
    uint8_t i;
    
/*
    DLOG_Critical("Item:");

    for(i = 1; test_info[i].testnum; i++)
    {
        DLOG_Critical("%s",test_info[i].name);
    }
*/
    for(i = 0; test_info[i].testnum; i++)
        test_info[i].result = test_info[i].testfun(i);

    for(i = 0; test_info[i].testnum; i++)
        if(test_info[i].result < 0)
        {
            DLOG_Critical("###test failed !!!###");
            Autotest_status = 0x00;
            set_led(BLUE,OFF);
            set_led(RED,ON);
            break;
        }

    if(test_info[i].testnum == 0)
    {
        DLOG_Critical("###test pass !!!###");
        Autotest_status = 0x55;
        set_led(RED,OFF);
        set_led(BLUE,ON);

    }

    HAL_Delay(10);

    vTaskDelete(NULL);
}

void c201d_pt(int enable)
{
    if(enable)
    {
        osThreadDef(COMTASK_PT, c201d_auto_test, osPriorityHigh, 0, 8 * 128);
        osThreadCreate(osThread(COMTASK_PT), NULL);
    }
}

int get_autotest_status()
{
 return  Autotest_status;
}

