#ifndef __ETH_IF_H__
#define __ETH_IF_H__

#ifdef __cplusplus
extern "C"
{
#endif

void ethernetif_input(void *data, uint32_t size);

err_t ethernetif_init(struct netif *netif);


#ifdef __cplusplus
}
#endif


#endif
