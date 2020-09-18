#ifndef _DEBUG_H
#define _DEBUG_H


#define DBG_GPIO_TX         GPIO_Pin_9
#define DBG_GPIO_RX         GPIO_Pin_10
#define DBG_GPIO            GPIOA
#define DBG_UART					  USART1

#define DBG_GPIO_CLK			  RCC_APB2Periph_GPIOA
#define DBG_GPIO_CLK_CMD		RCC_APB2PeriphClockCmd

#define DBG_UART_CLK			  RCC_APB2Periph_USART1
#define DBG_UART_CLK_CMD		RCC_APB2PeriphClockCmd


void DBG_Usart_Init(void);




















#endif

