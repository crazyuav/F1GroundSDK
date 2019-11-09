#ifndef TEST_SYSEVENT_H
#define TEST_SYSEVENT_H

#ifdef __cplusplus
extern "C"
{
#endif


void command_TestSysEventIdle(void);
void command_TestSysEventInterCore(char* ptr);
void command_TestRemoteEventInit(void);
void command_TestRemoteEvent(void);
void command_TestLoadPerformance(char * delayMs, char * totalNum);


#ifdef __cplusplus
}
#endif 

#endif
