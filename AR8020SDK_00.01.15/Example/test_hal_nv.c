#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "hal_nv.h"
#include "test_hal_nv.h"
#include "hal_bb.h"
#include "nor_flash.h"
#include "memory_config.h"
#include "hal.h"


void command_TestNvResetBbRcId(void)
{
    HAL_NV_ResetBbRcId();
}

void command_TestNvSetBbRcId(uint8_t *id1, uint8_t *id2, uint8_t *id3, uint8_t *id4, uint8_t *id5, 
                                    uint8_t *vt_id0, uint8_t *vt_id1)
{
    uint8_t idArr[5];
    uint8_t vtidArr[2];

    idArr[0] = (uint8_t)(strtoul(id1, NULL, 0));
    idArr[1] = (uint8_t)(strtoul(id2, NULL, 0));
    idArr[2] = (uint8_t)(strtoul(id3, NULL, 0));
    idArr[3] = (uint8_t)(strtoul(id4, NULL, 0));
    idArr[4] = (uint8_t)(strtoul(id5, NULL, 0));

    vtidArr[0] = (uint8_t)(strtoul(vt_id0, NULL, 0));
    vtidArr[1] = (uint8_t)(strtoul(vt_id1, NULL, 0));

    HAL_BB_SaveRcId(idArr, vtidArr);

    DLOG_Info("id:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",idArr[0], idArr[1], idArr[2], idArr[3], idArr[4], vtidArr[0], vtidArr[1]);
}

void command_TestNvSetChipId(uint8_t *id1, uint8_t *id2, uint8_t *id3, uint8_t *id4, uint8_t *id5)
{
    uint8_t idArr[5];

    idArr[0] = (uint8_t)(strtoul(id1, NULL, 0));
    idArr[1] = (uint8_t)(strtoul(id2, NULL, 0));
    idArr[2] = (uint8_t)(strtoul(id3, NULL, 0));
    idArr[3] = (uint8_t)(strtoul(id4, NULL, 0));
    idArr[4] = (uint8_t)(strtoul(id5, NULL, 0));

    HAL_NV_SaveChipId(idArr);

    DLOG_Info("chip id:0x%x 0x%x 0x%x 0x%x 0x%x",idArr[0], idArr[1], idArr[2], idArr[3], idArr[4]);
}



#define LEN_PER_WR       (4567)
#define MAX_LEN          (60*1024)

static uint8_t wData[LEN_PER_WR];

void command_TestUsrDataWrite(void)
{
    uint32_t wLen = 0;
    uint32_t i;
    uint32_t cnt = 0;

    for (i = 0; i < LEN_PER_WR; i++)
    {
        wData[i] = i;
    }

    while((wLen + LEN_PER_WR) < MAX_LEN)
    {
        HAL_NV_UsrDataWrite(wLen, wData, LEN_PER_WR);
        DLOG_Info("%d:\t0x%x", cnt++, wLen);
        wLen += LEN_PER_WR;
    }

    DLOG_Info("finished %d", (wLen - LEN_PER_WR));
}

void command_TestUsrDataRead(void)
{
    uint8_t rData[LEN_PER_WR];
    uint32_t rLen = 0;
    uint32_t i;
    uint32_t cnt = 0;

    for (i = 0; i < LEN_PER_WR; i++)
    {
        wData[i] = i;
    }

    while((rLen + LEN_PER_WR) < MAX_LEN)
    {
        memset(rData, 0x00, LEN_PER_WR);
        if (HAL_NV_ERR == HAL_NV_UsrDataRead(rLen, rData, LEN_PER_WR))

        {
            DLOG_Info("read err %d:\t0x%x", cnt, rLen);
            break;
        }
        if (0 != memcmp(wData, rData, LEN_PER_WR))
        {
            DLOG_Info("cmp err %d:\t0x%x", cnt, rLen);
            break;
        }
        DLOG_Info("%d:\t0x%x", cnt++, rLen);
        rLen += LEN_PER_WR;
        HAL_Delay(500);
    }

    DLOG_Info("finished %d", (rLen - LEN_PER_WR));
}


void command_TestUsrDataDestroy(uint8_t *addr)
{
    uint8_t data[1024];
    uint32_t i;
    uint32_t offsetAddr = strtoul(addr, NULL, 0);

    DLOG_Info("baseAddr:0x%x offsetAddr:0x%x", NV_FLASH_USR_ADDR1, offsetAddr);

    memset(data, 0x12, sizeof(data));
    
    NOR_FLASH_WriteByteBuffer(NV_FLASH_USR_ADDR1 + offsetAddr, data, sizeof(data));
}

