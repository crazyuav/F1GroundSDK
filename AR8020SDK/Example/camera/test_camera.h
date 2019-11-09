#ifndef __TEST_XC6130_H__
#define	__TEST_XC6130_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdlib.h>
#include <string.h>
#include "debuglog.h"

void command_TestXc6130Sc2143Init(void);
void command_TestXc6130Sc2143_1080p30fps_600fov(void);
void command_TestXc6130Sc2143_1080p30fps_800fov(void);
void command_TestXc6130Sc2143_1080p30fps_1200fov(void);
void command_TestXc6130Write(unsigned char *subAddr, unsigned char *value);
void command_TestXc6130Read(unsigned char *subAddr);

void command_TestSc2143Write(unsigned char *subAddr, unsigned char *value);
void command_TestSc2143Read(unsigned char *subAddr);


void command_SwitchI2CToXc6130(void);
void command_SwitchI2CToSc2143(void);
void command_SwitchI2CToOv5648(void);


void command_TestGc2145Init(void);
void command_TestGC2145DVP(void);
void command_TestGC2145MIPI_1Lane(void);
void command_TestGC2145MIPI_1Lane_SVGA(void);
void command_TestGC2145MIPI_2Lane(void);
void command_TestGC2145MIPI_2Lane_SVGA(void);
void command_TestGc2145Write(unsigned char *subAddr, unsigned char *value);
void command_TestGc145Read(unsigned char *subAddr);
void command_TestDVP2Encoder(char *ch);

void command_TestXc6130Ov5648Init(void);
void command_TestOv5648Write(unsigned char *subAddr, unsigned char *value);
void command_TestOv5648Read(unsigned char *subAddr);

void Camera_Task(void const *argument);


#ifdef __cplusplus
}
#endif 

#endif
