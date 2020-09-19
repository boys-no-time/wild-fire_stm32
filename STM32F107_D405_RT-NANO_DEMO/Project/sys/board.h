#ifndef __BOARD_H__
#define __BOARD_H__

#include "inodes_include.h"

/* board configuration */
// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
// 	<i>Default: 1
#define STM32_USE_SDIO			0

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define STM32_SRAM_SIZE         64
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

#define RT_USING_UART1

#define RT_USING_UART4

#ifdef INODES_HAVE_ETH_BT
	#define RT_USING_UART2
#endif

//#define RT_USING_SPI1

#define ON   1
#define OFF  0


#define LED_STATUS 	1
#define LED_NET 		2
#define LED_WIFI 		3
#define LED_BL 	4


#define SIGN_BEEP_ACT_ON   		GPIO_SetBits(GPIOD,GPIO_Pin_1)
#define SIGN_BEEP_ACT_OFF   	GPIO_ResetBits(GPIOD,GPIO_Pin_1)


void Hal_Led_Init(void);
void LED_ACT_OFFON(uint8_t cmd,uint8_t on);

void nextai_dataout_config(void);
extern void inodes_dataout_set(rt_uint8_t out_num, rt_uint8_t value);

extern void rt_hw_board_init(void);
extern void rt_hw_usart_init(void);

void  delay(uint32_t nCount);
char *board_getver(void);

#endif

