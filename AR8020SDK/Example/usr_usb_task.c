#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sys_event.h"
#include "debuglog.h"
#include "hal_usb_device.h"
#include "systicks.h"
#include "usr_usb_task.h"
#include "hal.h"
#include "hal_bb.h"
#include "cmsis_os.h"
#include "usr_wir_core.h"
#include "usr_protocol.h"
#include "wireless_interface.h"
#include "hal_sys_ctl.h"
#include "usr_sbus_uart_task.h"

extern void CDC_POLLING(void);

extern void cdc_registerReceiveHandler(Handler handler);

extern void cdc_sendData(uint8_t *dataBuffer, uint16_t datalen);

char usb_plug_flag[2] = {1,1};
//char HAL_SRAM_VIDEO_CHANNEL_0 = 0;
osSemaphoreId bb_send_semaphore_id;

int usb_rec = 0;
int usb_send = 0;
static int wir_rec = 0;
static int wir_send = 0;

#define Max_Rx_Usb 64

typedef struct USB_RX{

    volatile uint32_t   rec_fifo_size;
    char                rec_fifo_buf[3200] __attribute__ ((aligned (4)));//aline 4
    volatile uint32_t   rec_max_wr_counter;
    uint32_t            rec_buf_filled_len[50];
    volatile uint32_t   rec_rd_index;
    volatile uint32_t   rec_wr_index;

}USB_RX;

typedef struct USB_TX{

    volatile uint32_t   rec_fifo_size;
    char                rec_fifo_buf[3200] __attribute__ ((aligned (4)));//aline 4
    volatile uint32_t   rec_max_wr_counter;
    uint32_t            rec_buf_filled_len[50];
    volatile uint32_t   rec_rd_index;
    volatile uint32_t   rec_wr_index;

}USB_TX;

USB_RX usb_rx3;
USB_TX usb_tx3;

char bypass_to_host[512]	__attribute__ ((aligned (4)));

void Usb_Rx3_Init(void)
{
    memset(&usb_rx3,0,sizeof(usb_rx3));
    usb_rx3.rec_max_wr_counter = 50;
    usb_rx3.rec_fifo_size = 64;
}

void Usb_Tx3_Init(void)
{
    memset(&usb_tx3,0,sizeof(usb_tx3));
    usb_tx3.rec_max_wr_counter = 50;
    usb_tx3.rec_fifo_size = Max_Rx_Usb;
}

void Rx3_Usb_Buffer_Push(char *buffer,uint32_t count)
{
    int index;

    if((usb_rx3.rec_rd_index < (usb_rx3.rec_wr_index + 10)) && (usb_rx3.rec_rd_index > usb_rx3.rec_wr_index))
    {
        DLOG_Critical("usb rec full\n");
        return;
    }

    index = usb_rx3.rec_wr_index;

    memcpy((void *)(usb_rx3.rec_fifo_buf + index * usb_rx3.rec_fifo_size),buffer,count);
    usb_rx3.rec_buf_filled_len[index] = count;

    if (++usb_rx3.rec_wr_index == usb_rx3.rec_max_wr_counter)
        usb_rx3.rec_wr_index= 0;
}

void Tx3_Usb_Buffer_Push(char *buffer,uint32_t count)
{
    int index;

    if((usb_tx3.rec_rd_index < (usb_tx3.rec_wr_index + 10)) && (usb_tx3.rec_rd_index > usb_tx3.rec_wr_index))
    {
        DLOG_Critical("usb send full\n");
        return;
    }

    index = usb_tx3.rec_wr_index;

    memcpy((void *)(usb_tx3.rec_fifo_buf + index * usb_tx3.rec_fifo_size),buffer,count);
    usb_tx3.rec_buf_filled_len[index] = count;

    if (++usb_tx3.rec_wr_index == usb_tx3.rec_max_wr_counter)
        usb_tx3.rec_wr_index = 0;
}

uint32_t Usb_Rx3_Read(char *buffer, uint32_t count)
{
    uint32_t cnt = count;
    uint32_t cnt2;
    int index;
    int checkindex;
    int total = 0;

#if 0
    checkindex = usb_rx3.rec_rd_index;
    while(checkindex != usb_rx3.rec_wr_index)
    {
        total += usb_rx3.rec_buf_filled_len[checkindex];
        if(total >= cnt)
            break;
	if (++checkindex == usb_rx3.rec_max_wr_counter)
                checkindex = 0;
    }

    if(total >= 4)
    {
        if(cnt >= total)
            cnt = total & 0xfffffffc;
        else
            cnt = cnt & 0xfffffffc;
    }
    else
        return 0; 
#endif
    cnt2 = cnt;

    while(cnt > 0)
    {

        if(usb_rx3.rec_rd_index == usb_rx3.rec_wr_index)
            break;
        index = usb_rx3.rec_rd_index;

        if(cnt >= usb_rx3.rec_buf_filled_len[index])
        {
            memcpy(buffer,&usb_rx3.rec_fifo_buf[index*usb_rx3.rec_fifo_size],usb_rx3.rec_buf_filled_len[index]);
            buffer += usb_rx3.rec_buf_filled_len[index];
            cnt -= usb_rx3.rec_buf_filled_len[index];
            if (++usb_rx3.rec_rd_index == usb_rx3.rec_max_wr_counter)
                usb_rx3.rec_rd_index=0;
        }
        else
        {
            memcpy(buffer,(const void *)(&usb_rx3.rec_fifo_buf[index*usb_rx3.rec_fifo_size]),cnt);
            buffer += cnt;
            usb_rx3.rec_buf_filled_len[index] -= cnt;
            memcpy(&usb_rx3.rec_fifo_buf[index*usb_rx3.rec_fifo_size],&usb_rx3.rec_fifo_buf[index*usb_rx3.rec_fifo_size+cnt],usb_rx3.rec_buf_filled_len[index]);
            cnt = 0;
        }
    }

    return (cnt2 - cnt);
}

uint32_t Usb_Tx3_Read(char *buffer,uint32_t count)
{
    uint32_t cnt = count;
    uint32_t cnt2;
    int index;
    int checkindex;
    int total = 0;

/*
    checkindex = usb_tx3.rec_rd_index;
    while(checkindex != usb_tx3.rec_wr_index)
    {
        total += usb_tx3.rec_buf_filled_len[checkindex];
        if(total >= cnt)
            break;
	if (++checkindex == usb_tx3.rec_max_wr_counter)
                checkindex = 0;
    }

    if(total >= 4)
    {
        if(cnt >= total)
            cnt = total & 0xfffffffc;
        else
            cnt = cnt & 0xfffffffc;
    }
    else
        return 0; */

    cnt2 = cnt;

    while(cnt > 0)
    {

        if(usb_tx3.rec_rd_index == usb_tx3.rec_wr_index)
            break;
        index = usb_tx3.rec_rd_index;

        if(cnt >= usb_tx3.rec_buf_filled_len[index])
        {
            memcpy(buffer,&usb_tx3.rec_fifo_buf[index*usb_tx3.rec_fifo_size],usb_tx3.rec_buf_filled_len[index]);
            buffer += usb_tx3.rec_buf_filled_len[index];
            cnt -= usb_tx3.rec_buf_filled_len[index];
            if (++usb_tx3.rec_rd_index == usb_tx3.rec_max_wr_counter)
                usb_tx3.rec_rd_index=0;	
        }
        else
        {
            memcpy(buffer,(const void *)(&usb_tx3.rec_fifo_buf[index*usb_tx3.rec_fifo_size]),cnt);
            buffer += cnt;
            usb_tx3.rec_buf_filled_len[index] -= cnt;
            memcpy(&usb_tx3.rec_fifo_buf[index*usb_tx3.rec_fifo_size],&usb_tx3.rec_fifo_buf[index*usb_tx3.rec_fifo_size+cnt],usb_tx3.rec_buf_filled_len[index]);
            cnt = 0;
        }

    }

    return (cnt2 - cnt);
}

uint32_t usb_bypass_write(void *pbuf, uint32_t data_len)
{
    usb_rec += data_len;
    
    while(data_len > 0)
    {
        if(data_len >= Max_Rx_Usb)
        {
            Tx3_Usb_Buffer_Push((uint8_t *)pbuf,Max_Rx_Usb);
            pbuf += Max_Rx_Usb;
            data_len -= Max_Rx_Usb;
        }
        else
        {
            Tx3_Usb_Buffer_Push((uint8_t *)pbuf,data_len);
            data_len = 0;
        }
    }

}

uint32_t usb_bypass_read(void *pbuf, uint32_t max_len)
{
    uint32_t len;
    len = Usb_Rx3_Read(pbuf, max_len);
    usb_send += len;
    return len;
}

void Usb_Rcv_Data_Handler(void *pbuf, uint32_t data_len, uint8_t port_id)
{
    int i;
    if(0 == port_id)
    {
        usb_rec += data_len;

        while(data_len > 0)
        {
            if(data_len >= Max_Rx_Usb)
            {
                Tx3_Usb_Buffer_Push((uint8_t *)pbuf,Max_Rx_Usb);
                pbuf += Max_Rx_Usb;
                data_len -= Max_Rx_Usb;
            }
            else
            {
                Tx3_Usb_Buffer_Push((uint8_t *)pbuf,data_len);
                data_len = 0;
            }
        }
    }
}

static void BB_Com_Rcv_Data_Handler(void *p)
{
    uint32_t cnt = 0;
    uint8_t buffer[200];
    uint8_t *pbuf = buffer;
    HAL_RET_T ret;
    
    ret = HAL_BB_ComReceiveMsg(BB_COM_SESSION_1, buffer, sizeof(buffer),&cnt);
    if(ret != HAL_OK)
    {
        DLOG_Error("failed read bbcom");
        return;
    }
        
    wir_rec += cnt;

    while(cnt > 0)
    {
        if(cnt >= Max_Rx_Usb)
        {
            Rx3_Usb_Buffer_Push((uint8_t *)pbuf,Max_Rx_Usb);
            pbuf += Max_Rx_Usb;
            cnt -= Max_Rx_Usb;
        }
        else
        {
            Rx3_Usb_Buffer_Push((uint8_t *)pbuf,cnt);
            cnt = 0;
        }
    }
}
#define CMD_HANDLER_CNT 9
STRU_WIRELESS_USER_HANDLER   Cmd_Handler[CMD_HANDLER_CNT] = {
    {CMD_VIDEO_BUF_DEEP, cmd_ack_handle},
    {CMD_GRD_MOD_STATUS, cmd_ack_handle},
    {CMD_SKY_MOD_STATUS, cmd_ack_handle},
    {CMD_DBUG_INFO, cmd_ack_handle},
    {CMD_READ_USB_BYPASS, cmd_usb_bypass_read_handle},
    {CMD_WRITE_USB_BYPASS, cmd_usb_bypass_write_handle},
    {CMD_READ_UART5_BYPASS, cmd_usb_bypass_read_uart5_handle},
    {CMD_WRITE_UART5_BYPASS, cmd_usb_bypass_write_uart5_handle},
    {CMD_GET_AUTOTEST_STATUS, cmd_ack_handle},
};

void bitrate_change_callback(void* p)
{
    uint8_t br_idx  = ((STRU_SysEvent_BB_ModulationChange *)p)->encoder_brcidx;
    uint8_t ch      = ((STRU_SysEvent_BB_ModulationChange *)p)->u8_bbCh;

    if (0 == ch)
    {
        enc_brcidx_change(br_idx);
        DLOG_Critical("H264 bitidx ch0: %d \r\n", br_idx);
    }
}

void usb_plug_out_callback(void* p)
{
    DLOG_Warning("usb has plug out\n");
    HAL_SRAM_DisableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_0);
    HAL_SRAM_DisableSkyBypassVideo(HAL_SRAM_VIDEO_CHANNEL_1);
    usb_plug_flag[0] = 1;
    usb_plug_flag[1] = 1;
}

uint32_t usb_recv_size[2];
uint32_t get_usb_recv_size0(void)
{
    return usb_recv_size[0];
}

uint32_t get_usb_recv_size1(void)
{
    return usb_recv_size[1];
}

void receive_video_stream(void *video_buff, uint32_t length, uint8_t port_id)
{
    volatile static uint32_t cur_video_ch=0,cur_video_len=0,cur_send_video_len=0;
    volatile static int state = 0,len_byte_cnt=0;
    uint8_t *pbuf = (uint8_t *)video_buff;
    int skip,i,msg_len,leave_len;
    const ENUM_HAL_SRAM_VIDEO_CHANNEL usb_sram_revert[2] = {HAL_SRAM_VIDEO_CHANNEL_1,HAL_SRAM_VIDEO_CHANNEL_0};

    if(length == 0)
    {
        DLOG_Warning("len err %d",length);
        return;
    }
    
    i=0;
    skip=0;
    //DLOG_Warning("%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,",pbuf[0],
    //    pbuf[1],pbuf[2],pbuf[3],pbuf[4],pbuf[5],pbuf[6],pbuf[7],pbuf[8],pbuf[9],pbuf[10],pbuf[11]);
        
    do{
        if(state == 0)
        {
            if(pbuf[i] == 0x12)
            {
                state = 1;
            }
            else
            {
                skip++;
            }
            i++;
        }
        else if(state == 1)
        {
            if(pbuf[i] == 0x34)
            {
                state = 2;
                i++;
            }
            else
            {
                state = 0;
            }
        }
        else if(state == 2)
        {
            if(pbuf[i] == 0x56)
            {
                state = 3;
                i++;
            }
            else
            {
                state = 0;
            }
        }
        else if(state == 3)
        {
            if(skip > 0)
            {
                DLOG_Warning("head skip %d",skip*4);
            }

            cur_video_ch = pbuf[i];
            if(cur_video_ch < 2)
            {
                state = 4;
                i++;
                len_byte_cnt = 0;
                cur_video_len = 0;
            }
            else
            {
                DLOG_Error("video ch error %d",cur_video_ch);
                state = 0;
            }
        }
        else if(state == 4)
        {
            cur_video_len += (pbuf[i] << (8*(3-len_byte_cnt)));
            len_byte_cnt++;
            i++;
            if(len_byte_cnt >= 4)
            {
                state = 5;
            }
        }
        else if(state == 5)
        {
            leave_len =  cur_video_len - cur_send_video_len;
            if(i + leave_len <= length)
            {
                msg_len = leave_len;
            }
            else
            {
                msg_len = length - i;
            }
            
            if(usb_plug_flag[cur_video_ch] == 1)
            {
                HAL_SRAM_EnableSkyBypassVideo(usb_sram_revert[cur_video_ch]);
                usb_plug_flag[cur_video_ch] = 0;
                DLOG_Warning("usb video%d has plug in\n",cur_video_ch);
            }
            
            /* received video, now transfer video stream to ground */
            if(usb_plug_flag[cur_video_ch] == 0)
            {
               HAL_SRAM_TransferBypassVideoStream(usb_sram_revert[cur_video_ch], video_buff+i, msg_len);
               usb_recv_size[cur_video_ch] += msg_len;
            }

            i += msg_len;
            cur_send_video_len += msg_len;
            if(cur_send_video_len == cur_video_len)
            {
                state = 0;
                cur_send_video_len = 0;
            }
            else if(cur_send_video_len > cur_video_len)
            {
                DLOG_Error("video len err");
            }

        }
    }while(i < length);
}

void bypass_encoder_init(void)
{
    //SYS_EVENT_RegisterHandler(SYS_EVENT_ID_BB_SUPPORT_BR_CHANGE, bitrate_change_callback);
    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_USB_PLUG_OUT, usb_plug_out_callback);
    HAL_USB_RegisterEncoderBypassVideo(receive_video_stream);
}

static void COMTASK_Rx3Function(void const *argument)
{
    STRU_HAL_SYS_CTL_CONFIG *pst_cfg;
    int ret;
    int max_wir_len = 0;
    HAL_RET_T ret_t;
    char buffer[200];

    HAL_SYS_CTL_GetConfig( &pst_cfg);
    if(pst_cfg->u8_workMode == 0)
        max_wir_len = 200;
    else
    {
        max_wir_len = 72 - get_fac_spi_num() - 2;
        DLOG_Warning("uplink pack len %d",max_wir_len);
    }

    while (1)
    {

        ret = Usb_Tx3_Read(buffer,max_wir_len);
        HAL_Delay(1);        
        if(ret > 0)
        {
            wir_send += ret;
            osSemaphoreWait(bb_send_semaphore_id,0);
            ret_t = HAL_BB_ComSendMsg(BB_COM_SESSION_1,buffer,ret);
            osSemaphoreRelease(bb_send_semaphore_id);
            if(ret_t != HAL_OK)
            {

                DLOG_Error("send bbcom failed");
            }
            //    HAL_Delay(4);
            
            if(ret < (max_wir_len / 4))
                HAL_Delay(4);
            else if(ret < (max_wir_len / 2))
                HAL_Delay(2);
            else if(ret < max_wir_len)
                HAL_Delay(1);
        }
        else
        {
            HAL_Delay(8);
        }

    }
}

//create usb task for usr bypass function
static void COMTASK_Tx3Function(void const *argument)
{
    int ret;
    int32_t i;

    while(1)
    {
        ret = Usb_Rx3_Read(bypass_to_host, 500);
        HAL_Delay(1);
        if(ret > 0)
        {
            usb_send += ret;
            
            for(i = 0;i < 1000;i ++)
            {
                if(HAL_OK == HAL_USB_SendData(bypass_to_host,ret,0,0x83))
                    break;

                DLOG_Critical("usb send err");

                HAL_Delay(10);
            }
            if(i == 1000)
                DLOG_Critical("usb send data error\n");
        }
        else
        {
            HAL_Delay(5);
        }
    }

}



static void Usr_Usb_Debug(void const *argument)
{
    // HAL_Delay(1000);
    while(1)
    {
        HAL_Delay(1000);
        // CDC_POLLING();
        // DLOG_Warning("wir_rec = %d----------usb_send = %d",wir_rec,usb_send);
        // DLOG_Warning("usb_rec = %d----------wir_send = %d",usb_rec,wir_send);
        // DLOG_Warning("usb_video0 = %d----------usb_video1 = %d",usb_recv_size[0],usb_recv_size[1]);
        
    }

}

void usr_usb0_interface(void)
{
    int ret;
    STRU_HAL_SYS_CTL_CONFIG *pst_cfg;
    HAL_SYS_CTL_GetConfig( &pst_cfg);

    Usb_Rx3_Init();


    Usb_Tx3_Init();

    osSemaphoreDef(bb_send_semaphore);
    bb_send_semaphore_id = osSemaphoreCreate(osSemaphore(bb_send_semaphore),1);
#if 0
    osThreadDef(COMTASK_Tx1, COMTASK_Tx3Function, osPriorityHigh, 0, 4 * 128);
    osThreadCreate(osThread(COMTASK_Tx1), NULL);
#endif
       osThreadDef(COMTASK_Rx1, COMTASK_Rx3Function, osPriorityHigh, 0, 4 * 128);
       osThreadCreate(osThread(COMTASK_Rx1), NULL);

    osThreadDef(Usb_Debug, Usr_Usb_Debug, osPriorityIdle, 0, 4 * 128);
    osThreadCreate(osThread(Usb_Debug), NULL);

    ret = HAL_BB_ComRegisterSession(BB_COM_SESSION_1,
                              BB_COM_SESSION_PRIORITY_LOW,
                              BB_COM_SESSION_DATA_NORMAL,
                              BB_Com_Rcv_Data_Handler);

    WIRELESS_INTERFACE_RegisterUserHandler(Cmd_Handler,CMD_HANDLER_CNT);
    #if 0
    HAL_USB_RegisterCustomerRecvData(Usb_Rcv_Data_Handler);
    #endif

    SYS_EVENT_RegisterHandler(SYS_EVENT_ID_BB_SUPPORT_BR_CHANGE, bitrate_change_callback);

    if(pst_cfg->u8_workMode == 0)
        bypass_encoder_init();

}


void usb_cdc_registerReceiveHandler(Handler handler)
{
    cdc_registerReceiveHandler(handler);
}

void usb_cdc_sendData(uint8_t *dataBuffer, uint16_t datalen)
{
    cdc_sendData(dataBuffer, datalen);
}

