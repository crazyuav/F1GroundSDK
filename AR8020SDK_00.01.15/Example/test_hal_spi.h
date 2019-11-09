#ifndef __TEST_HAL_SPI_H__
#define __TEST_HAL_SPI_H__

#ifdef __cplusplus
extern "C"
{
#endif


#include "hal_spi.h"
#include "debuglog.h"

void command_TestHalSpiInit(unsigned char *ch, unsigned char *br);

void command_TestHalSpiTxRx(unsigned char *ch,  unsigned char *txStartData, unsigned char *txLen, unsigned char *rxLen);



#ifdef __cplusplus
}
#endif 

#endif
