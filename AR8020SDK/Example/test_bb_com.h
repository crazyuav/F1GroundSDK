#ifndef __TEST_BB_COM_H__
#define __TEST_BB_COM_H__

void command_TestRcv(void const *argument);

void command_TestRevRT(void const *argument);

void command_TestRcvRate(void const *argument);

void command_TestComTask(void const *argument);

void command_TestRT(void const *argument);

void command_TestHead(void const *argument);

void command_TestPeriodHead(void const *argument);

void command_TestRate(void const *cycle);

void command_TestSpiSkyInit(unsigned char *maxLen);

void command_TestSpiGrdInit(unsigned char *maxLen);

void command_TestSpiGrdSend(unsigned char *start_value, unsigned char *len);

void command_TestComInitRcv(unsigned char * initflag, unsigned char *initlen);

void command_TestComInitSend(unsigned char * initflag, unsigned char *initlen);



#endif
