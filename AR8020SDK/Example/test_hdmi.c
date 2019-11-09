#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "hal_hdmi_rx.h"
#include "sys_event.h"

void HDMI_RX_VideoCallbakSample(void *p)
{
    DLOG_Info("video width=%d hight=%d framerate=%d", ((STRU_SysEvent_H264InputFormatChangeParameter*)p)->width, 
                                                      ((STRU_SysEvent_H264InputFormatChangeParameter*)p)->hight, 
                                                      ((STRU_SysEvent_H264InputFormatChangeParameter*)p)->framerate);

    // notfiy encoder
    HDMI_RxVideoNotifyH264Endcoder(p);
}

