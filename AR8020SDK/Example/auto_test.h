#ifndef __AUTO_TEST_H__
#define __AUTO_TEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define DIV_LINE_PAR              "PAR"
#define DIV_LINE_CUSTOM           "CUSTOM"
#define DIV_LINE_REPORT           "REPORT"
#define DIV_LINE_END              "END"
#define DIV_LINE(argc)            DLOG_Critical("auto_test>-------------------- %s --------------------", argc)


#define PASSED                 "PASSED"
#define FAILURE                "FAILURE"

void AUTO_TEST_PrintResult(uint8_t *result);
void AUTO_TEST_PrintApiRunTime(uint8_t *apiName, uint32_t timeUs);
void AUTO_TEST_TimeRecord(void);
uint64_t AUTO_TEST_CalcTimeDiff(void);

#ifdef __cplusplus
}
#endif

#endif
