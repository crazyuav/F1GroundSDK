#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debuglog.h"
#include "auto_test.h"
#include "cpu_info.h"
#include "hal.h"

void command_readMemory(char *addr)
{
    unsigned int readAddress;
    unsigned char row;
    unsigned char column;
    
    DLOG_Critical("auto_test>%s %s", __FUNCTION__, addr);
    
    readAddress = strtoul(addr, NULL, 0);

    if (readAddress == 0xFFFFFFFF)
    {
        DLOG_Critical("read address is illegal\n");
        return;
    }

    /* align to 4 bytes */
    readAddress -= (readAddress % 4);

    /* print to serial */
    for (row = 0; row < 8; row++)
    {
        /* new line */
        DLOG_Critical("0x%08x: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n ", 
                  readAddress,
                  *(uint32_t *)readAddress,
                  *(uint32_t *)(readAddress + 4),
                  *(uint32_t *)(readAddress + 8),
                  *(uint32_t *)(readAddress + 12),
                  *(uint32_t *)(readAddress + 16),
                  *(uint32_t *)(readAddress + 20),
                  *(uint32_t *)(readAddress + 24),
                  *(uint32_t *)(readAddress + 28));

        readAddress += 32;
    }

    
    DIV_LINE(DIV_LINE_REPORT);
    AUTO_TEST_PrintResult(PASSED);
    AUTO_TEST_PrintApiRunTime("--", 0);
    DIV_LINE(DIV_LINE_END);
}


void command_writeMemory(char *addr, char *value)
{
    unsigned int writeAddress;
    unsigned int writeValue;

    writeAddress = strtoul(addr, NULL, 0);

    if (writeAddress == 0xFFFFFFFF)
    {
        DLOG_Error("write address is illegal\n");

        return;
    }

    writeValue   = strtoul(value, NULL, 0);

    *((unsigned int *)(writeAddress)) = writeValue;
}


void command_set_loglevel(char* cpu, char* loglevel)
{
    uint8_t level = strtoul(loglevel, NULL, 0);
    uint8_t id = CPUINFO_GetLocalCpuId();
    if ( (memcmp(cpu, "cpu0", strlen("cpu0")) == 0 && id == 0) || (memcmp(cpu, "cpu1", strlen("cpu1")) == 0 && id == 1) || (memcmp(cpu, "cpu2", strlen("cpu2")) == 0 && id == 2))
    {
        dlog_set_output_level(level);
    }

    return;
}

void command_getVersion(void)
{
    uint8_t str[64];

    HAL_GetVersion(str);

    DLOG_Warning("%s", str);
}

