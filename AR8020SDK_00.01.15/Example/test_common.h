#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void command_readMemory(char *addr);

void command_writeMemory(char *addr, char *value);

void command_set_loglevel(char* cpu, char* loglevel);

void command_getVersion(void);


#ifdef __cplusplus
}
#endif

#endif
