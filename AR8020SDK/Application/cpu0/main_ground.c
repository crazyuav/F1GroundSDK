#include "debuglog.h"
#include "test_i2c_adv7611.h"
#include "serial.h"
#include "hal_sram_ground.h"
#include "cmsis_os.h"
#include "sys_event.h"
#include "upgrade.h"
#include "hal.h"
#include "hal_bb.h"
#include "test_usbh.h"
#include "hal_usb_otg.h"
#include "hal_sys_ctl.h"
#include "wireless_interface.h"
#include "hal_nv.h"
#include "test_bb_led_ctrl.h"
#include "upgrade.h"
#include "hal_uart.h"
#include "cmd_line.h"
#include "hal_sd.h"
#include "uart_task.h"
#include "bb_reply_pc.h"
#include "usr_usb_task.h"
#include "usr_uart3_task.h"
#include "board_watchdog.h"
#include "app_sys_event_process.h"
#include "test_search_id.h"
#include "hal_i2c.h"
#include "hal_mfi.h"
#include "hal_gpio.h"
#include "stdarg.h"
#include "hal_mavlink.h"
#include "hal_adc.h"
#include "hal_timer.h"

#define USB_CTL_GPIO_NUM    HAL_GPIO_NUM80

void DLOG_Process(void* p);

static uint32_t hal_uart7_recvdata_handler(uint8_t *pu8_rxBuf, uint8_t u8_len)
{
    HAL_BB_ComSendMsg(BB_COM_SESSION_1, pu8_rxBuf, u8_len);
}

void console_init(uint32_t uart_num, uint32_t baut_rate)
{
    HAL_UART_Init(DEBUG_LOG_UART_PORT, HAL_UART_BAUDR_115200, NULL);

    DLOG_Init(CMD_exec_cmd, NULL, DLOG_SERVER_PROCESSOR);

    HAL_UART_Init(HAL_UART_COMPONENT_7, HAL_UART_BAUDR_115200, hal_uart7_recvdata_handler);
}


static void session1_rcvDataHandler(void *p)
{   
    uint8_t data_buf_proc[1024];
    uint32_t u32_rcvLen = 0;
    
    HAL_BB_ComReceiveMsg(BB_COM_SESSION_1, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);
    
    data_buf_proc[u32_rcvLen]= '\0';

    HAL_UART_TxData(HAL_UART_COMPONENT_7, data_buf_proc, u32_rcvLen, HAL_UART_DEFAULT_TIMEOUTMS * 10);
}


static void session2_rcvDataHandler(void *p)
{   
    uint8_t data_buf_proc[1024];
    uint32_t u32_rcvLen = 0;
    
    HAL_BB_ComReceiveMsg(BB_COM_SESSION_2, data_buf_proc, sizeof(data_buf_proc), &u32_rcvLen);

    if (u32_rcvLen > 0) 
    {
        usb_cdc_sendData(data_buf_proc, u32_rcvLen);
    }
}

static void ar_usb_cdc_recvdata_handler(uint8_t *pdata, uint32_t datalen)
{
    HAL_BB_ComSendMsg(BB_COM_SESSION_2, pdata, datalen);
}


void DLOG(void const *argument)
{
    static uint16_t toggleCnt = 0;
    while (1)
    {
        DLOG_Process(NULL);

        HAL_Delay(10);
        
        toggleCnt++;

        if (toggleCnt == 100)
        {
            HAL_LED_TOGGLE(CORE_LED_GPIO_WHITE);

            toggleCnt = 0;
        }
    }
}

static void IO_Task(void const *argument)
{
    while (1)
    {
        HAL_SRAM_CheckChannelTimeout();

        SYS_EVENT_Process();

    }
}

static void USB_Gpio_Ctrl_Init(void)
{
    HAL_GPIO_OutPut(USB_CTL_GPIO_NUM);
    HAL_GPIO_SetPin(USB_CTL_GPIO_NUM, HAL_GPIO_PIN_RESET);   
}

static void USER_Define_EventHandler(void* p)
{
    HAL_Delay(70);
    HAL_GPIO_SetPin(USB_CTL_GPIO_NUM, HAL_GPIO_PIN_SET);
    HAL_Delay(1); 
    HAL_GPIO_SetPin(USB_CTL_GPIO_NUM, HAL_GPIO_PIN_RESET);
}


// 地面端 8 个按键序列号
uint8_t pinNo[8] = {81, 82, 83, 84, 85, 86, 87, 88};

// 是否按下去了
uint8_t isPressed[8] = {false, false, false, false, false, false, false, false};

// 按下去次数计数
uint8_t pressedCnt[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// 配置偏移量
int16_t offsetValue[4]= {0, 0, 0, 0};


static void Usr_Usb_Debug(void const *argument)
{
    HAL_Delay(1000);

    unsigned char packet[25] = {0};

    uint16_t left_h = 0;
    uint16_t left_v = 0;
    uint16_t right_h = 0;
    uint16_t right_v = 0;

    uint16_t channels[16] = {0};

    while(1)
    {
        HAL_Delay(5);   

        uint8_t i = 10;

        while (i--)
        {
            left_h = (left_h + HAL_ADC_Read(7, 8)) >> 1;
            left_v = (left_v + HAL_ADC_Read(8, 6)) >> 1;

            right_h = (right_h + HAL_ADC_Read(6, 5)) >> 1;
            right_v = (right_v + HAL_ADC_Read(5, 7)) >> 1;
        }

        left_h =  (left_h + offsetValue[0]) << 1;
        left_v =  (left_v + offsetValue[1]) << 1;
        right_h = (right_h + offsetValue[2]) << 1;
        right_v = (right_v + offsetValue[3]) << 1;
        
        // 按键1 用于解锁
        if (isPressed[1] == false)
        {   
            channels[0] = left_h;
            channels[1] = left_v;
            channels[2] = right_h;
            channels[3] = right_v;
        }
        else
        {
            channels[0] = 1900;
            channels[1] = 1900;
            channels[2] = 50;
            channels[3] = 1900;
        }

        if (isPressed[2])
        {
            channels[0] = 50;
            channels[1] = 1900;
        }


        channels[4] = UINT16_MAX;  
        channels[5] = UINT16_MAX;
        channels[6] = (pressedCnt[3] % 2 == 0) ? 1900 : 50;
        channels[7] = (pressedCnt[4] % 2 == 0) ? 1900 : 50;
        channels[8] = (pressedCnt[5] % 2 == 0) ? 1900 : 50;
        channels[9] = (pressedCnt[6] % 2 == 0) ? 1900 : 50;
        channels[10] = (pressedCnt[7] % 2 == 0) ? 1900 : 50;
        channels[11] = UINT16_MAX;
        channels[12] = UINT16_MAX;
        channels[13] = UINT16_MAX;
        channels[14] = UINT16_MAX;
        channels[15] = UINT16_MAX;

        packet[0] = 0x0F; 
        // 16 channels of 11 bit data
        packet[1]  = (unsigned char) ((channels[0] & 0x07FF));
        packet[2]  = (unsigned char) ((channels[0] & 0x07FF)>>8   | (channels[1] & 0x07FF)<<3);
        packet[3]  = (unsigned char) ((channels[1] & 0x07FF)>>5   | (channels[2] & 0x07FF)<<6);
        packet[4]  = (unsigned char) ((channels[2] & 0x07FF)>>2);
        packet[5]  = (unsigned char) ((channels[2] & 0x07FF)>>10  | (channels[3] & 0x07FF)<<1);
        packet[6]  = (unsigned char) ((channels[3] & 0x07FF)>>7   | (channels[4] & 0x07FF)<<4);
        packet[7]  = (unsigned char) ((channels[4] & 0x07FF)>>4   | (channels[5] & 0x07FF)<<7);
        packet[8]  = (unsigned char) ((channels[5] & 0x07FF)>>1);
        packet[9]  = (unsigned char) ((channels[5] & 0x07FF)>>9   | (channels[6] & 0x07FF)<<2);
        packet[10] = (unsigned char) ((channels[6] & 0x07FF)>>6   | (channels[7] & 0x07FF)<<5);
        packet[11] = (unsigned char) ((channels[7] & 0x07FF)>>3);
        packet[12] = (unsigned char) ((channels[8] & 0x07FF));
        packet[13] = (unsigned char) ((channels[8] & 0x07FF)>>8   | (channels[9] & 0x07FF)<<3);
        packet[14] = (unsigned char) ((channels[9] & 0x07FF)>>5   | (channels[10] & 0x07FF)<<6);  
        packet[15] = (unsigned char) ((channels[10] & 0x07FF)>>2);
        packet[16] = (unsigned char) ((channels[10] & 0x07FF)>>10 | (channels[11] & 0x07FF)<<1);
        packet[17] = (unsigned char) ((channels[11] & 0x07FF)>>7  | (channels[12] & 0x07FF)<<4);
        packet[18] = (unsigned char) ((channels[12] & 0x07FF)>>4  | (channels[13] & 0x07FF)<<7);
        packet[19] = (unsigned char) ((channels[13] & 0x07FF)>>1);
        packet[20] = (unsigned char) ((channels[13] & 0x07FF)>>9  | (channels[14] & 0x07FF)<<2);
        packet[21] = (unsigned char) ((channels[14] & 0x07FF)>>6  | (channels[15] & 0x07FF)<<5);
        packet[22] = (unsigned char) ((channels[15] & 0x07FF)>>3);
        // flags
        packet[23] = 0x00;

        packet[24] = 0X00;
        
        HAL_BB_ComSendMsg(BB_COM_SESSION_SPI, (uint8_t *)&packet, sizeof(packet));
    }
}

// 按键0 用于校准
static void handleKey0Value(void)
{
    uint32_t u32_Gpioval = 0;
    HAL_GPIO_GetPin(HAL_GPIO_NUM81, &u32_Gpioval);

    if(u32_Gpioval == 0)
    {
        offsetValue[0] = 0;
        offsetValue[1] = 0;
        offsetValue[2] = 0;                                                         
        offsetValue[3] = 0;

        uint8_t i = 10;
        // 取值范围 
        while (i--)
        {
            offsetValue[0] = (offsetValue[0] + HAL_ADC_Read(7, 8)) >> 1;
            offsetValue[1] = (offsetValue[1] + HAL_ADC_Read(8, 6)) >> 1;
            offsetValue[2] = (offsetValue[2] + HAL_ADC_Read(6, 5)) >> 1;
            offsetValue[3] = (offsetValue[3] + HAL_ADC_Read(5, 7)) >> 1;
        }

        offsetValue[0] = 512 - offsetValue[0];
        offsetValue[1] = 512 - offsetValue[1];
        offsetValue[2] = 512 - offsetValue[2];
        offsetValue[3] = 512 - offsetValue[3];
    }
}


static void Key_Debug(void const *argument)
{
    uint32_t u32_Gpioval = 0;

    while (true)
    {   
        handleKey0Value();

        for (size_t i = 1; i < sizeof(pinNo)/sizeof(pinNo[0]); i++)
        {
            u32_Gpioval = -1;

            HAL_GPIO_GetPin(pinNo[i],&u32_Gpioval);

            if (u32_Gpioval == 0 && isPressed[i] == false)
            {
                pressedCnt[i]++;
                isPressed[i] = true;
            }
            else if (u32_Gpioval != 0 && isPressed[i] == true)
            {
                isPressed[i] = false;
            }
            else
            {}
        } 

        HAL_Delay(50);
    }
}



/**
  * @brief  Main program`
  * @param  None
  * @retval None
  */
int main(void)
{

    STRU_HAL_SYS_CTL_CONFIG *pst_cfg;
    HAL_SYS_CTL_Init(pst_cfg);

    /* initialize the uart */

    console_init(0,115200);

    // cf_uart_init();
    // ar_log("ar_uart0 --init----------\r\n");

    dlog_set_output_level(LOG_LEVEL_WARNING);

    DLOG_Critical("cpu0 gnd start!!! \n");

    WATCHDOG_Init();

    USB_Gpio_Ctrl_Init();

    HAL_NV_Init();

    HAL_SD_Init();


    BB_ledGpioInit();
    
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_BB_EVENT, BB_EventHandler);
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_BB_EVENT, BB_grdRcIdEventHandler);
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_USER_CFG_CHANGE, BB_ReplyPcHandler);
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_TO_APP, Grd_AppSysEventHandler);
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_USER_DEFINE, USER_Define_EventHandler);

    HAL_BB_ComRegisterSession(BB_COM_SESSION_1, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, session1_rcvDataHandler);
    
    HAL_BB_ComRegisterSession(BB_COM_SESSION_2, BB_COM_SESSION_PRIORITY_LOW, BB_COM_SESSION_DATA_NORMAL, session2_rcvDataHandler);

    HAL_BB_SpiDataTransInit(64);

    HAL_BB_ComRegisterSession(BB_COM_SESSION_SPI, BB_COM_SESSION_PRIORITY_HIGH, BB_COM_SESSION_DATA_NORMAL, NULL);

    usb_cdc_registerReceiveHandler(ar_usb_cdc_recvdata_handler);


    HAL_USB_ConfigPHY();


    HAL_USB_Init(HAL_USB_PORT_0, HAL_USB_DR_MODE_DEVICE);

    // HAL_USB_Init(HAL_USB_PORT_1, HAL_USB_DR_MODE_DEVICE);//HAL_USB_DR_MODE_OTG


    HAL_SRAM_ReceiveVideoConfig();


    HAL_SRAM_ChannelConfig(ENUM_HAL_SRAM_CHANNEL_TYPE_VIDEO1,
                           0);


    HAL_SRAM_ChannelConfig(ENUM_HAL_SRAM_CHANNEL_TYPE_VIDEO0,
                           1);


    UPGRADE_GNDInit();

    osThreadDef(USBHStatus_Task, USBH_USBHostStatus, osPriorityHigh, 0, 4 * 128);
    osThreadCreate(osThread(USBHStatus_Task), NULL);


    osThreadDef(DLOG_Task, DLOG, osPriorityIdle, 0, 4 * 128);
    osThreadCreate(osThread(DLOG_Task), NULL);

    osThreadDef(IOTask, IO_Task, osPriorityNormal, 0, 16 * 128);
    osThreadCreate(osThread(IOTask), NULL);


    osThreadDef(Usb_Debug, Usr_Usb_Debug, osPriorityIdle, 0, 4 * 128);
    osThreadCreate(osThread(Usb_Debug), NULL);

    osThreadDef(key_Debug, Key_Debug, osPriorityIdle, 0, 4 * 128);
    osThreadCreate(osThread(key_Debug), NULL);

    usr_usb0_interface();
    Wireless_TaskInit(WIRELESS_USE_RTOS);

    // MFI_Init(HAL_GPIO_NUM109, 0xFF, HAL_I2C_COMPONENT_0);

    DLOG_Critical("osKernelStart\r\n");


    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */
    for( ;; )
    {
    }
}
