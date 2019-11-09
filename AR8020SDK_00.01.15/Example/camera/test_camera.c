#include "isp_xc6130/xc6130.h"
#include "gc2145/gc2145.h"
#include "sc2143/sc2143.h"
#include "ov5648/ov5648.h"
#include "hal_i2c.h"
#include "i2c.h"
#include "hal_gpio.h"
#include "hal.h"
#include "hal_dvp.h"
#include "test_hal_mipi.h"
//#include "i2c_ll.h"



void command_TestGc2145Init(void)
{
    HAL_I2C_MasterInit(GC2145_COMPONENT, GC2145_I2C_ADDR, HAL_I2C_FAST_SPEED);

    GC2145_PinSet();
    
    DLOG_Warning("init finished");
}

void command_TestGC2145DVP(void)
{
    GC2145_Init(0);
}

void command_TestDVP2Encoder(char *ch)
{
    uint8_t tmpCh = strtoul(ch, NULL, 0);
    
    HAL_DVP_SendInfoToEncoder(tmpCh);
    DLOG_Warning("ch:%d", tmpCh);
}


void command_TestGC2145MIPI_1Lane(void)
{
    GC2145_Init(1);
}

void command_TestGC2145MIPI_1Lane_SVGA(void)
{
    GC2145_Init(2);
}

void command_TestGC2145MIPI_2Lane(void)
{
    GC2145_Init(3);
}

void command_TestGC2145MIPI_2Lane_SVGA(void)
{
    GC2145_Init(4);
}


void command_TestXc6130Write(unsigned char *subAddr, unsigned char *value)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t val = strtoul(value, NULL, 0);
    uint32_t delay = 200;
    uint8_t u8_val;

    XC6130_ReadReg(reg, &u8_val);
    DLOG_Warning("before write red:0x%x val:0x%x",reg,u8_val);
    
    XC6130_WriteReg(reg, val);
        
    XC6130_ReadReg(reg, &u8_val);
    DLOG_Warning("after write red:0x%x val:0x%x",reg,u8_val);
}

void command_TestXc6130Read(unsigned char *subAddr)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t u8_val;
        
    XC6130_ReadReg(reg, &u8_val);
    DLOG_Warning("red:0x%x val:0x%x",reg,u8_val);

}


void command_TestSc2143Write(unsigned char *subAddr, unsigned char *value)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t val = strtoul(value, NULL, 0);
    uint32_t delay = 200;
    uint8_t u8_val;

    SC2143_ReadReg(reg, &u8_val);
    DLOG_Warning("before write red:0x%x val:0x%x",reg,u8_val);
    
    SC2143_WriteReg(reg, val);
        
    SC2143_ReadReg(reg, &u8_val);
    DLOG_Warning("after write red:0x%x val:0x%x",reg,u8_val);
}

void command_TestSc2143Read(unsigned char *subAddr)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t u8_val;
        
    SC2143_ReadReg(reg, &u8_val);
    DLOG_Warning("red:0x%x val:0x%x",reg,u8_val);

}

void command_TestOv5648Write(unsigned char *subAddr, unsigned char *value)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t val = strtoul(value, NULL, 0);
    uint32_t delay = 200;
    uint8_t u8_val;

    OV5648_ReadReg(reg, &u8_val);
    DLOG_Warning("before write red:0x%x val:0x%x",reg,u8_val);
    
    OV5648_WriteReg(reg, val);
        
    OV5648_ReadReg(reg, &u8_val);
    DLOG_Warning("after write red:0x%x val:0x%x",reg,u8_val);
}

void command_TestOv5648Read(unsigned char *subAddr)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t u8_val;
        
    OV5648_ReadReg(reg, &u8_val);
    DLOG_Warning("red:0x%x val:0x%x",reg,u8_val);

}


void command_TestGc2145Write(unsigned char *subAddr, unsigned char *value)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t val = strtoul(value, NULL, 0);
    uint32_t delay = 200;
    uint8_t u8_val;

    //GC2145_ReadReg(reg, &u8_val);
    //DLOG_Warning("before write red:0x%x val:0x%x",reg,u8_val);
    
    GC2145_WriteReg(reg, val);
        
    GC2145_ReadReg(reg, &u8_val);
    DLOG_Warning("after write red:0x%x val:0x%x",reg,u8_val);
}

void command_TestGc145Read(unsigned char *subAddr)
{
    uint16_t reg = strtoul(subAddr, NULL, 0);
    uint8_t u8_val;
        
    GC2145_ReadReg(reg, &u8_val);
    DLOG_Warning("red:0x%x val:0x%x",reg,u8_val);

}

void command_TestXc6130Sc2143Init(void)
{
    XC6130_SC2143_PinSet();
    
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);


    XC6130_ReadChipId();
    
    XC6130_Init(0);

    XC6130_I2cByPassOn();
   
    HAL_I2C_MasterInit(SC2143_COMPONENT, SC2143_I2C_ADDR, HAL_I2C_FAST_SPEED);

    SC2143_ReadChipId();
    
    SC2143_Init(0);

    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    
    XC6130_I2cByPassOff();

    Xc6130MipiInit();
}

void command_TestXc6130Sc2143_1080p30fps_600fov(void)
{
    XC6130_SC2143_PinSet();
    
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);


    XC6130_ReadChipId();
    
    XC6130_Init(4);

    XC6130_I2cByPassOn();
   
    HAL_I2C_MasterInit(SC2143_COMPONENT, SC2143_I2C_ADDR, HAL_I2C_FAST_SPEED);

    SC2143_ReadChipId();
    
    SC2143_Init(2);
     
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    
    XC6130_I2cByPassOff();

    Xc6130MipiInit();
}

void command_TestXc6130Sc2143_1080p30fps_800fov(void)
{
    XC6130_SC2143_PinSet();
    
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);


    XC6130_ReadChipId();
    
    XC6130_Init(2);

    XC6130_I2cByPassOn();
   
    HAL_I2C_MasterInit(SC2143_COMPONENT, SC2143_I2C_ADDR, HAL_I2C_FAST_SPEED);

    SC2143_ReadChipId();
    
    SC2143_Init(1);
     
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    
    XC6130_I2cByPassOff();

    Xc6130MipiInit();
}

void command_TestXc6130Sc2143_1080p30fps_1200fov(void)
{
    XC6130_SC2143_PinSet();
    
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);


    XC6130_ReadChipId();
    
    XC6130_Init(3);

    XC6130_I2cByPassOn();
   
    HAL_I2C_MasterInit(SC2143_COMPONENT, SC2143_I2C_ADDR, HAL_I2C_FAST_SPEED);

    SC2143_ReadChipId();
    
    SC2143_Init(1);
     
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    
    XC6130_I2cByPassOff();

    Xc6130MipiInit();
}

void command_SwitchI2CToXc6130(void)
{
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    XC6130_I2cByPassOff();
    DLOG_Warning("SwitchI2CToXc6130...");
}

void command_SwitchI2CToSc2143(void)
{
    HAL_I2C_MasterInit(SC2143_COMPONENT, SC2143_I2C_ADDR, HAL_I2C_FAST_SPEED);
    XC6130_I2cByPassOn();
    DLOG_Warning("SwitchI2CToSc2143...");
}
void command_SwitchI2CToOv5648(void)
{
    HAL_I2C_MasterInit(OV5648_COMPONENT, OV5648_I2C_ADDR, HAL_I2C_FAST_SPEED);
    XC6130_I2cByPassOn();
    DLOG_Warning("SwitchI2CToOv5648...");
}


void command_TestXc6130Ov5648Init(void)
{
    XC6130_OV5648_PinSet();
    
    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);

    XC6130_ReadChipId();
    
    XC6130_Init(1);

    XC6130_I2cByPassOn();
   
    HAL_I2C_MasterInit(OV5648_COMPONENT, OV5648_I2C_ADDR, HAL_I2C_FAST_SPEED);

    OV5648_ReadChipId();
    
    OV5648_Init(0);

    HAL_I2C_MasterInit(XC6130_COMPONENT, XC6130_I2C_ADDR, HAL_I2C_FAST_SPEED);
    
    XC6130_I2cByPassOff();

    Xc6130MipiInit();
}


void Camera_Task(void const *argument)
{  

    HAL_GPIO_SetMode(HAL_GPIO_NUM50, HAL_GPIO_PIN_MODE2);
    HAL_GPIO_OutPut(HAL_GPIO_NUM50);
    HAL_GPIO_SetPin(HAL_GPIO_NUM50, HAL_GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_SetPin(HAL_GPIO_NUM50, HAL_GPIO_PIN_RESET);  
    HAL_Delay(1);
    HAL_GPIO_SetPin(HAL_GPIO_NUM50, HAL_GPIO_PIN_SET);


    HAL_Delay(3 * 1000);

 //   command_TestHalMipiInit("test_hal_mipi_init", "1920", "1080", "30");
 //   HAL_Delay(2);
 //   command_TestXc6130Ov5648Init();
 //   HAL_Delay(2);
 //   *((volatile uint32_t*)0xa0010000) = 0x01d40736;
 
    command_TestXc6130Sc2143_1080p30fps_1200fov();
    HAL_Delay(2);
    command_TestHalMipiInit("test_hal_mipi_init", "1920", "1080", "30");
    HAL_Delay(2);
    *((volatile uint32_t*)0xa0010000) = 0x01d40736;
    *((volatile uint32_t*)0xa0010010) = 0xf0006000;

    while (1)
    {
        HAL_Delay(1000);
    }
}

