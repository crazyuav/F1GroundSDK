/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: testhal_gpio.h
Description: test gpio
Author: SW
Version: 1.0
Date: 2016/12/19
History: test gpio
*****************************************************************************/
#ifndef __HAL_TEST_GPIO_H__
#define __HAL_TEST_GPIO_H__

#ifdef __cplusplus
extern "C"
{
#endif


void commandhal_TestGpioNormal(uint8_t *gpionum, uint8_t *highorlow);

void commandhal_TestGpioInterrupt(uint8_t *gpionum, uint8_t *inttype, uint8_t *polarity);

void commandhal_TestGetGpio(uint8_t *gpionum);

void commandhal_TestGpioAndPwm(uint8_t *gpionum, uint8_t *inttype, uint8_t *polarity);

void commandhal_TestSetGpio(uint8_t *gpionum, uint8_t *val);

#ifdef __cplusplus
}
#endif

#endif
