#ifndef __BSP107_ETH_H
#define __BSP107_ETH_H

#include "inodes_include.h"

#define LAN8720A_PHY        
#define PHY_ADDRESS		0 


/* Ethernet Flags for EthStatus variable */   
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */

#ifndef USE_Delay
void ETH_Delay(__IO uint32_t nCount);
#endif /* USE_Delay*/

void  ETH_BSP_Config(void);
void ETH_link_callback(struct netif *netif);
void ETH_CheckLinkStatus(uint16_t PHYAddress);

#ifdef __cplusplus
}
#endif

#endif /* __BSP107_ETH_H */

