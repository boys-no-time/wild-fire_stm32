/*********************************************************************
Copyright (c) 2014 by Automation Interactive.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/


#ifndef _NEXTAI_COM__H_
#define _NEXTAI_COM__H_

#define NEXTAI_COM_BUF_OVER (-2)

enum {
    NEXTAI_COM_TYPE_MODSERVER,
    NEXTAI_COM_TYPE_MODCLIENT,
    NEXTAI_COM_TYPE_DTU
};


enum {
    NEXTAI_COM1_ID = 1,
    NEXTAI_COM2_ID,
    NEXTAI_COM3_ID,
    NEXTAI_COM4_ID,
    NEXTAI_COM5_ID
};


#define NEXTAI_COM1_NAME "COM1"
#define NEXTAI_COM2_NAME "COM2"
#define NEXTAI_COM3_NAME "COM3"
#define NEXTAI_COM4_NAME "COM4"
#define NEXTAI_COM5_NAME "COM5"

#define NEXTAI_COM_BAUDRATE_115200 (115200)
#define NEXTAI_COM_BAUDRATE_38400 (38400)

#define INODES_COM_USART_WordLength_8b ((unsigned short)0x0000)
#define INODES_COM_USART_WordLength_9b ((unsigned short)0x1000)

#define INODES_COM_USART_StopBits_1 ((unsigned short)0x0000)
#define INODES_COM_USART_StopBits_0_5 ((unsigned short)0x1000)
#define INODES_COM_USART_StopBits_2 ((unsigned short)0x2000)
#define INODES_COM_USART_StopBits_1_5 ((unsigned short)0x3000)

#define INODES_COM_USART_Parity_No ((unsigned short)0x0000)
#define INODES_COM_USART_Parity_Even ((unsigned short)0x0400)
#define INODES_COM_USART_Parity_Odd ((unsigned short)0x0600)

#endif

