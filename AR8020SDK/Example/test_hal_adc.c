#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debuglog.h"
#include "test_hal_adc.h"
#include "hal_adc.h"


void command_TestHalAdRead(unsigned char *ch, unsigned char *nch)
{
    uint32_t value = 0;
    
    uint8_t tmpCh = strtoul(ch, NULL, 0); 
    uint8_t tmpNCh = strtoul(nch, NULL, 0); 

    value = HAL_ADC_Read(tmpCh, tmpNCh);
    DLOG_Warning("ch:%d value:0x%x", tmpCh, value);
}
