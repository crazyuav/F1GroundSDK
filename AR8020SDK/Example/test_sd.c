#include "debuglog.h"
#include "test_sd.h"
#include "cpu_info.h"
#include "cmsis_os.h"
#include "hal.h"
#include "systicks.h"
#include <string.h>
#include <stdlib.h>
#include "ff_gen_drv.h"
#include "systicks.h"
#include "auto_test.h"
#include "memory.h"

FIL outFile, inFile;
FATFS SDFatFs;  /* File system object for SD card logical drive */
// FIL MyFile, MyFileIn;     /* File object */
char *SDPath = "SD:/"; 

extern Diskio_drvTypeDef  SD_Driver;
extern unsigned int command_str2uint(char *str);

static FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const TCHAR* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        DLOG_Info("%d, f_size(fp) = %d", __LINE__, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

void TestWR(uint8_t param0, uint8_t param1, uint8_t param2)
{
	uint32_t info;

	ENUM_HAL_SD_CTRL cmd = HAL_SD_GET_SECTOR_COUNT;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	cmd = HAL_SD_GET_SECTOR_SIZE;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	cmd = HAL_SD_GET_CSD_VERSION;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	cmd = HAL_SD_GET_MANUID;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	cmd = HAL_SD_GET_TRAN_SPEED;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	cmd = HAL_SD_GET_CARD_STATUS;
	if (HAL_SD_Ioctl(cmd, &info) != HAL_OK)
	{
		DLOG_Error("ioctl failed\n");
	}

	int i = 0;
	uint32_t sect = 0;
/*	while(i < 10)
	{
		HAL_SD_Write(sect, 0x81000000+i*0x1400, 10);
		HAL_SD_Read(0x81100000, sect, 2);
		DLOG_Info("%d",i);
		i++;
		sect += 1;
	}*/
    DLOG_Critical("param %d %d %d", param0, param1, param2);
    HAL_SD_Write(param0, 0x81000000+param1*0x1400, param2);
    HAL_SD_Read(0x81100000+param1*0x1400, param0, param2);


/* 	HAL_SD_Deinit(); */
}

void TestFatFs()
{
	FRESULT res;
	uint32_t byteswritten, bytesread;
	static char *name;

	name = malloc_safe(128);
	memset(name, 0, 128);
	memcpy(name, "test.txt", strlen("test.txt"));
    FIL MyFile, MyFileIn;
	uint32_t u32_start; 

	uint32_t rw_size = 0x1000000;
    
	if (FR_OK != f_chdrive("SD:"))
	{
		DLOG_Error("%d chdrive to SD:\\ fail", __LINE__);
    }
    
    res = f_open(&MyFile, (const TCHAR*)(name), FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);
    if (res != FR_OK) 
    {
    	DLOG_Error("f_open error, res = %d", res);
    }
    else
    {
/*     	DLOG_Info("%d f_open success!", __LINE__); */
    	u32_start = SysTicks_GetTickCount();
        if (FR_OK != f_chdrive("SD:"))
        {
            DLOG_Error("%d chdrive to SD:\\ fail", __LINE__);
        }
    	res = f_write(&MyFile, (const void*)(0x81000000), 
    				rw_size, (void *)&byteswritten);    	

        DLOG_Info("write %d byte, used %d ms, speed = %d kB/S", 
                    rw_size, 
                    SysTicks_GetTickCount() - u32_start, 
                    rw_size/(SysTicks_GetTickCount() - u32_start));

    	if ((byteswritten == 0) || (res != FR_OK))
    	{
    		DLOG_Error("f_write error!");
    	}
    	else
    	{
/*     		DLOG_Info("f_write success!"); */
    		if ( FR_OK == f_close(&MyFile))
    		{
        		DLOG_Info("f_close success");
    		}
    		
            if (FR_OK != f_chdrive("SD:"))
            {
                DLOG_Error("%d chdrive to SD:\\ fail", __LINE__);
            }
/*     		if (f_open(&MyFile, (const TCHAR*)name, FA_READ) != FR_OK) */
            if(FR_OK != f_open(&MyFile, (const TCHAR*)(name), FA_OPEN_EXISTING | FA_READ))
    		{
    			DLOG_Error("f_open error!");
    		}
    		else
    		{
    			bytesread = rw_size;
    			u32_start = SysTicks_GetTickCount();
                if (FR_OK != f_chdrive("SD:"))
                {
                    DLOG_Error("%d chdrive to SD:\\ fail", __LINE__);
                }
    			res = f_read(&MyFile, (void*)(0x81000000), 
    						bytesread, (UINT*)&bytesread);
                DLOG_Info("read %d byte, used %d ms, speed = %d kB/S", 
                            rw_size, 
                            SysTicks_GetTickCount() - u32_start, 
                            rw_size/(SysTicks_GetTickCount() - u32_start));

    			if ((bytesread == 0) || (res != FR_OK))
    			{
    				DLOG_Error("f_read error!");
    			}
    			else
    			{
/*     				DLOG_Info("f_read success!"); */
    				f_close(&MyFile);

    				if ((bytesread != byteswritten))
    				{
    					DLOG_Error("f_close error!");
    				}
    				else
    				{
/*     					DLOG_Info("f_close success!"); */
    				}
    			}
    		}
    	}
	}

	free_safe(name);
}

void TestFatFs1()
{
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */

    int read, write;
    uint8_t *pcm_buffer = malloc_safe(2048);
    memset(pcm_buffer, 0, 2048);

	/*##-1- Link the micro SD disk I/O driver ##################################*/
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		DLOG_Info("Link success!");
		/*##-2- Register the file system object to the FatFs module ##############*/
		if ((res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1)) != FR_OK)
		{
			/* FatFs Initialization Error */
			DLOG_Error("f_mount = %d", res);
			DLOG_Error("f_mount error!");
		}
		else
		{
			DLOG_Info("f_mount success!");
			// res = f_mkfs((TCHAR const*)SDPath, 0, 0);
			// DLOG_Info("f_mkfs = %d\n", res);
			/*##-4- Create and Open a new text file object with write access #####*/
 			if (f_open(&outFile, "a.mp3", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) 
            //if (f_open(&outFile, (const TCHAR*)"a.mp3", FA_WRITE | FA_OPEN_APPEND) != FR_OK)
            // if (open_append(&outFile, "a.mp3"))
			{
				DLOG_Error("f_open out error!");
			}
			else
			{
                DLOG_Info("f_open a.mp3 success!");
/*                 f_printf(&outFile, "%s %s\n", __TIME__, __DATE__); */
			}

			if (f_open(&inFile, (const TCHAR*)"a.wav", FA_READ) != FR_OK)
			{
				DLOG_Error("f_open in error!");
			}
            else
            {
                DLOG_Info("f_open a.wav success!");                
            }
			
			DLOG_Info("f_open success!");
			/*##-5- Write data to the text file ################################*/
			
			//do
			//{
				res = f_read(&inFile, pcm_buffer, 10, &read);
                DLOG_Info("%d res = %d", __LINE__, res);
                
                DLOG_Info("pcm_buffer = %s", pcm_buffer);

                if ((read == 0) || (res != FR_OK))
                {
                    //break;
                }
#if 0
				if ((read == 0) || (res != FR_OK))
				{
					DLOG_Info("f_read error!");
				}
				else
				{
					DLOG_Info("read %d",read);
					res = f_write(&outFile, pcm_buffer, read, (void *)&write);
				}
#endif
                res = f_write(&outFile, pcm_buffer, read, (void *)&write);
                DLOG_Info("%d res = %d %d", __LINE__, res, write);
			//} while(read > 0);

			res = f_close(&inFile);
            DLOG_Info("%d res = %d", __LINE__, res);
			res = f_close(&outFile);
            DLOG_Info("%d res = %d", __LINE__, res);

			DLOG_Info("complete!");		
		}
	}

	/*##-11- Unlink the micro SD disk I/O driver ###############################*/
	free_safe(pcm_buffer);
	res = FATFS_UnLinkDriver(SDPath);
}

void TestFatFs2()
{
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread = 1;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[2048];                                   /* File read buffer */
	FIL fileIn;
	/*##-1- Link the micro SD disk I/O driver ##################################*/
	if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		DLOG_Critical("Link success!\n");
		/*##-2- Register the file system object to the FatFs module ##############*/
		if ((res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1)) != FR_OK)
		{
			/* FatFs Initialization Error */
			DLOG_Error("f_mount = %d\n", res);
			DLOG_Error("f_mount error!\n");
		}
		else
		{
			DLOG_Critical("f_mount success!\n");
			// res = f_mkfs((TCHAR const*)SDPath, 0, 0);
			// DLOG_Critical("f_mkfs = %d\n", res);
			/*##-4- Create and Open a new text file object with write access #####*/
			// if (f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
			// {
			// 	/* 'STM32.TXT' file Open for write Error */
			// 	DLOG_Critical("f_open error!\n");
			// }
			if (f_open(&fileIn, (const TCHAR*)("a.wav"), FA_READ) != FR_OK)
			{
				/* 'STM32.TXT' file Open for write Error */
				DLOG_Error("f_open error!\n");
			}
			else
			{
				DLOG_Critical("f_open success!\n");
				/*##-5- Write data to the text file ################################*/
				while(bytesread)
				{
				res = f_read(&fileIn, rtext, 1024, (void *)&bytesread);
				DLOG_Critical("read %d \n", bytesread);
				}


			}
		
		}
	}

	/*##-11- Unlink the micro SD disk I/O driver ###############################*/
	res = FATFS_UnLinkDriver(SDPath);
}

void TestFatFs3()
{
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswritten, bytesread;                     /* File write/read counts */

    int read, write;
    uint8_t *pcm_buffer = "ThisisSTM32workingwithFatFs";
    uint8_t rtext[128];
    /*##-1- Link the micro SD disk I/O driver ##################################*/
    if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
    {
        DLOG_Critical("Link success!");
        /*##-2- Register the file system object to the FatFs module ##############*/
        if ((res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1)) != FR_OK)
        {
            /* FatFs Initialization Error */
            DLOG_Error("f_mount = %d", res);
            DLOG_Error("f_mount error!");
        }
        else
        {
            DLOG_Critical("f_mount success!");
            // res = f_mkfs((TCHAR const*)SDPath, 0, 0);
            // DLOG_Info("f_mkfs = %d\n", res);
            if (f_open(&inFile, "autotest.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
            {
                DLOG_Error("f_open out error!");
            }
            else
            {
                DLOG_Critical("f_open autotest.txt success!");
            }

            if (f_write(&inFile, pcm_buffer, strlen(pcm_buffer), (void *)&byteswritten) != FR_OK)
            {
                DLOG_Error("f_write out error!");
            }
            else
            {
                DLOG_Critical("f_write autotest.txt success! %d %d", byteswritten, strlen(pcm_buffer));
            }

            res = f_close(&inFile);
            if(res != FR_OK )
            {
                DLOG_Error("f_close out error! res %d", res);
            }
            else
            {
                DLOG_Critical("f_close autotest.txt success!");
            }

            
            if (f_open(&outFile, "autotest.txt", FA_READ) != FR_OK)
            {
                DLOG_Error("f_open out error!");
            }
            else
            {
                DLOG_Critical("f_open autotest.txt success!");
            }
            memset(rtext,'\0',128);
            res = f_read(&outFile, rtext, byteswritten, (void *)&bytesread);
            if(res != FR_OK )
            {
                DLOG_Error("f_read out error! res %d", res);
            }
            else
            {
                DLOG_Critical("f_read autotest.mp3 success!");
            }

            
            res = f_close(&outFile);
            if(res != FR_OK )
            {
                DLOG_Error("f_close out error! res %d", res);
            }
            else
            {
                DLOG_Critical("f_close autotest.mp3 success!");
            }

            DLOG_Critical("read string %s ", rtext);
            DIV_LINE(DIV_LINE_REPORT);
            
            if (0 == memcmp(rtext, pcm_buffer, sizeof(pcm_buffer)))
            {
                AUTO_TEST_PrintResult(PASSED);
            }
            else
            {
                AUTO_TEST_PrintResult(FAILURE);
            }

            DIV_LINE(DIV_LINE_END);
   
        }
    }
    res = FATFS_UnLinkDriver(SDPath);
}

void command_initSdcard()
{
    // HAL_SD_InitIRQ();
	int res;
	if ((res = HAL_SD_Init()) != 0)
	{
		return;
	}

}

void command_sd_release()
{
    HAL_SD_Deinit();
}

void OS_TestRawWR_Handler(void const * argument)
{
	uint32_t sect;
	uint32_t u32_start; 
	uint32_t u32_end; 
	uint32_t u32_diff; 
	uint32_t totol_sects;

    uint32_t NUM_OF_BLOCK = 64;
    uint32_t TIMES = 10000;
    uint32_t START_BLOCK = 1000;
    
	uint32_t sect_multi;
	totol_sects = 30541 * 1024;
	
#if 0
	if (HAL_OK == HAL_SD_Erase(0, totol_sects) )
	{
		DLOG_Info("erase %d sects, used %d ms", totol_sects, SysTicks_GetTickCount() - u32_start);
	}
	else
	{
		DLOG_Info("error");
	}
#endif

    DLOG_Info("in testing, please wait for several seconds...");
	
    u32_start = SysTicks_GetTickCount();
    for (sect_multi = 0; sect_multi < TIMES; sect_multi++)
    {
        if ( HAL_OK == HAL_SD_Write(START_BLOCK + sect_multi * NUM_OF_BLOCK, 
                                    0x21006000, 
                                    NUM_OF_BLOCK))
        {       
/*          DLOG_Info("times = %d", sect_multi); */
        }
        else
        {
            DLOG_Error("error");
        }
    }
    
    u32_diff = SysTicks_GetTickCount() - u32_start;
    DLOG_Info("write %d sects, used %d ms, speed = %d kB/S", 
                NUM_OF_BLOCK * TIMES, 
                u32_diff,
                NUM_OF_BLOCK * TIMES * 512/u32_diff);

    u32_start = SysTicks_GetTickCount();
	for (sect_multi = 0; sect_multi < TIMES; sect_multi++)
	{
		if ( HAL_OK == HAL_SD_Read(0x21006000, 
									START_BLOCK + sect_multi * NUM_OF_BLOCK, 
									NUM_OF_BLOCK))
/*
		if ( HAL_OK == HAL_SD_Read(0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET, 
									sect_multi * NUM_OF_BLOCK, 
									NUM_OF_BLOCK))
*/
		{		
/* 			DLOG_Info("times = %d", sect_multi); */
		}
		else
		{
			DLOG_Error("error");
		}
	}
	
	u32_diff = SysTicks_GetTickCount() - u32_start;
    DLOG_Info("read %d sects, used %d ms, speed = %d kB/S", 
                NUM_OF_BLOCK * TIMES, 
                u32_diff,
                NUM_OF_BLOCK * TIMES * 512/u32_diff);
    
#if 0
	for (sect_multi = 0; sect_multi < 10; sect_multi++)
	{
		u32_start = SysTicks_GetTickCount();
/*
		if ( HAL_OK == HAL_SD_Write(sect_multi * NUM_OF_BLOCK, 
		                            0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET, 
		                            NUM_OF_BLOCK) )
*/
        if ( HAL_OK == HAL_SD_Write(0, 0x21000000, NUM_OF_BLOCK) )              // sram
		{
			DLOG_Info("write %d sects, sect_multi = %d, used %d ms, speed = %d kB/S", 
			            NUM_OF_BLOCK, 
			            sect_multi, 
						SysTicks_GetTickCount() - u32_start, 
						NUM_OF_BLOCK*512/(SysTicks_GetTickCount() - u32_start));
		}
		else
		{
			DLOG_Info("error");
		}
	}

	for (sect_multi = 0; sect_multi < 10; sect_multi++)
	{
		u32_start = SysTicks_GetTickCount();
		if ( HAL_OK == HAL_SD_Read(0x21000000, 
									sect_multi * NUM_OF_BLOCK, 
									NUM_OF_BLOCK))
/*
		if ( HAL_OK == HAL_SD_Read(0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET, 
									sect_multi * NUM_OF_BLOCK, 
									NUM_OF_BLOCK))
*/
		{
			DLOG_Info("read %d sects, sect_multi = %d, used %d ms, speed = %d kB/S", 
			            NUM_OF_BLOCK, 
			            sect_multi, 
						SysTicks_GetTickCount() - u32_start,
						NUM_OF_BLOCK*512/(SysTicks_GetTickCount() - u32_start));			
		}
		else
		{
			DLOG_Info("error");
		}
	}
#endif

	DLOG_Info("task finished");
	
	for (;;)
	{
		HAL_Delay(1500);
	}
}

void OS_TestSD_Erase_Handler(void const * argument)
{
	uint32_t totol_sects = 30541 * 1024;
	uint32_t u32_start = SysTicks_GetTickCount();
	
	if (HAL_OK == HAL_SD_Erase(0, totol_sects) )
	{
		DLOG_Info("erase %d sects, used %d ms", totol_sects, SysTicks_GetTickCount() - u32_start);
	}
	else
	{
		DLOG_Error("error");
	}
	
        for (;;)
	{
		HAL_Delay(1500);
	}
}


/*
void OS_TestRawWR()
{	
	osThreadDef(TestRawWR_Handler, OS_TestRawWR_Handler, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(TestRawWR_Handler), NULL);
}
*/

/*
void OS_TestSD_Erase()
{
	osThreadDef(TestSD_Erase, OS_TestSD_Erase_Handler, osPriorityNormal, 0, 8 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(TestSD_Erase), NULL);
}
*/

void Test_hal_read()
{
    if (HAL_OK == HAL_SD_Read(0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET, 0, 13))
    {
        DLOG_Info("read done");
    }
}
#if 0
extern SD_HandleTypeDef sdhandle;
void TestFatFs_with_usb()
{
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */
	static char name[] = "myfile1";

	uint32_t u32_start; 

	DLOG_Info("%d, sdhandle.inited = %d", __LINE__, sdhandle.inited);
    
	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0)
	{
		DLOG_Error("Link error!");
		return;
	}
	
	DLOG_Info("Link success!");

	if ((res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1)) != FR_OK)
	{
		DLOG_Error("f_mount = %d", res);
		DLOG_Error("f_mount error!");
	}
	else
	{
		DLOG_Info("%d f_mount success!", __LINE__);
		DLOG_Output(1000);
#if 0
		name[6] += 1;
		if (f_open(&MyFile, name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) 
		{
			DLOG_Info("f_open error!");
		}
		else
		{
			DLOG_Info("%d f_open success!", __LINE__);
	
			u32_start = SysTicks_GetTickCount();
			res = f_write(&MyFile, (const void*)(0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET), 
						0x1000000, (void *)&byteswritten);
			DLOG_Info("%d, write %d ms", __LINE__, SysTicks_GetTickCount() - u32_start);
			
			if ((byteswritten == 0) || (res != FR_OK))
			{
				DLOG_Info("f_write error!");
			}
			else
			{
				DLOG_Info("f_write success!");
				f_close(&MyFile);

				if (f_open(&MyFile, name, FA_READ) != FR_OK)
				{
					DLOG_Info("f_open error!");
				}
				else
				{
					bytesread = 0x1000000;
					u32_start = SysTicks_GetTickCount();
					res = f_read(&MyFile, (void*)(0x81000000 - DTCM_CPU0_DMA_ADDR_OFFSET), 
								bytesread, (UINT*)&bytesread);
					DLOG_Info("%d, read %d ms", __LINE__, SysTicks_GetTickCount() - u32_start);

					if ((bytesread == 0) || (res != FR_OK))
					{
						DLOG_Info("f_read error!");
					}
					else
					{
						DLOG_Info("f_read success!");
						f_close(&MyFile);

						if ((bytesread != byteswritten))
						{
							DLOG_Info("f_close error!");
						}
						else
						{
							DLOG_Info("f_close success!");
						}
					}
				}
			}
		}
#endif
	
	}

    
	DLOG_Info("%d, sdhandle.inited = %d", __LINE__, sdhandle.inited);

	res = FATFS_UnLinkDriver(SDPath);
}
#endif
void TestRawWR_SDRAM()
{
    uint32_t sect;
	uint32_t u32_start; 
	uint32_t u32_end; 
	uint32_t u32_diff; 
	uint32_t totol_sects;

    uint32_t NUM_OF_BLOCK = 30000;
    uint32_t TIMES = 10;
    uint32_t START_BLOCK = 1000;
    
	uint32_t sect_multi;
	totol_sects = 30541 * 1024;
	
    
	for (sect_multi = 0; sect_multi < TIMES; sect_multi++)
	{
		u32_start = SysTicks_GetTickCount();
		if ( HAL_OK == HAL_SD_Write(START_BLOCK + sect_multi * NUM_OF_BLOCK, 
		                            0x81000000, 
		                            NUM_OF_BLOCK) )
		{
		    u32_diff = SysTicks_GetTickCount() - u32_start;
			DLOG_Info("write %d sects, sect_multi = %d, used %d ms, speed = %d kB/S", 
			            NUM_OF_BLOCK, 
			            sect_multi, 
						u32_diff, 
						NUM_OF_BLOCK*512/u32_diff);
		}
		else
		{
			DLOG_Error("error");
		}
	}

	for (sect_multi = 0; sect_multi < TIMES; sect_multi++)
	{
		u32_start = SysTicks_GetTickCount();
		if ( HAL_OK == HAL_SD_Read(0x81000000, 
									START_BLOCK + sect_multi * NUM_OF_BLOCK, 
									NUM_OF_BLOCK))
		{
		    u32_diff = SysTicks_GetTickCount() - u32_start;
			DLOG_Info("read %d sects, sect_multi = %d, used %d ms, speed = %d kB/S", 
			            NUM_OF_BLOCK, 
			            sect_multi, 
						u32_diff,
						NUM_OF_BLOCK*512/u32_diff);			
		}
		else
		{
			DLOG_Error("error");
		}
	}

	DLOG_Info("task finished");	
}

static void test_multi_read()
{
	uint32_t sect;
	uint32_t u32_start; 
	uint32_t u32_end; 
	uint32_t u32_diff; 
	uint32_t totol_sects;

    uint32_t NUM_OF_BLOCK = 64;
    uint32_t TIMES = 100;
    uint32_t START_BLOCK = 1000;
    
	uint32_t sect_multi;
	totol_sects = 30541 * 1024;

	if ( HAL_OK == HAL_SD_Read_test(0x21006000, 
							START_BLOCK + NUM_OF_BLOCK, 
							NUM_OF_BLOCK))
	{
        DLOG_Info("done");
	}						
}

static void test_multi_write()
{
	uint32_t sect;
	uint32_t u32_start; 
	uint32_t u32_end; 
	uint32_t u32_diff; 
	uint32_t totol_sects;

    uint32_t NUM_OF_BLOCK = 64;
    uint32_t TIMES = 100;
    uint32_t START_BLOCK = 1000;
    
	uint32_t sect_multi;
	totol_sects = 30541 * 1024;

    if ( HAL_OK == HAL_SD_Write_test(START_BLOCK, 0x21006000, NUM_OF_BLOCK))
	{
        DLOG_Info("done");
	}						
}


void command_SdcardFatFs(char *argc, char *argc1 , char *argc2, char *argc3)
{	
	uint8_t choise;
	choise = strtoul(argc, NULL, 0);
    uint8_t param1 = strtoul(argc1, NULL, 0);
    uint8_t param2 = strtoul(argc2, NULL, 0);
    uint8_t param3 = strtoul(argc3, NULL, 0);
    uint32_t i;
    
	switch (choise)
	{
		case 0:
			TestWR(param1, param2, param3);
		break;

		case 1:
		    for (i = 0; i < 10;i++)
		    {
			    DLOG_Info("time = %d", i);
			    TestFatFs();
			}
		break;

		case 2:
			TestFatFs1();
		break;

		case 3:
			TestFatFs2();
		break;

		case 4:
/* 			OS_TestRawWR(); */
		break;

		case 5:
/* 			OS_TestSD_Erase(); */
		break;

		case 6:
		    Test_hal_read();
		break;

		case 7:
		    //TestFatFs_with_usb();
		break;

		case 8:
		    TestRawWR_SDRAM();
		break;

		case 9:
		    test_multi_read();
		break;

        case 10:
            test_multi_write();
        break;

        case 11:
            TestFatFs3();
        break;

		default: break;
	}
}

