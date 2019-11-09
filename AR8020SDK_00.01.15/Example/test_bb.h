#ifndef __TEST_BB_
#define __TEST_BB_

#ifdef __cplusplus
extern "C"
{
#endif


void command_test_BB_uart(char *index_str);

void command_test_BB_spi(char *index_str);

void command_BB_add_cmds(char *cmdstr0, char *cmdstr1, char *cmdstr2, char *cmdstr3);

void command_BB_GroundconnectToskyByRcId(char *rcid0, char *rcid1, char *rcid2, char *rcid3, char *rcid4, char *vtid0, char *vtid1);

void command_searchRcId(char *following);

void command_setTargetPower(char *u8_2gpower,char *u8_5gpower);

void command_setTargetStardard(char *u8_stardard_enum);

void command_SetBbFilter(char *filter);

void command_setSubBBch(char *ch);

#ifdef __cplusplus
}
#endif

#endif
