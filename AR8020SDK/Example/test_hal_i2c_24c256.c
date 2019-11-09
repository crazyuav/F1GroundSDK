#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "hal_ret_type.h"
#include "hal_i2c.h"
#include "test_hal_i2c_24c256.h"
#include "debuglog.h"
#include "hal.h"
#include "systicks.h"
#include "auto_test.h"

#define I2C_24C256_TIMEOUT_VALUE    (10)
#define TEST_24C256_BUFFSIZE 64

static void testhal_24c256(unsigned char i2c_num,unsigned char value);

void testhal_24c256(unsigned char i2c_num,unsigned char value)
{
    uint8_t result = 1;
    uint64_t t_diff[3] = {0, 0, 0};
    

    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterInit(i2c_num, TAR_24C256_ADDR, HAL_I2C_FAST_SPEED);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    
    DLOG_Info("i2c_num %d I2C_Initial finished!\n", i2c_num);
    unsigned char i=0;
    unsigned char data_src1[TEST_24C256_BUFFSIZE+2] = {0x00,0x00};
    unsigned char data_chk[TEST_24C256_BUFFSIZE];
    unsigned short rd_start_addr = 0;
    
    for (i = 0; i< TEST_24C256_BUFFSIZE; i++)
    {
        data_src1[i+2] = value + i;
    }

    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterWriteData(i2c_num, TAR_24C256_ADDR, data_src1, 66, I2C_24C256_TIMEOUT_VALUE);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    
    DLOG_Info("I2C_Master_Write_Data finished!\n");
    HAL_Delay(200);
    memset((void *)data_chk, 0, sizeof(data_chk));

    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterReadData(i2c_num, TAR_24C256_ADDR, data_src1, 2, data_chk, TEST_24C256_BUFFSIZE, I2C_24C256_TIMEOUT_VALUE);
    t_diff[2] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    for(i = 0; i < TEST_24C256_BUFFSIZE; i++)
    {
        if (data_src1[i+2] != data_chk[i])
        {
            result = 0;
            break;
        }
    }

    DLOG_Info("I2C_Master_Read_Data finished!\n");

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterInit", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterWriteData", (uint32_t)t_diff[1]);
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterReadData", (uint32_t)t_diff[2]);
    DIV_LINE(DIV_LINE_END);
}

void commandhal_Test24C256(char* i2c_num_str,char* i2c_value)
{
    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, i2c_num_str, i2c_value);
    testhal_24c256(strtoul(i2c_num_str, NULL, 0),strtoul(i2c_value, NULL, 0));
}

void commandhal_TestI2cAsyn24C256(char* i2c_num_str,char* i2c_value)
{
    uint8_t result = 1;
    uint64_t t_diff[3] = {0, 0, 0};
    unsigned char i2c_num = strtoul(i2c_num_str, NULL, 0);
    unsigned char value = strtoul(i2c_value, NULL, 0);
    
    DLOG_Critical("auto_test>%s %s %s", __FUNCTION__, i2c_num_str, i2c_value);
    
    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterInit(i2c_num, TAR_24C256_ADDR, HAL_I2C_FAST_SPEED);
    t_diff[0] = (uint32_t)AUTO_TEST_CalcTimeDiff();
    
    //DLOG_Info("i2c_num %d I2C_Initial finished!\n", i2c_num);
    unsigned char i=0;
    unsigned char data_src1[TEST_24C256_BUFFSIZE+2] = {0x00,0x00};
    unsigned char data_src2[TEST_24C256_BUFFSIZE+2] = {0x00,0x00};
    unsigned char data_chk[TEST_24C256_BUFFSIZE];
    unsigned short rd_start_addr = 0;
    
    for (i = 0; i< TEST_24C256_BUFFSIZE; i++)
    {
        data_src1[i+2] = value + i;
    }

    DLOG_Info("write the fisrt data!\n");
    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterWriteData(i2c_num, TAR_24C256_ADDR, data_src1, 66, 0);
    t_diff[1] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    memset(&data_src2[2], 0x55, TEST_24C256_BUFFSIZE);
    DLOG_Info("write the second data!\n");
    HAL_RET_T ret = HAL_I2C_MasterWriteData(i2c_num, TAR_24C256_ADDR, data_src2, 66, 0);
    if(HAL_BUSY == ret)
    {
        DLOG_Info("***i2c %d is busy.", i2c_num);
    }
    HAL_Delay(200);
    memset((void *)data_chk, 0, sizeof(data_chk));

    AUTO_TEST_TimeRecord();
    HAL_I2C_MasterReadData(i2c_num, TAR_24C256_ADDR, data_src1, 2, data_chk, TEST_24C256_BUFFSIZE, I2C_24C256_TIMEOUT_VALUE);
    t_diff[2] = (uint32_t)AUTO_TEST_CalcTimeDiff();

    for(i = 0; i < TEST_24C256_BUFFSIZE; i++)
    {
        if (data_src1[i+2] != data_chk[i])
        {
            result = 0;
            break;
        }
    }

    DIV_LINE(DIV_LINE_REPORT);
    if (0 == result)
    {
        AUTO_TEST_PrintResult(FAILURE);
    }
    else
    {
        AUTO_TEST_PrintResult(PASSED);
    }
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterInit", (uint32_t)t_diff[0]);
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterWriteData", (uint32_t)t_diff[1]);
    AUTO_TEST_PrintApiRunTime("HAL_I2C_MasterReadData", (uint32_t)t_diff[2]);
    DIV_LINE(DIV_LINE_END);
}
