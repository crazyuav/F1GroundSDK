#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bb_reply_pc.h"
#include "hal_usb_host.h"
#include "hal_usb_device.h"
#include "debuglog.h"
#include "hal.h"
#include "bb_types.h"

static uint8_t cmd[16];

void BB_ReplyPcHandler(void *p)
{
    STRU_REG_FLAG *pflag = (STRU_REG_FLAG *)(p);
    STRU_WIRELESS_MSG_HEADER *pMsgHeader = (STRU_WIRELESS_MSG_HEADER *)cmd;
    uint8_t       *pData; 
    uint8_t        i;

    if(pflag->dir)
    {
        pData = cmd + sizeof(STRU_WIRELESS_MSG_HEADER);

        memcpy(pData, (uint8_t *)p, 4);

        pMsgHeader->magic_header[0] = 0xFF;
        pMsgHeader->magic_header[1] = 0x5A;
        pMsgHeader->msg_id          = 0x4D;
        pMsgHeader->packet_num      = 1;
        pMsgHeader->packet_cur      = 0;
        pMsgHeader->msg_len         = 4;
        pMsgHeader->chk_sum         = 0;

        for (i = 0; i < 4; i++)
        {
            pMsgHeader->chk_sum += pData[i];
        }

        if(HAL_OK == HAL_USB_DeviceGetConnState(0))
        {
            HAL_USB_SendData(cmd, sizeof(cmd), 0, 0x84);
        }
        
        if(HAL_OK == HAL_USB_DeviceGetConnState(1))
        {
            HAL_USB_SendData(cmd, sizeof(cmd), 1, 0x84);
        }
#if 0        
        STRU_RF_REG_CMD *pRfCmd = (STRU_RF_REG_CMD *)(p);
        STRU_BB_REG_CMD *pBbCmd = (STRU_BB_REG_CMD *)(p);
        if(pflag->type) //rf
        {
            DLOG_Warning("flag:%x subType:%d rsv:%d rfCh:%d type:%d dir:%d addr_h:%x addr_l:%x value:%x",\
                        pRfCmd->flag,
                        pRfCmd->flag.subType,
                        pRfCmd->flag.rsv,
                        pRfCmd->flag.rfCh,
                        pRfCmd->flag.type,
                        pRfCmd->flag.dir,
                        pRfCmd->addr_h,
                        pRfCmd->addr_l,
                        pRfCmd->value);
        }
        else
        {
            DLOG_Warning("flag:%x subType:%d rsv:%d rfCh:%d type:%d dir:%d page:%x addr:%x value:%x",
                                    pBbCmd->flag,
                                    pBbCmd->flag.subType,
                                    pBbCmd->flag.rsv,
                                    pBbCmd->flag.rfCh,
                                    pBbCmd->flag.type,
                                    pBbCmd->flag.dir,
                                    pBbCmd->page,
                                    pBbCmd->addr,
                                    pBbCmd->value);
        }
#endif
    }
}

