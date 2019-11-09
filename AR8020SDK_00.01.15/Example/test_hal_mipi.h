#ifndef __TEST_HAL_MIPI_H__
#define __TEST_HAL_MIPI_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void command_TestHalMipiInit(char *toEncoderCh, char *width, char *hight, char *frameRate);

void command_TestSetMipiLane(char *lane);


#ifdef __cplusplus
}
#endif 

#endif
