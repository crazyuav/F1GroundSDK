#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "debuglog.h"
#include "hal_gpio.h"
#include "systicks.h"
#include "hal_spi.h"
#include "C201D_bringup.h"

/*
系统程序结构
*/
typedef void (*fun)(char);

typedef struct _TEST_INFO{

	char testnum;
	fun testfun;
	char *name;
	char flag;
	char result;

}TEST_INFO;


TEST_INFO test_info[] = {
    
    {1, bup_sys_init,   "sys_init",     0,  0},
    {2, bup_gpio_test,  "gpio test",    0,  0},
    {3, bup_i2c_test,   "i2c test",     0,  0},
    {4, bup_pwm_test,   "pwm test",     0,  0},     
    {5, bup_usb0_test,  "usb0 test",    0,  0},
    {6, bup_usb1_test,  "usb1 test",    0,  0},
    {7, bup_uart_test,  "uart test",    0,  0},
    {8, bup_adc_test,   "adc_test",     0,  0},
    {0, 0,                "finished",     0,  0}

};

void bup_sys_init(char num)
{
    DLOG_Critical("now is in sys_init");
}

void bup_gpio_test(char num)
{
    DLOG_Critical("now is in gpio_test");
}

void bup_i2c_test(char num)
{
    DLOG_Critical("now is in i2c_test");
}

void bup_pwm_test(char num)
{
    DLOG_Critical("now is in i2c_test");
}

void bup_usb0_test(char num)
{
    DLOG_Critical("now is in usb0_test");
}

void bup_usb1_test(char num)
{
    DLOG_Critical("now is in usb1_test");
}

void bup_uart_test(char num)
{
    DLOG_Critical("now is in uart_test");
}

void bup_adc_test(char num)
{
    DLOG_Critical("now is adc_test");
}


/*

*/

/*
 * gpio test
 */
 
//pair test
uint8_t test_pinA[] = {0, 1,5,2, 4,97};
uint8_t test_pinB[] = {34,3,6,32,7,33};
void command_test_pin_pair(void)
{
    uint8_t i;
    uint32_t val;

    test_info[0].testfun(0);

    DLOG_Critical("Item:");

    for(i = 1; test_info[i].testnum; i++)
    {
        DLOG_Critical("%s",i,test_info[i].name);
    }

    for(i = 1; test_info[i].testnum; i++)
        test_info[i].testfun(i);

    return;
    
    for(i=0;i<sizeof(test_pinA);i++)
    {
	
        HAL_GPIO_InPut(test_pinB[i]);

        SysTicks_DelayMS(2);

        HAL_GPIO_OutPut(test_pinA[i]);
        HAL_GPIO_SetPin(test_pinA[i], 1); 

        SysTicks_DelayMS(2);
        
        val = 0;
        HAL_GPIO_GetPin(test_pinB[i],&val);

        if(val != 1)
        {
            DLOG_Critical("failed pin%d write 1 , pin%d read %d",test_pinA[i],test_pinB[i],val);
            continue;
        }

        HAL_GPIO_SetPin(test_pinA[i], 0); 
        
        SysTicks_DelayMS(2);
        
        val = 1;
        HAL_GPIO_GetPin(test_pinB[i],&val);
        if(val != 0)
        {
            DLOG_Critical("failed pin%d write 0 , pin%d read %d",test_pinA[i],test_pinB[i],val);
            continue;
        }

        DLOG_Critical("pin%d write pin%d read pass!!!",test_pinA[i],test_pinB[i]);
        

    }

    for(i=0;i<sizeof(test_pinB);i++)
    {
        HAL_GPIO_InPut(test_pinA[i]);

        SysTicks_DelayMS(2);

        HAL_GPIO_OutPut(test_pinB[i]);
        HAL_GPIO_SetPin(test_pinB[i], 1); 

        SysTicks_DelayMS(2);

        HAL_GPIO_InPut(test_pinA[i]);
        val = 0;
        HAL_GPIO_GetPin(test_pinA[i],&val);

        if(val != 1)
        {
            DLOG_Critical("failed pin%d write 1 , pin%d read %d",test_pinB[i],test_pinA[i],val);
            continue;
        }

        HAL_GPIO_SetPin(test_pinB[i], 0); 
        
        SysTicks_DelayMS(2);
        
        val = 1;
        HAL_GPIO_GetPin(test_pinA[i],&val);
        if(val != 0)
        {
            DLOG_Critical("failed pin%d write 0 , pin%d read %d",test_pinB[i],test_pinA[i],val);
            continue;
        }

        DLOG_Critical("pin%d write pin%d read pass!!!",test_pinB[i],test_pinA[i]);

    }

}

/*
 * flash test
 */
 
#define W25Q128_ID  0xEF17
void test_spi_init(uint8_t index)
{
    STRU_HAL_SPI_INIT st_spiInitInfo = 
    {
        .u16_halSpiBaudr = 6,
        .e_halSpiPolarity = HAL_SPI_POLARITY_HIGH,
        .e_halSpiPhase = HAL_SPI_PHASE_2EDGE,
    };
    HAL_SPI_MasterInit(index, &st_spiInitInfo);
}

static uint16_t get_flash_id(uint8_t spi_base_addr)
{
    uint16_t device_id = 0;
    uint8_t date[] = {0x90, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buf[6]; 

    HAL_SPI_MasterWriteRead(spi_base_addr,date, 4,buf, 2, 5);  
    
    device_id |= buf[0]<< 8; 
    device_id |= buf[1];

    return device_id ;
}

void command_spi0(void)
{

    uint16_t flash_id = 0;
    
    test_spi_init(0);

    flash_id = get_flash_id(0);
    
    if(W25Q128_ID == flash_id)
    {
        DLOG_Critical("spi ok");
    }
    else
    {
        DLOG_Critical("spi err");
    }

}

/*
 * I2c
 */



