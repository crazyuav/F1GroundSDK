#include <string.h>
#include <stdlib.h>
#include "debuglog.h"
#include "serial.h"
#include "cmsis_os.h"
#include "bb_ctrl.h"
#include "test_common.h"
#include "test_bb.h"
#include "hal_ret_type.h"
#include "cmd_line.h"

#include "test_common.h"


STRU_CMD_ENTRY_T g_cmdArray[] =
{
    {5, (f_cmdline)command_BB_add_cmds, "BB_add_cmds",  "<type> <param 0~3>"},
    {2, (f_cmdline)command_set_loglevel, "set_loglevel", "<cpuid> <loglevel>"},
    {7, (f_cmdline)command_BB_GroundconnectToskyByRcId, "BB_Groundconnect", "<RC 0~5 VT 0~1>"},
    {1, (f_cmdline)command_searchRcId, "searchRcId", "<flag_groundAutoSaveRcIdVtId>"},
    {1, (f_cmdline)command_setSubBBch, "SetSubBBch",  "ch"},
    END_OF_CMD_ENTRY
};
