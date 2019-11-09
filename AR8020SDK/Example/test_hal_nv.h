#ifndef __TEST_HAL_NV_H__
#define __TEST_HAL_NV_H__

#ifdef __cplusplus
extern "C"
{
#endif


void command_TestNvResetBbRcId(void);

void command_TestNvSetBbRcId(uint8_t *id1, uint8_t *id2, uint8_t *id3, uint8_t *id4, uint8_t *id5, 
                                    uint8_t *vt_id0, uint8_t *vt_id1);

void command_TestNvSetChipId(uint8_t *id1, uint8_t *id2, uint8_t *id3, uint8_t *id4, uint8_t *id5);

void command_TestUsrDataWrite(void);

void command_TestUsrDataRead(void);

void command_TestUsrDataDestroy(uint8_t *addr);

#ifdef __cplusplus
}
#endif 

#endif
