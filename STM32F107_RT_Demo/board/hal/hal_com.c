/*********************************************************************
Copyright (c) 2020-2028 by inodes. All rights reserved. 
You are not allowed to copy or distribute the code without permission. 
*********************************************************************/
#include "inodes_include.h"

#define UART2_GPIO_TX	    GPIO_Pin_5
#define UART2_GPIO_RX	    GPIO_Pin_6
#define UART2_GPIO          GPIOD

#define RX_SIZE    512
#define TX_SIZE    256

#ifdef INODES_HAVE_ETH_BT
#error "initialize HAL here"

#define COMM_OUT_ENABLE   GPIOD->BSRR = GPIO_Pin_7
#define COMM_OUT_DISABLE  GPIOD->BRR = GPIO_Pin_7

static uint8_t usart_rx_buf[RX_SIZE] __attribute__((aligned(4)));
static uint8_t usart_tx_buf[TX_SIZE] __attribute__((aligned(4)));
static volatile int8_t tx_busy = 0;

static void GPIO_ConfigurationHC(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

static  void USART_Rx_dma_init(void)
{
    DMA_InitTypeDef  DMA_InitStructure_Rx;

    DMA_InitStructure_Rx.DMA_PeripheralBaseAddr = (uint32_t) & (USART2->DR);
    DMA_InitStructure_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure_Rx.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure_Rx.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure_Rx.DMA_M2M = DMA_M2M_Disable ;

    DMA_InitStructure_Rx.DMA_MemoryBaseAddr = (uint32_t)usart_rx_buf;
    DMA_InitStructure_Rx.DMA_BufferSize = (uint16_t)RX_SIZE;

    DMA_Init(DMA1_Channel6, &DMA_InitStructure_Rx);

    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    DMA_Cmd(DMA1_Channel6, ENABLE);

}

static void USART_Tx_dma_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure_Tx;

    DMA_InitStructure_Tx.DMA_PeripheralBaseAddr = (uint32_t) & (USART2->DR);
    DMA_InitStructure_Tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure_Tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure_Tx.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure_Tx.DMA_Priority = DMA_Priority_VeryHigh;

    DMA_InitStructure_Tx.DMA_DIR = DMA_DIR_PeripheralDST;

    DMA_InitStructure_Tx.DMA_MemoryBaseAddr = (uint32_t)usart_tx_buf;
    DMA_InitStructure_Tx.DMA_BufferSize = (uint16_t)TX_SIZE;
    DMA_InitStructure_Tx.DMA_M2M = DMA_M2M_Disable ;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

    DMA_Init(DMA1_Channel7, &DMA_InitStructure_Tx);

    tx_busy  = 0;
}

void HalCom_TxDone(void)
{
    tx_busy  = 0;
    COMM_OUT_DISABLE ;
}

/* TIMn频率计算: TIMCLK=PCLK/(PSC+1). 当前PCLK=72M, 所以TIMCLK=4MHz
** 计一个数的时间=1/TIMCLK=0.25ns, 中断一次的时间=(ARR+1)/TIMCLK=0.5ms */
static void TIM4_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);

    TIM4->CR1 = TIM_CR1_OPM | TIM_CR1_ARPE ;
    TIM4->ARR = 2000  - 1;
    TIM4->PSC =  17;  //72M / 18  = 4M
    TIM4->EGR = 0x01;

    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0);

    TIM4->DIER |= TIM_IT_Update ;
    TIM4->CNT = 0x00;
}

static inline void TIM4_Start(void)
{
    TIM4->CR1 |= TIM_CR1_CEN;
}


void TIM4_IRQHandler(void)
{
    ENTER_IRQ();
    if (TIM4->SR & TIM_IT_Update) {
        HalCom_TxDone();
        TIM4->SR = (uint16_t)~TIM_IT_Update;
    }
    EXIT_IRQ();
}


void DMA1_Channel7_IRQHandler(void)
{
    ENTER_IRQ();
    DMA_Cmd(DMA1_Channel7, DISABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7);
    DMA_ClearITPendingBit(DMA1_IT_TC7);
    TIM4_Start();
    EXIT_IRQ();
}



void HalCom_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_ConfigurationHC();

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_Rx_dma_init();
    USART_Tx_dma_init();
    TIM4_Config();
    USART_Cmd(USART2, ENABLE);
}

void HalCom_Write(int8_t *send, int32_t len)
{
    COMM_OUT_ENABLE;
    delay(72 * 500);
    memcpy(usart_tx_buf, send, len);

    DMA_SetCurrDataCounter(DMA1_Channel7, len);
    DMA_ClearFlag(DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel7, ENABLE);
    tx_busy  = 1;
}

int8_t HalCom_TxBusy(void)
{
    return tx_busy;
}

int8_t HalCom_WaitTx(int32_t timeout)
{
    do {
        if (tx_busy == 0) return 0;
        delay(72);
    } while (--timeout > 0);

    return -1;
}

static int32_t RdPtr_Rx = 0;
int32_t HalCom_GetSize(void)
{
    int32_t fullness;
    int32_t WrPtr_Rx = 0;

    WrPtr_Rx = (int32_t)((uint32_t)RX_SIZE - ((uint32_t)(DMA1_Channel6 -> CNDTR)));
    fullness = WrPtr_Rx - RdPtr_Rx;

    if (fullness < 0) fullness += RX_SIZE ;
    return fullness;
}

void HalCom_ResetRx(void)
{
    int32_t WrPtr_Rx = 0;
    WrPtr_Rx = (int32_t)((uint32_t)RX_SIZE - ((uint32_t)(DMA1_Channel6 -> CNDTR)));
    RdPtr_Rx = WrPtr_Rx ;
}

int32_t HalCom_Read(int8_t *buf, int32_t size)
{
    int32_t fullness;
    int32_t WrPtr_Rx = 0;
    int32_t rlen = 0;
    int8_t *rxbuf;

    if (size <= 0) return 0;
    rxbuf = (int8_t *) usart_rx_buf;

    WrPtr_Rx = (int32_t)((uint32_t)RX_SIZE - ((uint32_t)(DMA1_Channel6 -> CNDTR)));
    fullness = WrPtr_Rx - RdPtr_Rx;
    if (fullness == 0) return 0;

    if (fullness < 0) fullness += RX_SIZE ;

    if (size > fullness)  rlen  = fullness;
    else rlen = size;

    if (RdPtr_Rx + rlen < RX_SIZE) {
        memcpy(buf, &rxbuf[RdPtr_Rx], rlen);
        RdPtr_Rx += rlen;
    } else {
        int32_t len1, len2;
        len1 = RX_SIZE - RdPtr_Rx;
        len2 = rlen - len1;
        memcpy(buf, rxbuf + RdPtr_Rx, len1);
        if (len2)
            memcpy(buf + len1, rxbuf, len2);
        RdPtr_Rx = len2;
    }
    return rlen;

}

#endif

