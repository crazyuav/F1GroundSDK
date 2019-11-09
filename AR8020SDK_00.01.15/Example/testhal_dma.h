#ifndef _TESTHAL_DMA_H
#define _TESTHAL_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif


void command_dma(char * u32_src, char *u32_dst, char *u32_byteNum);
void command_dma_asyn(char *cycle, char *u32_byteNum);
void command_test_dma_loop(char * u32_src, char *u32_dst, char *u32_byteNum);
void command_test_dma_driver(char * u32_src, char *u32_dst, char *u32_byteNum, 
											char* u32_ms);
void command_test_dma_user(char * u32_src, char *u32_dst, char *u32_byteNum, 
											char* u32_ms);

void test_dma_task_cpu0(void);

void test_dma_cpu1(void);


#ifdef __cplusplus
}
#endif 

#endif
