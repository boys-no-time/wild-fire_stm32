#ifndef __HWCONFIG_H
#define __HWCONFIG_H
#include <stdint.h>
#include "sys.h"


#define USB_USART_TXFIFO_SIZE   1024	//USB虚拟串口发送FIFO大小		
#define USB_USART_REC_LEN	 	200		//USB串口接收缓冲区最大字节数

extern u8  USB_USART_RX_BUF[USB_USART_REC_LEN]; //接收缓冲,最大USB_USART_REC_LEN个字节.末字节为换行符 
extern u16 USB_USART_RX_STA;   					//接收状态标记

void Error_Handler(void);
void Enter_LowPowerMode(void);
void USB_Reset(void);
uint8_t USB_GetStatus(void);

void USB_USART_SendData(u8 *data);
void USB_Printf(char* fmt,...);

void USB_To_USART_Send_Data(u8* data_buffer, u32 Nb_bytes);
#endif

