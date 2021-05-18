/**
  ******************************************************************************
  * @file    stm324xg_eval.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   This file contains definitions for STM324xG_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xG_ATK_H
#define __STM32F4xG_ATK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32_eval_legacy.h"
   
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_ATK
  * @{
  */

/** @addtogroup STM324xG_ATK
  * @{
  */
      
/** @addtogroup STM324xG_ATK_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM324xG_ATK_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_TypeDef;

typedef enum 
{ 
  JOY_NONE = 0,
  JOY_SEL = 1,
  JOY_DOWN = 2,
  JOY_LEFT = 3,
  JOY_RIGHT = 4,
  JOY_UP = 5
} JOYState_TypeDef
;


/**
  * @}
  */ 

/** @defgroup STM324xG_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** 
  * @brief  Define for STM324xG_EVAL board  
  */ 
#if !defined (USE_STM324xG_EVAL)
 #define USE_STM324xG_EVAL
#endif

/** @addtogroup STM324xG_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_6
#define LED1_GPIO_PORT                   GPIOG
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOG  
  
#define LED2_PIN                         GPIO_Pin_8
#define LED2_GPIO_PORT                   GPIOG
#define LED2_GPIO_CLK                    RCC_AHB1Periph_GPIOG  
  
#define LED3_PIN                         GPIO_Pin_9
#define LED3_GPIO_PORT                   GPIOI
#define LED3_GPIO_CLK                    RCC_AHB1Periph_GPIOI  
  
#define LED4_PIN                         GPIO_Pin_7
#define LED4_GPIO_PORT                   GPIOC
#define LED4_GPIO_CLK                    RCC_AHB1Periph_GPIOC

/**
  * @}
  */ 
  
/** @addtogroup STM324xG_EVAL_LOW_LEVEL_BUTTON
  * @{
  */
#define KEY_ON	1
#define KEY_OFF	0

/**
 * @brief Wakeup push-button
 */
#define WAKEUP_BUTTON_PIN                GPIO_Pin_0
#define WAKEUP_BUTTON_GPIO_PORT          GPIOA
#define WAKEUP_BUTTON_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define WAKEUP_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define WAKEUP_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource0
#define WAKEUP_BUTTON_EXTI_IRQn          EXTI0_IRQn 

/**
 * @brief Key push-button
 */
#define KEY_PE0_PIN                   GPIO_Pin_3
#define KEY_PE0_GPIO_PORT             GPIOE
#define KEY_PE0_GPIO_CLK              RCC_AHB1Periph_GPIOE
//#define KEY_PE0_EXTI_LINE             EXTI_Line4
//#define KEY_PE0_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOE
//#define KEY_PE0_EXTI_PIN_SOURCE       EXTI_PinSource4
//#define KEY_PE0_EXTI_IRQn             EXTI4_IRQn


/**
  * @}
  */ 

/** @addtogroup STM324xG_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;	
	


/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define DEBUG_USART                      	 USART1
#define DEBUG_USART_IRQn                   USART1_IRQn
#define DEBUG_USART_CLK                    RCC_APB2Periph_USART1

#define DEBUG_USART_TX_PIN                 GPIO_Pin_9
#define DEBUG_USART_TX_GPIO_PORT           GPIOA
#define DEBUG_USART_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_SOURCE              GPIO_PinSource9
#define DEBUG_USART_TX_AF                  GPIO_AF_USART1

#define DEBUG_USART_RX_PIN                 GPIO_Pin_10
#define DEBUG_USART_RX_GPIO_PORT           GPIOA
#define DEBUG_USART_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_SOURCE              GPIO_PinSource10
#define DEBUG_USART_RX_AF                  GPIO_AF_USART1
/**
 * @brief Definition for COM port2, connected to USART3
 */ 
#define ATK_COM3                        USART3
#define ATK_COM3_IRQn                   USART3_IRQn
#define ATK_COM3_CLK                    RCC_APB1Periph_USART3

#define ATK_COM3_TX_PIN                 GPIO_Pin_10
#define ATK_COM3_TX_GPIO_PORT           GPIOB
#define ATK_COM3_TX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define ATK_COM3_TX_SOURCE              GPIO_PinSource10
#define ATK_COM3_TX_AF                  GPIO_AF_USART3

#define ATK_COM3_RX_PIN                 GPIO_Pin_11
#define ATK_COM3_RX_GPIO_PORT           GPIOB
#define ATK_COM3_RX_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define ATK_COM3_RX_SOURCE              GPIO_PinSource11
#define ATK_COM3_RX_AF                  GPIO_AF_USART3






/**
  * @}
  */ 

/** @addtogroup STM324xG_EVAL_LOW_LEVEL_SD_FLASH
  * @{
  */ 
/**
  * @brief  SD FLASH SDIO Interface
  */
#define SD_DETECT_PIN                    GPIO_Pin_13                 /* PH.13 */
#define SD_DETECT_GPIO_PORT              GPIOH                       /* GPIOH */
#define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOH
   
#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
/** 
  * @brief  SDIO Intialization Frequency (400KHz max)
  */
#define SDIO_INIT_CLK_DIV                ((uint8_t)0x76)
/** 
  * @brief  SDIO Data Transfer Frequency (25MHz max) 
  */
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x0) 

#define SD_SDIO_DMA                   DMA2
#define SD_SDIO_DMA_CLK               RCC_AHB1Periph_DMA2
 
#define SD_SDIO_DMA_STREAM3	          3
//#define SD_SDIO_DMA_STREAM6           6

#ifdef SD_SDIO_DMA_STREAM3
 #define SD_SDIO_DMA_STREAM            DMA2_Stream3
 #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
 #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
 #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
 #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
 #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF3 
 #define SD_SDIO_DMA_IRQn              DMA2_Stream3_IRQn
 #define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream3_IRQHandler 
#elif defined SD_SDIO_DMA_STREAM6
 #define SD_SDIO_DMA_STREAM            DMA2_Stream6
 #define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
 #define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
 #define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
 #define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
 #define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
 #define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF6 
 #define SD_SDIO_DMA_IRQn              DMA2_Stream6_IRQn
 #define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream6_IRQHandler
#endif /* SD_SDIO_DMA_STREAM3 */

/**
  * @}
  */ 
  
/** @addtogroup STM324xG_EVAL_LOW_LEVEL_I2C_EE
  * @{
  */
/**
  * @brief  I2C EEPROM Interface pins
  */  
#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
#define sEE_I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sEE_I2C_SCL_SOURCE               GPIO_PinSource6
#define sEE_I2C_SCL_AF                   GPIO_AF_I2C1
#define sEE_I2C_SDA_PIN                  GPIO_Pin_9                  /* PB.09 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sEE_I2C_SDA_SOURCE               GPIO_PinSource9
#define sEE_I2C_SDA_AF                   GPIO_AF_I2C1
#define sEE_M24C64_32

#define sEE_I2C_DMA                      DMA1   
#define sEE_I2C_DMA_CHANNEL              DMA_Channel_1
#define sEE_I2C_DMA_STREAM_TX            DMA1_Stream6
#define sEE_I2C_DMA_STREAM_RX            DMA1_Stream0   
#define sEE_I2C_DMA_CLK                  RCC_AHB1Periph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005410)
#define sEE_USE_DMA
   
#define sEE_I2C_DMA_TX_IRQn              DMA1_Stream6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Stream0_IRQn
#define sEE_I2C_DMA_TX_IRQHandler        DMA1_Stream6_IRQHandler
#define sEE_I2C_DMA_RX_IRQHandler        DMA1_Stream0_IRQHandler   
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   
   
#define sEE_TX_DMA_FLAG_FEIF             DMA_FLAG_FEIF6
#define sEE_TX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF6
#define sEE_TX_DMA_FLAG_TEIF             DMA_FLAG_TEIF6
#define sEE_TX_DMA_FLAG_HTIF             DMA_FLAG_HTIF6
#define sEE_TX_DMA_FLAG_TCIF             DMA_FLAG_TCIF6
#define sEE_RX_DMA_FLAG_FEIF             DMA_FLAG_FEIF0
#define sEE_RX_DMA_FLAG_DMEIF            DMA_FLAG_DMEIF0
#define sEE_RX_DMA_FLAG_TEIF             DMA_FLAG_TEIF0
#define sEE_RX_DMA_FLAG_HTIF             DMA_FLAG_HTIF0
#define sEE_RX_DMA_FLAG_TCIF             DMA_FLAG_TCIF0
   
#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1  

/* Time constant for the delay caclulation allowing to have a millisecond 
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 168MHz then sEE_TIME_CONST should be 168. */
#define sEE_TIME_CONST                   168 
/**
  * @}
  */  
/**
  * @}
  */ 
  
/** @defgroup STM324xG_EVAL_LOW_LEVEL_Exported_Macros
  * @{
  */  
/**
  * @}
  */ 


/** @defgroup STM324xG_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);

void STM_ATK_KEY_Init(void);
uint8_t STM_ATK_KEY_Scan(GPIO_TypeDef * GPIOx,uint16_t GPIO_Pin);

void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct); 
void DEBUG_USART_Init(void);

void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
void sEE_LowLevel_DeInit(void);
void sEE_LowLevel_Init(void); 
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM324xG_ATK_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
