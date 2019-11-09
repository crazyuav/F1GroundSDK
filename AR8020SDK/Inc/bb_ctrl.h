#ifndef __BB_API_
#define __BB_API_

#include "bb_types.h"
#include "bb_customerctx.h"

typedef void (*pfun)(void);

extern uint8_t BB_grd_regs[4][256];
extern uint8_t BB_sky_regs[4][256];

/** 
 * @brief       API for Baseband initialize.
 * @param[in]   en_mode: brief @ENUM_BB_MODE
 */
void BB_init(ENUM_BB_MODE en_mode, STRU_CUSTOMER_CFG *pstru_customerCfg, uint8_t *bb_reg);


/** 
 * @brief       API for set the encoder bitrate Unit:Mbps
 * @param[in]   bitrate_Mbps: select the bitrate unit: Mbps
 * @retval      TURE:  success to add command
 * @retval      FALSE, Fail to add command
 */
void BB_uart10_spi_sel(uint32_t sel_dat);

/** 
 * @brief       
 * @param   
 * @retval      
 * @note      
 */
int BB_GetDevInfo(void);

/** 
 * @brief       
 * @param   
 * @retval      
 * @note      
 */
int BB_SwtichOnOffCh(uint8_t u8_ch, uint8_t u8_data);

void BB_grd_notify_rc_skip_freq(uint8_t *u8_rcfrq);

uint8_t BB_get_bitrateByMcs(ENUM_CH_BW bw, uint8_t u8_mcs);

int BB_InsertCmd(uint8_t type, STRU_WIRELESS_CONFIG_CHANGE *p);

int BB_GetRcId(uint8_t *pu8_rcId, uint8_t bufsize);

/** 
 * @brief       get rc rate
 * @param       none
 * @retval      1: BPSK 1/2, uart max 32bytes
 *              2: QPSK 2/3, uart max 208bytes
 *              0: unknow qam/code_rate
 * @note        None
 */
uint32_t BB_GetRcRate(ENUM_BB_MODE en_mode);

int BB_NormalModePcRwReg(void *p);

int BB_SetTargetPower(uint8_t u8_2g_power,uint8_t u8_5g_power);
int BB_SetTargetPowerWorkMode(ENUM_RF_POWER_WORK_MODE e_power_work_mode);
void BB_set_power(ENUM_RF_BAND band,uint8_t power);


uint8_t BB_get_band_power(ENUM_RF_BAND band);

void BB_SetPowerCloseMode(ENUM_RF_BAND band);

void BB_SetPowerOpenMode(ENUM_RF_BAND band);

void sky_startSweep(ENUM_RF_BAND band);

void sky_SweepProcess(void);

void sky_requestRfBandSwitch(ENUM_RF_BAND e_band);
void BB_Register_lna(pfun open_lna_function, pfun bypass_lna_function);
void BB_open_lna(void);
void BB_bypass_lna(void);
void BB_Lna_AddAgc(uint8_t agca, uint8_t agcb);
void BB_Lna_reset(void);
ENUM_LNA_STATUS BB_Lna_isNeedSwitch(ENUM_RF_BAND band);

#endif
