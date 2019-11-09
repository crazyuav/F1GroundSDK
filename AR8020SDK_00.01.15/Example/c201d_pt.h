#ifndef __HAL_TEST_BRINGUP_H__
#define __HAL_TEST_BRINGUP_H__

#ifdef __cplusplus
extern "C"
{
#endif
int is_test_mode(void);
void c201d_pt(int enable);
int get_autotest_status();
#ifdef __cplusplus
}
#endif

#endif

