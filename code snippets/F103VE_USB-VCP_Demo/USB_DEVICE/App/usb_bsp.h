#ifndef __USB_BSP_H
#define __USB_BSP_H
#include <stdint.h>


#define USB_USART_TXFIFO_SIZE   1024	//USB虚拟串口发送FIFO大小		
#define USB_USART_REC_LEN	 	200		//USB串口接收缓冲区最大字节数

extern uint8_t  USB_USART_RX_BUF[USB_USART_REC_LEN]; //接收缓冲,最大USB_USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USB_USART_RX_STA;   					//接收状态标记

void Enter_LowPowerMode(void);
void USB_Reset(void);
uint8_t USB_GetStatus(void);

void USB_USART_SendData(uint8_t *data);
void USB_Printf(char* fmt,...);

void USB_To_USART_Send_Data(uint8_t* data_buffer, uint32_t Nb_bytes);
#endif

