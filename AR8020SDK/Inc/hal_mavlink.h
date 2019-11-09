/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: hal_i2c.c
Description: The external HAL APIs to use the  mavlink controller.
Author: Artosyn Software Team
Version: 0.0.1
Date: 2019/09/10
History: 
        0.0.1    2019/09/10    The initial version of hal
*****************************************************************************/

#ifndef __HAL_MAVLINK_H__
#define __HAL_MAVLINK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAVLINK_MSG_ID_RC_CHANNELS 65

typedef struct _input_rc_s {
	uint64_t timestamp_last_signal;
	uint32_t channel_count;
	int32_t rssi;
	uint16_t rc_lost_frame_count;
	uint16_t rc_total_frame_count;
	uint16_t rc_ppm_frame_length;
	uint16_t values[18];
	bool rc_failsafe;
	bool rc_lost;
	uint8_t input_source;
	uint8_t _padding0[3]; // required for logger
} input_rc_s;


typedef struct _rc_msssage_s {
	uint16_t values[12];
} rc_message_s;



// 测试使用 mavlink

// typedef struct __mavlink_rc_channels_t {
// // uint64 timestamp_last_valid					# Timestamp of last valid RC signal
// // float32[18] channels						# Scaled to -1..1 (throttle: 0..1)
// // uint8 channel_count						# Number of valid channels
// // int8[26] function						# Functions mapping
// // uint8 rssi							# Receive signal strength index
// // bool signal_lost						# Control signal lost, should be checked together with topic timeout
// // uint32 frame_drop_count						# Number of dropped frames
//  uint16_t chan1_raw; /*< RC channel 1 value, in microseconds. A value of UINT16_MAX implies the channel is unused.*/
//  uint16_t chan2_raw; /*< RC channel 2 value, in microseconds. A value of UINT16_MAX implies the channel is unused.*/
// } mavlink_rc_channels_t;



// uint8 RC_CHANNELS_FUNCTION_THROTTLE=0 // 油门
// uint8 RC_CHANNELS_FUNCTION_ROLL=1  // 横滚 
// uint8 RC_CHANNELS_FUNCTION_PITCH=2  //  俯仰
// uint8 RC_CHANNELS_FUNCTION_YAW=3  // 方向角
// uint8 RC_CHANNELS_FUNCTION_MODE=4  // 模式
// uint8 RC_CHANNELS_FUNCTION_RETURN=5  // 返回
// uint8 RC_CHANNELS_FUNCTION_POSCTL=6 // 位置控制
// uint8 RC_CHANNELS_FUNCTION_LOITER=7 // 悬停
// uint8 RC_CHANNELS_FUNCTION_OFFBOARD=8 // 离板
// uint8 RC_CHANNELS_FUNCTION_ACRO=9 //  
// uint8 RC_CHANNELS_FUNCTION_FLAPS=10 // 襟翼
// uint8 RC_CHANNELS_FUNCTION_AUX_1=11 // 
// uint8 RC_CHANNELS_FUNCTION_AUX_2=12
// uint8 RC_CHANNELS_FUNCTION_AUX_3=13
// uint8 RC_CHANNELS_FUNCTION_AUX_4=14
// uint8 RC_CHANNELS_FUNCTION_AUX_5=15
// uint8 RC_CHANNELS_FUNCTION_PARAM_1=16
// uint8 RC_CHANNELS_FUNCTION_PARAM_2=17
// uint8 RC_CHANNELS_FUNCTION_PARAM_3_5=18
// uint8 RC_CHANNELS_FUNCTION_RATTITUDE=19  // 
// uint8 RC_CHANNELS_FUNCTION_KILLSWITCH=20 // 
// uint8 RC_CHANNELS_FUNCTION_TRANSITION=21
// uint8 RC_CHANNELS_FUNCTION_GEAR=22
// uint8 RC_CHANNELS_FUNCTION_ARMSWITCH=23
// uint8 RC_CHANNELS_FUNCTION_STAB=24
// uint8 RC_CHANNELS_FUNCTION_MAN=25

// uint64 timestamp_last_valid					# Timestamp of last valid RC signal
// float32[18] channels						# Scaled to -1..1 (throttle: 0..1)
// uint8 channel_count						# Number of valid channels
// int8[26] function						# Functions mapping
// uint8 rssi							# Receive signal strength index
// bool signal_lost						# Control signal lost, should be checked together with topic timeout
// uint32 frame_drop_count						# Number of dropped frames

// void mavlink_msg_rc_channels_send_struct(const input_rc_s *rc_channels, uint8_t *pbufferr,  uint16_t *length);


void mavlink_msg_rc_channels_send_struct(const input_rc_s *rc_channels, uint8_t *pbufferr,  uint16_t *length);

// void mavlink_msg_rc_channels_send_struct(const mavlink_rc_channels_t *rc_channels, uint8_t *pbufferr,  uint16_t *length);

#ifdef __cplusplus
}
#endif

#endif
