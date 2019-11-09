#include <stdlib.h>
#include "debuglog.h"
#include "hal.h"
#include "systicks.h"
#include "app_sys_event_process.h"
#include "wireless_interface.h"
#include "test_search_id.h"
#include "bb_types.h"
#include "factory.h"

void Grd_AppSysEventHandler(void *p)
{
    STRU_WIRELESS_MSG_HEADER       *msgHeader;
    uint8_t                        *msgContent;

    msgHeader   = (STRU_WIRELESS_MSG_HEADER *)p;
    msgContent  = (uint8_t *)p;
    msgContent += sizeof(STRU_WIRELESS_MSG_HEADER);

    switch((WIRELESS_INTRTFACE_PID_DEF)(msgHeader->msg_id))
    {
        case WIRELESS_INTERFACE_ENABLE_FREQUENCY:
        {
            BB_Grd_SearchIdHandler(p);
            break;
        }
        case WIRELESS_INTERFACE_FCT_RESET:
        {
            FCT_Reset();
            break;
        }
        default:
        {
            DLOG_Warning("error pid %d", msgHeader->msg_id);
            break;
        }
    }
}

