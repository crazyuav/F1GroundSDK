#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "upgrade.h"
#include "hal_bb.h"
#include "hal.h"
#include "debuglog.h"
#include "ff.h"
#include "md5.h"
#include "hal_usb_host.h"
#include "test_usbh.h"
#include "test_upgrade.h"
#include "nor_flash.h"
#include "efuse.h"

/**
* @brief  check gnd lock.
* @param  none.
* @retval 1        ok.
* @retval 0        lose.
* @note   None.
*/
int8_t UPGRADE_GndAndSkyLockStatus(void)
{
    STRU_WIRELESS_INFO_DISPLAY *pst_bb_info;
    HAL_BB_GetInfo(&pst_bb_info);
    uint8_t lock_status = pst_bb_info->lock_status;
    return (lock_status & 0x01);
}

void UPGRADE_GndForSky(void const *argument)
{
    uint32_t i = 0;
    uint8_t u8_sendDataArray[UPGRADE_DATACHUNK_SIZE];
    uint16_t u16_upgradeReturnStatus = 0;
    FRESULT    fileResult;
    FIL        MyFile;
    uint32_t   u32_bytesRead = UPGRADE_DATACHUNK_SIZE;

    STRU_UPGRADE_PARAMTER *pst_upgradeParameter = (STRU_UPGRADE_PARAMTER *)argument;

    UPGRADE_UseUsbUpgrade(pst_upgradeParameter->filePath, 1);
    
    UPGRADE_Start();    
    
    while (0x01 != UPGRADE_GndAndSkyLockStatus())
    {
        HAL_Delay(500);
        DLOG_Info("wait lock");
    }

    while(1 != UPGRADE_GndSendInit())
    {
        DLOG_Warning("wait sky init");
        HAL_Delay(1000);
    }

    fileResult = f_open(&MyFile, pst_upgradeParameter->filePath, FA_READ);
    if (FR_OK != fileResult)
    {
        DLOG_Info("open or create file error: %d\n", fileResult);
        return;
    }

    u32_bytesRead = UPGRADE_DATACHUNK_SIZE;
    u16_upgradeReturnStatus = 0;
    while(UPGRADE_DATACHUNK_SIZE == u32_bytesRead)
    {            
        fileResult = f_read(&MyFile, (void *)u8_sendDataArray, UPGRADE_DATACHUNK_SIZE, (void *)&u32_bytesRead);
        if((fileResult != FR_OK))
        {
            DLOG_Info("Cannot Read from the file \n");
            f_close(&MyFile);
            return;
        }
        while (0 != UPGRADE_SendDataBlock(UPGRADE_DATA_DATA, u8_sendDataArray, u32_bytesRead, u16_upgradeReturnStatus))
        {
            HAL_Delay(50);
        }
        
        u16_upgradeReturnStatus++;
    }
   
    DLOG_Info("send data finish");
    STRU_UPGRADE_STATUS st_lostData;
    while((0 == UpgradeGndStatus_Get(UPGRADE_ACK_END)) && (0 == UpgradeGndStatus_Get(UPGRADE_ACK_FAIL)))
    {
        if (UpgradeGndStatus_Get(UPGRADE_ACK_LOSTDATA))        
        {
            UPGRADE_GetStatus(&st_lostData);
            for (i = 0; i <  st_lostData.u16_lostDataBlockCount; i++)
            {
                
                DLOG_Warning("re-send data block %d",st_lostData.u16_lostDataBlockArray[i]);
                fileResult = f_lseek(&MyFile, st_lostData.u16_lostDataBlockArray[i] * UPGRADE_DATACHUNK_SIZE);
                if((fileResult != FR_OK))
                {
                    DLOG_Info("Cannot lseek the file \n");
                    f_close(&MyFile);
                    return;
                }
                fileResult = f_read(&MyFile, (void *)u8_sendDataArray, UPGRADE_DATACHUNK_SIZE, (void *)&u32_bytesRead);
                if((fileResult != FR_OK))
                {
                    DLOG_Info("Cannot Read from the file \n");
                    f_close(&MyFile);
                    return;
                }
                while (0 != UPGRADE_SendDataBlock(UPGRADE_LOSTBLOCK_DATA, u8_sendDataArray, u32_bytesRead, st_lostData.u16_lostDataBlockArray[i]))
                {
                    HAL_Delay(50);
                }
                  
            }
            memset(&st_lostData, 0, sizeof(st_lostData));
        }

        while (0 != UPGRADE_GndSendEnd())
        {
            HAL_Delay(50);
        }

        DLOG_Info("wait sky receive data finish ");
        HAL_Delay(2000);
    }

    if (UpgradeGndStatus_Get(UPGRADE_ACK_END))
    {
        DLOG_Error("upgrade ok");
    }
    else if (UpgradeGndStatus_Get(UPGRADE_ACK_FAIL))
    {
        DLOG_Error("upgrade fail");
    }

    vTaskDelete(NULL);
}

void UPGRADE_LocalUpgrade(void const *argument)
{
    
    STRU_UPGRADE_PARAMTER *pst_upgradeParameter = (STRU_UPGRADE_PARAMTER *)argument;
    if (-1 == UPGRADE_UseUsbUpgrade(pst_upgradeParameter->filePath, 0))
    {
        vTaskDelete(NULL);
        return ;
    }

    UPGRADE_ModifyBootInfo(APPLICATION_IMAGE_START0);
    
    vTaskDelete(NULL);

}
char path[128];
void upgrade(char *filepath)
{
    memset(path,'\0',128);
    if(strlen(filepath)>127)
    {
        return;
    }
    memcpy(path, filepath, strlen(filepath));

    STRU_UPGRADE_PARAMTER st_upgradeParameter;
    st_upgradeParameter.filePath = path;
    st_upgradeParameter.e_upgradeTransferType = UPGRADE_USB;

    osThreadDef(UsbUpgrade, UPGRADE_LocalUpgrade, osPriorityNormal, 0, 15 * 128);
    osThreadCreate(osThread(UsbUpgrade), &st_upgradeParameter);
    
    vTaskDelay(100);       
}

void gndforskyupgrade(char *filepath)
{
    memset(path,'\0',128);
    if(strlen(filepath)>127)
    {
        return;
    }
    memcpy(path,filepath,strlen(filepath));
    
    STRU_UPGRADE_PARAMTER st_upgradeParameter;
    st_upgradeParameter.filePath = path;
    st_upgradeParameter.e_upgradeTransferType = UPGRADE_USB;

    osThreadDef(UsbUpgrade, UPGRADE_GndForSky, osPriorityNormal, 0, 15 * 128);
    osThreadCreate(osThread(UsbUpgrade), &st_upgradeParameter);
    vTaskDelay(100);       
}
