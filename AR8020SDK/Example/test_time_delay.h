/*****************************************************************************

*****************************************************************************/
#ifndef __TEST_TIME_DELAY_H__
#define __TEST_TIME_DELAY_H__

#ifdef __cplusplus
extern "C"
{
#endif


void command_TestUsDelayPrecision(char *cnt, char *us, char *gpio);

void command_TestUsTick(char *cnt, char *us, char *gpio);


#ifdef __cplusplus
}
#endif

#endif
