#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hal.h"
#include "debuglog.h"
#include "hal_norflash.h"
#include "test_norflash.h"
#include "memory.h"

void command_testNorFlash(char* start_addr_str, char* size_str, char* val_str)
{
    unsigned char val   = strtoul(val_str, NULL, 0);
    uint32_t start_addr = strtoul(start_addr_str, NULL, 0);
    uint32_t size       = strtoul(size_str, NULL, 0);
    uint32_t i          = 0;

    uint8_t* buf = malloc_safe(size);
    for(i = 0; i < size; i++)
    {
        buf[i] = (i&0xff);
    }

    DLOG_Critical("command_testNorFlash ~~");

    HAL_NORFLASH_Init();

    HAL_NORFLASH_Erase(HAL_NORFLASH_Sector, 1024*1024*4);
    HAL_NORFLASH_WriteByteBuffer(1024*1024*4, buf, size);

    HAL_NORFLASH_ReadByteBuffer(1024*1024*4, buf, 10);

    for(i = 0; i < 10; i++)
    {
        DLOG_Info("%x",buf[i]);
    }
}
