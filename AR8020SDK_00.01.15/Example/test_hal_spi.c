#include "test_hal_spi.h"
#include "hal_gpio.h"

extern unsigned long strtoul(const char *cp, char **endp, unsigned int base);

void command_TestHalSpiInit(unsigned char *ch, unsigned char *br)
{
    STRU_HAL_SPI_INIT st_spiInitInfo;
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned int u32_br = strtoul(br, NULL, 0);
    
    st_spiInitInfo.u16_halSpiBaudr = (uint16_t)(u32_br);
    st_spiInitInfo.e_halSpiPolarity = HAL_SPI_POLARITY_HIGH;
    st_spiInitInfo.e_halSpiPhase = HAL_SPI_PHASE_2EDGE;

    DLOG_Critical("ch:%d br:%d", u32_ch, u32_br);

    HAL_SPI_MasterInit((ENUM_HAL_SPI_COMPONENT)(u32_ch), 
                        &st_spiInitInfo);

    if (5 == u32_ch)
    {
        HAL_GPIO_SetMode(HAL_GPIO_NUM50,HAL_GPIO_PIN_MODE1);
        HAL_GPIO_SetMode(HAL_GPIO_NUM51,HAL_GPIO_PIN_MODE1);
        HAL_GPIO_SetMode(HAL_GPIO_NUM52,HAL_GPIO_PIN_MODE1);
        HAL_GPIO_SetMode(HAL_GPIO_NUM53,HAL_GPIO_PIN_MODE1);
    }
}

void command_TestHalSpiTxRx(unsigned char *ch,  unsigned char *txStartData, unsigned char *txLen, unsigned char *rxLen)
{
    unsigned int i;
    unsigned int u32_ch = strtoul(ch, NULL, 0);
    unsigned char u8_startData = strtoul(txStartData, NULL, 0);
    unsigned char u8_txData[128];
    unsigned char u8_rxData[128];
    unsigned int u32_txLen = strtoul(txLen, NULL, 0);
    unsigned int u32_rxLen = strtoul(rxLen, NULL, 0);

    DLOG_Critical("ch:%d startData:%d txLen:%d  rxLen:%d", u32_ch, u8_startData, u32_txLen, u32_rxLen);

    memset(u8_txData, 0x00, sizeof(u8_txData));
    memset(u8_rxData, 0x00, sizeof(u8_rxData));

    for(i = 0; i < u32_txLen; i++)
    {
        u8_txData[i] = u8_startData + i;
    }

    HAL_SPI_MasterWriteRead((ENUM_HAL_SPI_COMPONENT)(u32_ch), 
                             u8_txData,
                             u32_txLen,
                             u8_rxData,
                             u32_rxLen,
                             10);
}


