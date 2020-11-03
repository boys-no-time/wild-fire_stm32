/**
  ******************************************************************************
  * @file    serial_debug.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   Header for serial_debug.c file
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERIAL_DEBUG_H
#define __SERIAL_DEBUG_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef SERIAL_DEBUG
  void DebugComPort_Init(void);
#endif

#endif /* __SERIAL_DEBUG_H */ 