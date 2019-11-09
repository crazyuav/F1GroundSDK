#ifndef __HAL_TEST_BRINGUP_H__
#define __HAL_TEST_BRINGUP_H__

#ifdef __cplusplus
extern "C"
{
#endif


    void bup_sys_init(char num);
    void bup_gpio_test(char num);
    void bup_i2c_test(char num);
    void bup_pwm_test(char num);
    void bup_usb0_test(char num);
    void bup_usb1_test(char num);
    void bup_uart_test(char num);
    void bup_adc_test(char num);


    void command_test_pin_pair(void);


#ifdef __cplusplus
}
#endif

#endif

