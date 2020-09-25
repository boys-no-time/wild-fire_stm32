#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "inodes_include.h"
#include "stm32_eth.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

void Set_MAC_Address(unsigned char* macadd);
u32_t ETH_TxPkt_ChainMode(u16_t FrameLength);
u32_t ETH_GetCurrentTxBuffer(void);

FrameTypeDef ETH_RxPkt_ChainMode(void);
	
#ifdef SERVER

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
#define MAC_ADDR5 0x01

#else

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
//#define MAC_ADDR5 0x02
#define MAC_ADDR5 0x03
//#define MAC_ADDR5 0x04

#endif

#endif
