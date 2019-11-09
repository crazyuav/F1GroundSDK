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

#ifndef __HAL_TEST_SIGQUALITY_H__
#define __HAL_TEST_SIGQUALITY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define INVALID_SIG_QUALITY_VALUE (0xFF)
                                                             
uint8_t get_sky_sigQuality(void);

uint8_t get_sigQuality(uint8_t *gnd_sigQuality, uint8_t *sky_sigQuality);

uint8_t GndSigQuality(int errCnt, float snr);

uint8_t SkySigQuality(int lockCnt, float snr);

#ifdef __cplusplus
}
#endif 

#endif