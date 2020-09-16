

#include "stm32f10x.h"
#include "debug.h"




static void DBG_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    DBG_GPIO_CLK_CMD(DBG_GPIO_CLK, ENABLE);
    
    DBG_UART_CLK_CMD(DBG_UART_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = DBG_GPIO_RX;
    GPIO_Init(DBG_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = DBG_GPIO_TX;
    GPIO_Init(DBG_GPIO, &GPIO_InitStructure);
}

void DBG_Usart_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
		DBG_GPIO_Configuration();	

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DBG_UART, &USART_InitStructure);

    USART_Cmd(DBG_UART, ENABLE);

}


void Delay(uint32_t nCount)
{
    do {
        __asm volatile("nop");
    } while (nCount --);
}



