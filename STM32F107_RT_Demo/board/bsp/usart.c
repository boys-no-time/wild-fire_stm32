/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2010-03-29     Bernard      remove interrupt Tx and DMA Rx mode
 * 2013-05-13     aozima       update for kehong-lingtai.
 * 2015-01-31     armink       make sure the serial transmit complete in putc()
 */

#include "inodes_include.h"

/* USART1 */
#define UART1_GPIO_TX        GPIO_Pin_9
#define UART1_GPIO_RX        GPIO_Pin_10
#define UART1_GPIO           GPIOA

/* USART2 */
#if defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_CL)
#define UART2_GPIO_TX	    GPIO_Pin_5
#define UART2_GPIO_RX	    GPIO_Pin_6
#define UART2_GPIO        GPIOD

//#define RS485SEND() GPIO_SetBits(GPIOD, GPIO_Pin_7)
//#define RS485REVC() GPIO_ResetBits(GPIOD, GPIO_Pin_7)

#else /* for STM32F10X_HD */
/* USART2_REMAP = 0 */
#define UART2_GPIO_TX     GPIO_Pin_2
#define UART2_GPIO_RX     GPIO_Pin_3
#define UART2_GPIO        GPIOA
#endif

/* UART4 */
#define UART4_GPIO_TX        GPIO_Pin_10
#define UART4_GPIO_RX        GPIO_Pin_11
#define UART4_GPIO           GPIOC

/* STM32 uart driver */
struct stm32_uart {
    USART_TypeDef *uart_device;
    IRQn_Type irq;
};

static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart *uart;
    USART_InitTypeDef USART_InitStructure;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct stm32_uart *)serial->parent.user_data;

    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

    if (cfg->data_bits == DATA_BITS_8) {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    } else if (cfg->data_bits == DATA_BITS_9) {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    }

    if (cfg->stop_bits == STOP_BITS_1) {
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    } else if (cfg->stop_bits == STOP_BITS_2) {
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    }

    if (cfg->parity == PARITY_NONE) {
        USART_InitStructure.USART_Parity = USART_Parity_No;
    } else if (cfg->parity == PARITY_ODD) {
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
    } else if (cfg->parity == PARITY_EVEN) {
        USART_InitStructure.USART_Parity = USART_Parity_Even;
    }

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);
    //USART_ClearFlag(uart->uart_device,USART_FLAG_TC);
    /* Enable USART */
    USART_Cmd(uart->uart_device, ENABLE);

    return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart *uart;

    UNUSED(arg);
    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    switch (cmd) {
        /* disable interrupt */
        case RT_DEVICE_CTRL_CLR_INT:
            /* disable rx irq */
            UART_DISABLE_IRQ(uart->irq);
            /* disable interrupt */
            USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
            break;
        /* enable interrupt */
        case RT_DEVICE_CTRL_SET_INT:
            /* enable rx irq */
            UART_ENABLE_IRQ(uart->irq);
            /* enable interrupt */
            USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
            break;
    }

    return RT_EOK;
}

static int stm32_putc(struct rt_serial_device *serial, char c)
{
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    while (!(uart->uart_device->SR & USART_FLAG_TC));
    uart->uart_device->DR = c;
    while (!(uart->uart_device->SR & USART_FLAG_TC));

    return 1;
}

static int stm32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->uart_device->SR & USART_FLAG_RXNE) {
        ch = uart->uart_device->DR & 0xff;
    }

    return ch;
}

static rt_size_t stm32_dma_transmit(struct rt_serial_device *serial, rt_uint8_t *buf, rt_size_t size, int direction)
{
    UNUSED(serial);
    UNUSED(buf);
    UNUSED(size);
    UNUSED(direction);
	return 0;
}

static const struct rt_uart_ops stm32_uart_ops = {
    stm32_configure,
    stm32_control,
    stm32_putc,
    stm32_getc,
	stm32_dma_transmit
};

#if defined(RT_USING_UART1)
/* UART1 device driver structure */
struct stm32_uart uart1 = {
    USART1,
    USART1_IRQn,
};
struct rt_serial_device serial1;

void USART1_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart1;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
        rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }

    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
        stm32_getc(&serial1);
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
struct stm32_uart uart2 = {
    USART2,
    USART2_IRQn,
};
struct rt_serial_device serial2;
void USART2_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart2;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
        rt_hw_serial_isr(&serial2, RT_SERIAL_EVENT_RX_IND);
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
        stm32_getc(&serial2);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

//void RS485SendFun(rt_device_t *dev, rt_uint8_t *data, rt_uint16_t lengh)
//{
//    RS485SEND();
//    delay(5000*48); //500uS
//    rt_device_write(*dev, 0, data, lengh);
//    delay(10*48); //500uS
//    RS485REVC();
//}
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART4)
struct stm32_uart uart4 = {
    UART4,
    UART4_IRQn,
};
struct rt_serial_device serial4;
void UART4_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart4;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET) {
        rt_hw_serial_isr(&serial4, RT_SERIAL_EVENT_RX_IND);
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET) {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET) {
        stm32_getc(&serial4);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART4 */

static void RCC_ConfigurationU(void)
{

#if defined(RT_USING_UART1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
#if (defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_CL))
    /* Enable AFIO and GPIOD clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);
    /* Enable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#else
    /* Enable AFIO and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
#endif
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif /* RT_USING_UART2 */
	
#if defined(RT_USING_UART4)	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
#endif /* RT_USING_UART4 */	
}

static void GPIO_ConfigurationU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

#if defined(RT_USING_UART1)
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

//    //RS485�շ�����
//    GPIO_StructInit(&GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART4)
    /* Configure UART4 Rx (PC.11) as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_RX;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);

    /* Configure UART4 Tx (PC.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_TX;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART4 */
}

static void NVIC_ConfigurationU(struct stm32_uart *uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void rt_hw_usart_init(void)
{
    struct stm32_uart *uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    RCC_ConfigurationU();
    GPIO_ConfigurationU();

#if defined(RT_USING_UART1)
    uart = &uart1;
    config.baud_rate = BAUD_RATE_115200;
    serial1.ops    = &stm32_uart_ops;
    serial1.config = config;
    NVIC_ConfigurationU(&uart1);

    /* register UART1 device */
    rt_hw_serial_register(&serial1, "uart1",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX ,
                          uart);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    uart = &uart2;
    config.baud_rate = BAUD_RATE_115200;
    serial2.ops    = &stm32_uart_ops;
    serial2.config = config;
    NVIC_ConfigurationU(&uart2);
    /* register UART2 device */
    rt_hw_serial_register(&serial2, "uart2",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART4)
    uart = &uart4;
    config.baud_rate = BAUD_RATE_115200;
    serial4.ops    = &stm32_uart_ops;
    serial4.config = config;
    NVIC_ConfigurationU(&uart4);
    /* register UART2 device */
    rt_hw_serial_register(&serial4, "uart4",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
#endif /* RT_USING_UART2 */

}
