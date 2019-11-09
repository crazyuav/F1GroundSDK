/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: test_bb_sigQuality
Description: 
Author: Artosy Software Team
Version: 0.0.1
Date: 2017/12/14
History:
         0.0.1    2017/12/14    test_bb_sigQuality
*****************************************************************************/

#include "debuglog.h"
#include "hal_ret_type.h"
#include "hal_bb.h"
#include "test_bb_sigQuality.h"

float GndSnr2Score(float snr)
{
    return 3.52*snr + 19.057;
}

float SkySnr2Score(float snr)
{
    return 6.2*snr + 27;
}

int GndErrCnt2Score(int errCnt)
{
    if(errCnt < 5)
    {
        return 2;
    }
    else if(errCnt < 10)
    {
        return 5;
    }
    else if(errCnt < 20)
    {
        return 10;
    }
    else if(errCnt < 30)
    {
        return 20;
    }
    else if(errCnt < 40)
    {
        return 30;
    }
    else if(errCnt < 50)
    {
        return 40;
    }
    else if(errCnt < 55)
    {
        return 50;
    }
    else
    {
        return 55;
    }


}
uint8_t GndSigQuality(int errCnt, float snr)
{
    float score;
    
    score = GndSnr2Score(snr) - GndErrCnt2Score(errCnt);
    if(score <= 0)
    {
        return 0;
    }
    else if(score >= 100)
    {
        return 100;
    }
    else
    {
        return (uint8_t)score;
    }
}

uint8_t SkySigQuality(int lockCnt, float snr)
{
    float score;

    score = SkySnr2Score(snr)*lockCnt/100;
    if(score <= 0)
    {
        return 0;
    }
    else if(score >= 100)
    {
        return 100;
    }
    else
    {
        return (uint8_t)score;
    }
}

uint8_t get_sky_sigQuality(void)
{
    HAL_RET_T ret;
    STRU_WIRELESS_INFO_DISPLAY *pst_bbInfoAddr;


    ret = HAL_BB_GetInfo(&pst_bbInfoAddr);
    if(ret != HAL_OK)
    {
        return INVALID_SIG_QUALITY_VALUE;
    }
    
    return SkySigQuality(pst_bbInfoAddr->u8_rclock,pst_bbInfoAddr->snr_vlaue[1]);
}

uint8_t get_sigQuality(uint8_t *gnd_sigQuality, uint8_t *sky_sigQuality)
{
    HAL_RET_T ret;
    STRU_WIRELESS_INFO_DISPLAY *pst_bbInfoAddr;


    ret = HAL_BB_GetInfo(&pst_bbInfoAddr);
    if(ret != HAL_OK)
    {
        return INVALID_SIG_QUALITY_VALUE;
    }

    *gnd_sigQuality = GndSigQuality(pst_bbInfoAddr->errcnt1,pst_bbInfoAddr->snr_vlaue[1]);
    *sky_sigQuality = SkySigQuality(pst_bbInfoAddr->u8_rclock,pst_bbInfoAddr->sky_snr);

    return 0;

}

