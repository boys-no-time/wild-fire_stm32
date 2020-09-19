#ifndef _DEBUG_H
#define _DEBUG_H


#define DBG_GPIO_TX         GPIO_Pin_10
#define DBG_GPIO_RX         GPIO_Pin_11
#define DBG_GPIO            GPIOC
#define DBG_UART					  UART4

#define DBG_GPIO_CLK			  RCC_APB2Periph_GPIOC
#define DBG_GPIO_CLK_CMD		RCC_APB2PeriphClockCmd

#define DBG_UART_CLK			  RCC_APB1Periph_UART4
#define DBG_UART_CLK_CMD		RCC_APB1PeriphClockCmd


void DBG_Usart_Init(void);




















#endif

