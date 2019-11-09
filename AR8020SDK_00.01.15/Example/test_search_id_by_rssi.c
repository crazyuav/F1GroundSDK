/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: test_search_id
Description: 
Author: Artosy Software Team
Version: 0.0.1
Date: 2017/12/14
History:
         0.0.1    2017/12/14    test_search_id
*****************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debuglog.h"
#include "hal_ret_type.h"
#include "hal_timer.h"
#include "hal_nv.h"
#include "sys_event.h"
#include "hal_bb.h"
#include "data_type.h"
#include "test_search_id.h"
#include "hal_gpio.h"
#include "hal.h"


#define MAX_SEARCH_DEV_ID_NUM           (8)
#define DEV_ID_LEN                      (7)

#define MIN(a,b) (((a) > (b)) ?  (b) :  (a) )
#define MAX(a,b) (((a) > (b)) ?  (a) :  (b) )

typedef struct
{
    uint8_t  id[DEV_ID_LEN];//head 2 byte vt id, tail 5 byte rc id
    uint16_t  rssi_a;
    uint16_t  rssi_b;
} STRU_ID_AND_RSSI;

typedef struct
{
    STRU_ID_AND_RSSI dev_info[MAX_SEARCH_DEV_ID_NUM];
    uint8_t index;
}STRU_DEV_INFO_LIST;

static STRU_DEV_INFO_LIST st_DevInfoList;

void print_dev_list_info(void)
{
    uint8_t i;

    for(i=0;i<st_DevInfoList.index;i++)
    {
        DLOG_Warning("vt:%02x,%02x,rc:%02x %02x %02x %02x %02x,rssi %d,%d", 
            st_DevInfoList.dev_info[i].id[0], st_DevInfoList.dev_info[i].id[1], 
            st_DevInfoList.dev_info[i].id[2], st_DevInfoList.dev_info[i].id[3], 
            st_DevInfoList.dev_info[i].id[4], st_DevInfoList.dev_info[i].id[5],
            st_DevInfoList.dev_info[i].id[6],
            st_DevInfoList.dev_info[i].rssi_a,st_DevInfoList.dev_info[i].rssi_b);

    }
}

uint8_t find_farthest_dev(void)
{
    uint8_t i;
    uint8_t rssi,farthest_dev;

    rssi = 0;
    farthest_dev = 0;
    for(i=0;i<MAX_SEARCH_DEV_ID_NUM;i++)
    {
        uint8_t avg_rssi;
        avg_rssi = (st_DevInfoList.dev_info[i].rssi_a + st_DevInfoList.dev_info[i].rssi_b) / 2;
        if(avg_rssi > rssi)
        {
            rssi = avg_rssi;
            farthest_dev = i;
        }
    }

    return farthest_dev;
}

uint8_t select_nearest_dev(void)
{
    uint8_t i,rssi,nearest_dev;

    nearest_dev = 0;
    
    if(st_DevInfoList.index == 1)
    {
        return nearest_dev;
    }

    rssi = MIN(st_DevInfoList.dev_info[nearest_dev].rssi_a,st_DevInfoList.dev_info[nearest_dev].rssi_b);

    for(i=1;i<st_DevInfoList.index;i++)
    {
        uint8_t min_rssi;
        min_rssi = MIN(st_DevInfoList.dev_info[i].rssi_a,st_DevInfoList.dev_info[i].rssi_b);
        if(min_rssi < rssi)
        {
            rssi = min_rssi;
            nearest_dev = i;
        }
    }

    return nearest_dev;
}

void add_dev_info(uint8_t *pid, uint8_t rssi_a, uint8_t rssi_b)
{
    uint8_t i,j;
    uint8_t bFind,bIsOverMaxSupportDevNum,cnt;

    bFind = FALSE;
    bIsOverMaxSupportDevNum = FALSE;

    for(i=0;i<st_DevInfoList.index;i++)
    {
        cnt = 0;
        for(j=0;j<DEV_ID_LEN;j++)
        {
            if(st_DevInfoList.dev_info[i].id[j] == pid[j])
            {
                cnt++;
            }
        }
        
        if(cnt == DEV_ID_LEN)
        {
            bFind = TRUE;
            break;
        }
    }

    if(bFind)
    {
        st_DevInfoList.dev_info[i].rssi_a += rssi_a;
        st_DevInfoList.dev_info[i].rssi_a /= 2;
        st_DevInfoList.dev_info[i].rssi_b += rssi_b;
        st_DevInfoList.dev_info[i].rssi_b /= 2;
        return;
    }

    if(st_DevInfoList.index >= MAX_SEARCH_DEV_ID_NUM)
    {
        DLOG_Critical("over max %d support search dev num",MAX_SEARCH_DEV_ID_NUM);
        bIsOverMaxSupportDevNum = TRUE;
    }

    if(bIsOverMaxSupportDevNum)
    {
        uint8_t farthest_dev;
        farthest_dev = find_farthest_dev();

        if((st_DevInfoList.dev_info[farthest_dev].rssi_a + st_DevInfoList.dev_info[farthest_dev].rssi_b) 
            < (rssi_a + rssi_b))
        {
            return;
        }
        
        for(i=0;i<DEV_ID_LEN;i++)
            st_DevInfoList.dev_info[farthest_dev].id[i] = pid[i];
        
        st_DevInfoList.dev_info[farthest_dev].rssi_a = rssi_a;
        st_DevInfoList.dev_info[farthest_dev].rssi_b = rssi_b;
        return;
    }

    for(i=0;i<DEV_ID_LEN;i++)
        st_DevInfoList.dev_info[st_DevInfoList.index].id[i] = pid[i];
    
    st_DevInfoList.dev_info[st_DevInfoList.index].rssi_a = rssi_a;
    st_DevInfoList.dev_info[st_DevInfoList.index].rssi_b = rssi_b;
    st_DevInfoList.index++;
    DLOG_Warning("add vt:%02x,%02x,rc:%02x,%02x,%02x,%02x,%02x,rssi:%d %d",pid[0],pid[1],pid[2],pid[3],pid[4],pid[5],pid[6],rssi_a,rssi_b);
    return;
}

void get_nearest_dev_info(uint8_t *pid)
{
    uint8_t pos;

    print_dev_list_info();
    pos = select_nearest_dev();
    memcpy(pid,st_DevInfoList.dev_info[pos].id,DEV_ID_LEN);
    DLOG_Warning("select %02x,%02x,%02x,%02x,%02x,%02x,%02x,rssi:%d %d",pid[0],pid[1],pid[2],pid[3],pid[4],pid[5],pid[6],
        st_DevInfoList.dev_info[pos].rssi_a,st_DevInfoList.dev_info[pos].rssi_b);
}

void reset_dev_info(void)
{
    memset(&st_DevInfoList,0,sizeof(st_DevInfoList));
}
