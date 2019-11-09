#include <stdlib.h>
#include "hal_rtc.h"
#include "ff.h"


DWORD get_fattime (void)
{
    STRU_HAL_UTC_CALENDAR st_halRtcCalendar;
    DWORD fatfs_time;

    HAL_UTC_Get(&st_halRtcCalendar);
    fatfs_time = (st_halRtcCalendar.u16_year-1980) << 25 |
                  st_halRtcCalendar.u8_month << 21 |
                  st_halRtcCalendar.u8_day << 16 |
                  st_halRtcCalendar.u8_hour << 11 |
                  st_halRtcCalendar.u8_minute << 5 |
                  st_halRtcCalendar.u8_second >> 1;

    return fatfs_time;
}


