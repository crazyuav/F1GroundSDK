#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debuglog.h"
#include "hal_sram_ground.h"


void command_GetSramReceivedDataSize(void)
{
    uint32_t sram0DataSize, sram1DataSize;

    HAL_SRAM_GetReceivedDataSize(&sram0DataSize, &sram1DataSize);

    DLOG_Info("sram0 sram1: %d %d", sram0DataSize, sram1DataSize);
}
