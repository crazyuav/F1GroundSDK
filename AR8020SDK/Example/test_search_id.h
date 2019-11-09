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

#ifndef __HAL_TEST_SEARCH_ID_H__
#define __HAL_TEST_SEARCH_ID_H__

#ifdef __cplusplus
extern "C"
{
#endif

                                                             
#define SEARCH_ID_TIMER                 (HAL_TIMER_NUM16)
#define SEARCH_ID_TIMEOUT          (10*1000*1000)     //10 second

void BB_Grd_SearchIdHandler(void *p);

void BB_Sky_SearchIdHandler(void *p);

void BB_skyRcIdEventHandler(void *p);

void BB_grdRcIdEventHandler(void *p);

#ifdef __cplusplus
}
#endif 

#endif
