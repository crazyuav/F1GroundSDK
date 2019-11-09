/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: hal_mfi.h
Description: The external HAL APIs to use the mfi controller.
Author: Artosyn Software Team
Version: 0.0.1
Date: 2017/10/26
History: 
        0.0.1    2017/10/26    The initial version of hal_mfi.h
*****************************************************************************/

#ifndef __HAL_MFI_H__
#define __HAL_MFI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "hal_ret_type.h"
#include "hal_gpio.h"

#define IAP2_MSG_TYPE_SYN        0x80
#define IAP2_MSG_TYPE_ACK        0x40

#define IAP2_MSG_REQ_AUTHENTICATION_CERTIFICATE     0xAA00
#define IAP2_MSG_AUTHENTICATION_CERTIFICATE         0xAA01
#define IAP2_MSG_REQ_AUTHENTICATION_CHALLENGE_RESP  0xAA02
#define IAP2_MSG_AUTHENTICATION_RESP                0xAA03
#define IAP2_MSG_AUTHENTICATION_FAILED              0xAA04
#define IAP2_MSG_AUTHENTICATION_SUCCEEDED           0xAA05
#define IAP2_MSG_START_IDENTIFICATION               0x1D00
#define IAP2_MSG_IDENTIFICATION_INFORMATION         0x1D01
#define IAP2_MSG_IDENTIFICATION_ACCEPTED            0x1D02

#define IAP2_MSG_SYN_ACK                            0xFFFF

#define MFI_I2C_AUTHENTICATION_CONTROL_STATUS       0x10
#define MFI_I2C_CHALLENGE_RESPONSE_DATA_LEN         0x11
#define MFI_I2C_CHALLENGE_RESPONSE_DATA             0x12
#define MFI_I2C_CHALLENGE_DATA_LEN                  0x20
#define MFI_I2C_CHALLENGE_DATA                      0x21
#define MFI_I2C_CERTIFICATE_DATA_LEN                0x30
#define MFI_I2C_CERTIFICATE_DATA                    0x31




typedef struct
{
    uint8_t   start_msb;
    uint8_t   start_lsb;
    uint8_t   len_msb;
    uint8_t   len_lsb;
    uint8_t   ctrl_byte;
    uint8_t   seq_num;
    uint8_t   ack_num;
    uint8_t   session_identifier;
    uint8_t   check_sum;
} STRU_iAP2PacketHeader;


typedef struct
{
    uint8_t   start_msb;
    uint8_t   start_lsb;
    uint8_t   len_msb;
    uint8_t   len_lsb;
    uint8_t   msg_id_msb;
    uint8_t   msg_id_lsb;
} STRU_iAP2ControlDef;


typedef struct
{
    uint8_t   param_len_msb;
    uint8_t   param_len_lsb;
    uint8_t   param_id_msb;
    uint8_t   param_id_lsb;
} STRU_iAP2ParamDef;


typedef struct
{
    uint8_t   session_identifier;
    uint8_t   session_type;
    uint8_t   session_version;
} STRU_iAP2SessionDef;

typedef struct
{
    uint8_t   link_version;
    uint8_t   max_outstanding_packet;
    uint8_t   max_recv_len_msb;
    uint8_t   max_recv_len_lsb;
    uint8_t   retrans_timeout_msb;
    uint8_t   retrans_timeout_lsb;
    uint8_t   cumulative_ack_timeout_msb;
    uint8_t   cumulative_ack_timeout_lsb;
    uint8_t   max_num_retrans;
    uint8_t   max_cumulative_ack;
    STRU_iAP2SessionDef st_iap2_sessions;
    uint8_t   check_sum;
} STRU_iAP2SyncPayload;


HAL_RET_T HAL_MFI_Write(ENUM_HAL_I2C_COMPONENT e_i2cComponent, 
                          uint16_t u16_i2cAddr,
                          uint8_t *pu8_wrData,
                          uint32_t u32_wrSize,
                          uint32_t u32_timeOut);


HAL_RET_T MFI_MFI_Read(ENUM_HAL_I2C_COMPONENT e_i2cComponent, 
                         uint16_t u16_i2cAddr,
                         uint8_t *pu8_wrData,
                         uint8_t  u8_wrSize,
                         uint8_t *pu8_rdData,
                         uint32_t u32_rdSize,
                         uint32_t u32_timeOut);

void MFI_Init(ENUM_HAL_GPIO_NUM e_gpio_rst, ENUM_HAL_GPIO_NUM e_gpio_host, ENUM_HAL_I2C_COMPONENT e_i2c);


#ifdef __cplusplus
}
#endif

#endif

