#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include <stdio.h>
#include "stm32f4xx.h"

#include "main.h"
#include "target_config.h"

/*****************************************************************
UART端口定义
*****************************************************************/
#define TARGET_PRINT_COM	huart1
#define TARGET_DEV_COM		huart2
#define TARGET_NET_COM		huart3

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/*****************************************************************
UART串口参数配置定义
*****************************************************************/
typedef struct _target_com_param_
{
		uint32_t target_baudrate;
		uint32_t target_wordlenth;
		uint32_t target_stopbits;
		uint32_t target_parity;
}target_com_param;


int  sendchar(uint8_t ch);
int  getkey(void); 

void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
#endif /* __USART1_H */

/*********************************************END OF FILE**********************/
