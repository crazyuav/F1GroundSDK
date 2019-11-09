#ifndef __TEST_UPGRADE_H__
#define __TEST_UPGRADE_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define RDWR_SECTOR_SIZE                    (0x1000)
#define APPLICATION_IMAGE_START0            (0x10028000)

void upgrade(char *filepath);

void UPGRADE_LocalUpgrade(void const *argument);

void gndforskyupgrade(char *filepath);

void UPGRADE_GndForSky(void const *argument);

#ifdef __cplusplus
}
#endif

#endif
