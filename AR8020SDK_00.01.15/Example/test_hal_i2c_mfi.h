
#ifndef __TEST_HAL_I2C_MFI__
#define __TEST_HAL_I2C_MFI__

#ifdef __cplusplus
extern "C"
{
#endif

void command_TestHalI2cInitMfi(uint8_t *ch, uint8_t *i2c_addr, uint8_t *speed);

void command_TestHalI2cWriteMfi(uint8_t *ch, uint8_t *subAddr, uint8_t *subAddrLen, uint8_t *data, uint8_t *dataLen);

void command_TestHalI2cReadMfi(uint8_t *ch, uint8_t *subAddr, uint8_t *subAddrLen, uint8_t *dataLen);

#ifdef __cplusplus
}
#endif

#endif