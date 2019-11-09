#include "debuglog.h"
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "test_i2c_adv7611.h"
#include "test_hal_camera.h"
#include "hal_dma.h"
#include "memory_config.h"
#include "hal_ret_type.h"
#include "test_hal_mipi.h"
#include "test_usbh.h"
#include "test_hal_nv.h"
#include "ar_freertos_specific.h"
#include "test_bb.h"
#include "test_common.h"
#include "test_upgrade.h"
#include "upgrade.h"
#include "cmd_line.h"
#include "test_memory.h"
#include "test_usbh.h"
#include "test_sysevent.h"
#include "test_hal_spi_flash.h"
#include "test_hal_i2c_24c256.h"
#include "test_hal_can.h"
#include "test_hal_uart.h"
#include "testhal_dma.h"
#include "uvc_task.h"
#include "test_bb_com.h"
#include "testhal_timer.h"
#include "testhal_pwm.h"
#include "testhal_gpio.h"
#include "test_sd.h"
#include "test_dhcp.h"
#include "test_udp_echo.h"
#include "test_tcp_echo.h"
#include "test_ping.h"
#include "test_hal_spi.h"
#include "test_time_delay.h"
#include "test_net_repeater_ground.h"
#include "test_net_repeater_sky.h"
#include "sleep_mode.h"
#include "factory.h"
#include "test_hal_sram.h"
#include "test_hal_spi_flash.h"

static void command_send_usb(char *len, char *ep);
static void command_switchto_ios(void);

STRU_CMD_ENTRY_T g_cmdArray[] =
{
    {0, (f_cmdline)command_TestNvResetBbRcId, "NvResetBbRcId",  ""},
    {1, (f_cmdline)upgrade,  "upgrade", "<filename>"},
    {1, (f_cmdline)gndforskyupgrade, "gndforskyupgrade", "<filename>"},
    {1, (f_cmdline)command_startBypassVideo, "startbypassvideo", "channel"},
    {0, (f_cmdline)command_stopBypassVideo, "stopbypassvideo",  ""},// 
    {2, (f_cmdline)command_TestHalCameraInit, "test_camera_init",  "<rate 0~1> <mode 0~8>"},
    {4, (f_cmdline)command_TestHalMipiInit, "test_hal_mipi_init", "<toEncoderCh 0~1> <width>, <hight> <frameRate>"},
    {2, (f_cmdline)command_TestHalUartInit, "test_hal_uart_init", "<ch> <baudr>"},
    {2, (f_cmdline)command_TestHalUartAsynTx, "test_hal_uart_asyn_tx", "<ch> <len>"},
    {2, (f_cmdline)command_TestHalUartTx, "test_hal_uart_tx", "<ch> <len>"},
    {7, (f_cmdline)command_TestNvSetBbRcId, "NvSetBbRcId",  "<rc id1~5> <vt id0~1>"},
    {5, (f_cmdline)command_TestNvSetChipId, "NvSetChipId",  "<chip id1~5>"},
    {2, (f_cmdline)command_set_loglevel, "set_loglevel", "<cpuid> <loglevel>"},
    {0, (f_cmdline)ar_top, "top", ""},
    {1, (f_cmdline)command_TestRcvRate, "TestRcvRate", "<com_type>"},
    {2, (f_cmdline)command_TestHalSpiInit, "TestHalSpiInit", "<ch> <br>"},
    {4, (f_cmdline)command_TestHalSpiTxRx, "TestHalSpiTxRx", "<ch> <txStartData> <txLen> <rxLen>"},
    {0, (f_cmdline)command_TestRevRT, "TestRevRT", ""},
    {0, (f_cmdline)command_TestRcv, "TestRcv", ""},
    {0, (f_cmdline)command_TestComTask, "TestComTask", ""},
    {0, (f_cmdline)command_TestRT, "TestRT", ""},
    {0, (f_cmdline)command_TestHead, "TestHead", ""},
    {0, (f_cmdline)command_TestPeriodHead, "TestPeriodHead", ""},
    {1, (f_cmdline)command_TestRate, "TestRate", "<cycle>"},
    {2, (f_cmdline)command_dma_asyn, "testdma", ""},
    {1, (f_cmdline)command_EnterSleep, "EnterSleep", "<level>"},
    {0, (f_cmdline)command_ExitSleep, "ExitSleep", ""},
    {2, (f_cmdline)command_setTargetPower, "setPower", "2gTargetpower 5gTargetpower"},
    {1, (f_cmdline)command_setTargetStardard, "PowerStardard", "FCC/CE.."},    
    {0, (f_cmdline)command_getVersion, "getVersion", ""},
    {0, (f_cmdline)command_GetSramReceivedDataSize, "ReceivedDataSize", ""},
    {0, (f_cmdline)FCT_Reset, "FCT_Reset", ""},
    {2, (f_cmdline)command_send_usb, "sendusb", "<datalen> <endpoint>"},
    {0, (f_cmdline)command_switchto_ios, "switchtoios", ""},
    {0, (f_cmdline)command_TestReadFlashReg, "ReadFlashReg", ""},
    {1, (f_cmdline)command_TestSetFlashStatusReg1, "SetFlashStatusReg1", "<reg_value>"},
    {1, (f_cmdline)command_TestSetFlashStatusReg2, "SetFlashStatusReg2", "<reg_value>"},
    {0, (f_cmdline)command_TestReadFlashID, "ReadFlashID", ""},
    END_OF_CMD_ENTRY
};

unsigned int command_str2uint(char *str)
{
    return strtoul(str, NULL, 0);
}

static uint8_t g_stringSend[512] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
static void command_send_usb(char *len, char *ep)
{
    uint16_t   length = command_str2uint(len);
    uint8_t    endpoint = strtoul(ep, NULL, 16);

    HAL_USB_SendData(g_stringSend, length, 0, endpoint);
}

extern uint8_t g_u8CurrentHost;     /* 0: normal   1: iphone */
static void command_switchto_ios(void)
{
    g_u8CurrentHost = 1;
}

