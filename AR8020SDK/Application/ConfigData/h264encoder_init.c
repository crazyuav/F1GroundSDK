#include <stdint.h>

#include "h264_encoder.h"
#include "cfg_parser.h"

extern STRU_REG h264_init_reg[18];

STRU_cfgNode vsoc_enc_nodeInfo =
{
    .nodeId       = VSOC_ENC_INIT_ID,
    .nodeElemCnt  = 18,
    .nodeDataSize = sizeof(h264_init_reg)
};


STRU_REG h264_init_reg[18] __attribute__ ((aligned (4)))= 
{
    // view_0
	{(ENC_REG_ADDR+(0x02<<2)), 0x00004D28, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x05<<2)), 0x00030000, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x08<<2)), 0x04040f00, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x09<<2)), 0x00005900, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x0b<<2)), 0x12469422, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x0c<<2)), 0x00FFFF02, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x12<<2)), 0x1D1E0401, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x32<<2)), 0x18212931, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x33<<2)), 0x39414A80, 0xFFFFFFFF},
	// view_1
	{(ENC_REG_ADDR+(0x1b<<2)), 0x00004D28, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x1e<<2)), 0x00030000, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x21<<2)), 0x04040f00, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x22<<2)), 0x00003000, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x24<<2)), 0x12469422, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x25<<2)), 0x01FFFF02, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x2b<<2)), 0x11120401, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x32<<2)), 0x18212931, 0xFFFFFFFF},
	{(ENC_REG_ADDR+(0x33<<2)), 0x39414A80, 0xFFFFFFFF},
};

extern STRU_REG h264_view_0[0];

STRU_cfgNode vsoc_enc_view_0 =
{
    .nodeId       = VSOC_ENC_VIEW_ID_0,
    .nodeElemCnt  = 0,
    .nodeDataSize = 0
};


STRU_REG h264_view_0[0] __attribute__ ((aligned (4)))=
{
    // view_0
};

//////////////////////////////////
extern STRU_REG h264_view_1[0];

STRU_cfgNode vsoc_enc_view_1 =
{
    .nodeId       = VSOC_ENC_VIEW_ID_1,
    .nodeElemCnt  = 0,
    .nodeDataSize = 0
};

STRU_REG h264_view_1[0] __attribute__ ((aligned (4)))=
{
    // view_0
};
