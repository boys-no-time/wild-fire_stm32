/**
  ******************************************************************************
  * @file    stm32_eval.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   Header file for stm32_eval.c module.
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
#ifndef __STM32_EVAL_H
#define __STM32_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/



typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;   


#if !defined (USE_STM32100B_EVAL) && !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)\
   &&  !defined (USE_STM3210C_EVAL) &&  !defined (USE_STM32L152_EVAL) &&  !defined (USE_STM32100E_EVAL)

#endif


#include "stm32f10x.h"
#include "stm3210c_eval/stm3210c_eval.h"
                     
 

#ifdef __cplusplus
}
#endif


#endif /* __STM32_EVAL_H */

 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
