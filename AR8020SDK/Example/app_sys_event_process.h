#ifndef __APP_SYS_EVENT_PROCESS_H__
#define __APP_SYS_EVENT_PROCESS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "debuglog.h"

void Sky_AppSysEventHandler(void *p);

void Grd_AppSysEventHandler(void *p);

#ifdef __cplusplus
}
#endif 

#endif
