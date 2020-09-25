
#include "inodes_include.h"

/* STM32F107 ETH dirver options */
//#define CHECKSUM_BY_HARDWARE    1       /* 0: disable.  1: use hardware checksum. */
#define RMII_MODE               1       /* 0: MII MODE, 1: RMII MODE. */
#define STM32_ETH_IO_REMAP      0       /* 0: default,  1: remap RXD to PDx. */
#define USE_MCO                 0       /* 0: disable,  1: PA8(MCO) out 25Mhz(MII) or 50Mhz(RMII). */

ETH_InitTypeDef ETH_InitStructure;

#define GET_PHY_LINK_STATUS()		(ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR) & 0x00000004)

__IO uint32_t  EthStatus = 0;
__IO uint8_t EthLinkStatus = 0;

extern struct netif gnetif;

#ifdef USE_DHCP
extern __IO uint8_t DHCP_state;
#endif /* LWIP_DHCP */

static void GPIO_ConfigurationETH(void);
void NVIC_Configuration(void);
static void Ethernet_Configuration(void);
 
#ifndef USE_Delay

void ETH_Delay(__IO uint32_t nCount)
{
    do {
        __asm volatile("nop");
    } while (nCount --);
}
#endif /* USE_Delay*/

static void phy_reset(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    ETH_Delay(72 * 1000);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void ETH_BSP_Config(void)
{
	RCC_ClocksTypeDef RCC_Clocks;

  /* Setup STM32 clock, PLL and Flash configuration) */
  SystemInit();
	
	  /* Enable GPIOs and ADC1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	  /* NVIC configuration */
  NVIC_Configuration(); 
	
  /* Configure the GPIO ports */
  GPIO_ConfigurationETH();

  /* Configure the Ethernet peripheral */
  Ethernet_Configuration();
	
	  /* SystTick configuration: an interrupt every 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);

  /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
  /* The Localtime should be updated during the Ethernet packets processing */
  NVIC_SetPriority (SysTick_IRQn, 1);
	
	  /* Get Ethernet link status*/
  if(GET_PHY_LINK_STATUS())
  {
    EthStatus |= ETH_LINK_FLAG;
  }
	
  }

	/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void Ethernet_Configuration(void)
{ 

  /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM3210C-EVAL  */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

  /* Get HSE clock = 25MHz on PA8 pin (MCO) */
  RCC_MCOConfig(RCC_MCO_HSE);

#elif defined RMII_MODE  /* Mode RMII with STM3210C-EVAL */
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
    /* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
  RCC_PLL3Config(RCC_PLL3Mul_10);
  /* Enable PLL3 */
  RCC_PLL3Cmd(ENABLE);
  /* Wait till PLL3 is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
  {}

  /* Get PLL3 clock on PA8 pin (MCO) */
  RCC_MCOConfig(RCC_MCO_PLL3CLK);

#endif

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);

  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                                                          
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                                                                 
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

  /* Enable the Ethernet Rx Interrupt */
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

}


/*
 * GPIO Configuration for ETH
 AF Output Push Pull:
 - ETH_MDC   : PC1
 - ETH_MDIO  : PA2
 - ETH_TX_EN : PB11
 - ETH_TXD0  : PB12
 - ETH_TXD1  : PB13
 - ETH_TXD2  : PC2
 - ETH_TXD3  : PB8
 - ETH_PPS_OUT / ETH_RMII_PPS_OUT: PB5

 Input (Reset Value):
 - ETH_MII_TX_CLK: PC3
 - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
 - ETH_MII_CRS: PA0
 - ETH_MII_COL: PA3
 - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PA7
 - ETH_MII_RXD0: PC4
 - ETH_MII_RXD1: PC5
 - ETH_MII_RXD2: PB0
 - ETH_MII_RXD3: PB1
 - ETH_MII_RX_ER: PB10

 ***************************************
 For Remapped Ethernet pins
 *******************************************
 Input (Reset Value):
 - ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
 - ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
 - ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
 - ETH_MII_RXD2: PD11
 - ETH_MII_RXD3: PD12
 */
static void GPIO_ConfigurationETH(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#if STM32_ETH_IO_REMAP
    /* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
    GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);
#endif /* STM32_ETH_IO_REMAP */

    /* MII/RMII Media interface selection */
#if (RMII_MODE == 0) /* Mode MII. */
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);
#elif (RMII_MODE == 1)  /* Mode RMII. */
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
#endif /* RMII_MODE */

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* MDIO */
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

        /* MDC */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* MDIO */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } /* MDIO */

    /* TXD */
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

        /* TX_EN */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* TXD0 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* TXD1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

#if (RMII_MODE == 0)
        /* TXD2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* TXD3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* TX_CLK */
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif /* RMII_MODE */
    } /* TXD */

    /* RXD */
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

#if (STM32_ETH_IO_REMAP == 0)
        /* RX_DV/CRS_DV */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* RXD0 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* RXD1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

#if (RMII_MODE == 0)
        /* RXD2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* RXD3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif /* RMII_MODE */
#else
        /* RX_DV/CRS_DV */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        /* RXD0 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        /* RXD1 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

#if (RMII_MODE == 0)
        /* RXD2 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        /* RXD3 */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif /* RMII_MODE */
#endif /* STM32_ETH_IO_REMAP */

#if (RMII_MODE == 0)
        /* CRS */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* COL */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* RX_CLK */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* RX_ER */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif /* RMII_MODE */
    } /* RXD */

#if (USE_MCO == 1)
#if (RMII_MODE == 0) /* Mode MII. */
    /* Get HSE clock = 25MHz on PA8 pin(MCO) */
    RCC_MCOConfig(RCC_MCO_HSE);
#elif (RMII_MODE == 1)  /* Mode RMII. */
    /* Get HSE clock = 25MHz on PA8 pin(MCO) */
    /* set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
    RCC_PLL3Config(RCC_PLL3Mul_10);
    /* Enable PLL3 */
    RCC_PLL3Cmd(ENABLE);
    /* Wait till PLL3 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET) {
    }

    /* Get clock PLL3 clock on PA8 pin */
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif /* RMII_MODE */

    /* MCO pin configuration------------------------------------------------- */
    /* Configure MCO (PA8) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif /* USE_MCO */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    phy_reset();
}





void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
  
}


void ETH_CheckLinkStatus(uint16_t PHYAddress) 
{
	static uint8_t status = 0;
	uint32_t t = GET_PHY_LINK_STATUS();
	
	/* If we have link and previous check was not yet */
	if (t && !status) {
		/* Set link up */
		netif_set_link_up(&gnetif);
		status = 1;
	}	
	/* If we don't have link and it was on previous check */
	if (!t && status) {
		EthLinkStatus = 1;
		/* Set link down */
		netif_set_link_down(&gnetif);
			
		status = 0;
	}
}

/**
  * @brief  Link callback function, this function is called on change of link status.
  * @param  The network interface
  * @retval None
  */
void ETH_link_callback(struct netif *netif)
{
  __IO uint32_t timeout = 0;
	uint32_t tmpreg;
	uint16_t RegValue;
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;

  if(netif_is_link_up(netif))
  {
    /* Restart the auto-negotiation */
    if(ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
    {
      /* Reset Timeout counter */
      timeout = 0;

      /* Enable auto-negotiation */
      ETH_WritePHYRegister(PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);

      /* Wait until the auto-negotiation will be completed */
      do
      {
        timeout++;
      } while (!(ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));  

      /* Reset Timeout counter */
      timeout = 0;

      /* Read the result of the auto-negotiation */
      RegValue = ETH_ReadPHYRegister(PHY_ADDRESS, PHY_SR);

      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((RegValue & PHY_Duplex_Status) != (uint16_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(RegValue & PHY_Speed_Status)
      {
        /* Set Ethernet speed to 10M following the auto-negotiation */
        ETH_InitStructure.ETH_Speed = ETH_Speed_10M; 
      }
      else
      {
        /* Set Ethernet speed to 100M following the auto-negotiation */
        ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
      }

      /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
      /* Get the ETHERNET MACCR value */  
      tmpreg = ETH->MACCR;

      /* Set the FES bit according to ETH_Speed value */ 
      /* Set the DM bit according to ETH_Mode value */ 
      tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);

      /* Write to ETHERNET MACCR */
      ETH->MACCR = (uint32_t)tmpreg;

      _eth_delay_(PHY_WRITE_TO);
      tmpreg = ETH->MACCR;
      ETH->MACCR = tmpreg;
    }

    /* Restart MAC interface */
    ETH_Start();

#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    DHCP_state = DHCP_START;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* USE_DHCP */

    netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);
    
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);    

    EthLinkStatus = 0;
  }
  else
  {
    ETH_Stop();
#ifdef USE_DHCP
    DHCP_state = DHCP_LINK_DOWN;
    dhcp_stop(netif);
#endif /* USE_DHCP */

    /*  When the netif link is down this function must be called.*/
    netif_set_down(&gnetif);
  }
}




