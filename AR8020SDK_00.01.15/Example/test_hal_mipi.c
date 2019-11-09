#include "debuglog.h"
#include "hal_mipi.h"
#include "test_hal_mipi.h"
#include "test_hal_camera.h"


void command_TestHalMipiInit(char *toEncoderCh, char *width, char *hight, char *frameRate)
{
    uint16_t u16_width = strtoul(width, NULL, 0);
    uint16_t u16_hight = strtoul(hight, NULL, 0);
    uint8_t u8_frameRate = strtoul(frameRate, NULL, 0);
    uint8_t ch = strtoul(toEncoderCh, NULL, 0);

    //HAL_CAMERA_GetImageInfo(&u16_width, &u16_hight, &u8_frameRate);
    DLOG_Warning("width:%d hight:%d frameRate:%d",u16_width,u16_hight,u8_frameRate);
    HAL_MIPI_Init(ch, u16_width, u16_hight, u8_frameRate);
}

void command_TestSetMipiLane(char *lane)
{
    uint32_t tmp_lane = strtoul(lane, NULL, 0);

    HAL_MIPI_IOCtl(0, tmp_lane);
}

