/*********************************************************************
Copyright (c) 2017 by inodes.
All rights reserved. You are not allowed to
copy or distribute the code without permission.
*********************************************************************/
#include "inodes_include.h"

#if 0 //this file is NOT used

#define MAX_COM_MGR_LEN (256)
#define MAX_RECV_BUF_LEN (512)
#define MAX_RECV_DATA_MASK (0x1ff)
#define MAX_COM_COUNT (3)

/*********************************************************************
���ļ���ʱ����������ڣ�
com1  232�ӿ�2
com2  GSMģ��
com3  232�ӿ�1
com4  485�ӿ�2
com5  485�ӿ�1
*********************************************************************/

/*******************************************************************************
* ��̬��������
*******************************************************************************/
static void nextai_com_rcc_configuration(unsigned char comID);
static void nextai_com_gpio_configuration(unsigned char comID);
static void nextai_com_nvic_configuration(unsigned char comID);
static unsigned char nextai_com_reg_port(unsigned char comID);
static unsigned char nextai_com_unreg_port(unsigned char comID);

static unsigned char com2_recv_buf[MAX_RECV_BUF_LEN] __attribute__((at(0x20001800)));
volatile unsigned int com2_gloabl_p = 0;
volatile unsigned int com2_read_p = 0;

//static unsigned char com1_recv_buf[MAX_RECV_BUF_LEN] = {0};
static unsigned char com1_recv_buf[MAX_RECV_BUF_LEN] __attribute__((at(0x20001a00)));
int com1_gloabl_p = 0;
int com1_read_p = 0;

typedef struct _NEXTAI_CHANNEL_INFO_ {
    unsigned char local_com_id;
    unsigned char used_flag;
} NEXTAI_CHANNEL_INFO;


/*******************************************************************************
* local_channel_data  : ͨ�����ݽṹ
* data_recv_flag      : �Ƿ�������flag��1��ʾ
*******************************************************************************/
typedef struct _NEXTAI_MGR_INFO_ {
    NEXTAI_CHANNEL_INFO local_channel_data[MAX_COM_COUNT];
    unsigned char data_recv_flag;
} NEXTAI_MGR_INFO;

static unsigned char inodes_local_com_mgr[MAX_COM_MGR_LEN] __attribute__((at(0x20001c00)));

static NEXTAI_MGR_INFO *local_com_mgr_p = 0;

/*******************************************************************************
* ������  : nextai_com_init
* ��  ��  : �������ݽṹ����
* ��  ��  : None
* ��  ��  : None
* ��  ��  : 0���ɹ�  -1: ʧ��
*******************************************************************************/
char nextai_com_init(void)
{
    local_com_mgr_p = (NEXTAI_MGR_INFO *)&inodes_local_com_mgr[0];
    memset(local_com_mgr_p, 0, MAX_COM_MGR_LEN);

    return 0;
}


/*******************************************************************************
* ������  : nextai_com_destroy
* ��  ��  : ���ڹ���ϵͳ������
* ��  ��  : None
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
void nextai_com_destroy(void)
{
    NEXTAI_MGR_INFO *me = local_com_mgr_p;
    unsigned char i = 0;

    if (0 != me) {
        for (i = 0; i < MAX_COM_COUNT; i++) {
            if (me->local_channel_data[i].used_flag) {
                nextai_com_close(me->local_channel_data[i].local_com_id);
            }
        }
    }
    local_com_mgr_p = 0;
}


/*******************************************************************************
* ������  : nextai_com_reg_port
* ��  ��  : ��ϵͳ��ע��һ������
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : 0:success   1:fail
*******************************************************************************/
static unsigned char nextai_com_reg_port(unsigned char comID)
{
    NEXTAI_MGR_INFO *me = local_com_mgr_p;
    unsigned char i = 0;

    if (0 == me)
        return 0xff;

    i = comID - NEXTAI_COM1_ID;

    if (me->local_channel_data[i].used_flag) {
        return 0xff;
    }

    me->local_channel_data[i].local_com_id = comID;
    me->local_channel_data[i].used_flag = 1;

    return 0;

}


/*******************************************************************************
* ������  : nextai_com_unreg_port
* ��  ��  : ��ϵͳ��ע��һ������
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : 0:success   1:fail
*******************************************************************************/
static unsigned char nextai_com_unreg_port(unsigned char comID)
{
    NEXTAI_MGR_INFO *me = local_com_mgr_p;
    unsigned char i = 0;

    if (0 == me)
        return 0xff;

    if ((comID > NEXTAI_COM3_ID) || (comID < NEXTAI_COM1_ID))return 0xff;

    i = comID - NEXTAI_COM1_ID;

    me->local_channel_data[i].local_com_id = 0;
    me->local_channel_data[i].used_flag = 0;

    return 0;

}

/*******************************************************************************
* ������  : nextai_com_rcc_configuration
* ��  ��  : ���ø�����ͬ�˿ڵ�ʱ��
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
static void nextai_com_rcc_configuration(unsigned char comID)
{
    /* Enable USART clock */

    if (comID == NEXTAI_COM1_ID) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    } else if (comID == NEXTAI_COM2_ID) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    } else if (comID == NEXTAI_COM3_ID) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    }
}

/*******************************************************************************
* ������  : nextai_com_gpio_configuration
* ��  ��  : ���ø�����ͬIO�˿�
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
static void nextai_com_gpio_configuration(unsigned char comID)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure USART Tx and Rx as alternate function push-pull */
    if (comID == NEXTAI_COM1_ID) {
        GPIO_StructInit(&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (comID == NEXTAI_COM2_ID) {
        GPIO_StructInit(&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        GPIO_StructInit(&GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

        GPIO_Init(GPIOD, &GPIO_InitStructure);
    } else if (comID == NEXTAI_COM3_ID) {
        GPIO_StructInit(&GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_StructInit(&GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;

        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}


void USART1_IRQHandler(void)
{
    NEXTAI_MGR_INFO *me = local_com_mgr_p;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        com1_recv_buf[com1_gloabl_p++] = USART_ReceiveData(USART1);
        com1_gloabl_p &= MAX_RECV_DATA_MASK;
    }

    me->data_recv_flag |= 0x01;
}

void USART2_IRQHandler(void)
{
    NEXTAI_MGR_INFO *me = local_com_mgr_p;

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        com2_recv_buf[com2_gloabl_p++] = USART_ReceiveData(USART2);
        com2_gloabl_p &= MAX_RECV_DATA_MASK;
    }

    me->data_recv_flag |= 0x02;

}



/*******************************************************************************
* ������  : nextai_com_nvic_configuration
* ��  ��  : ���ò�ͬ���ڵ��жϺ��������ȼ���ͬ
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
static void nextai_com_nvic_configuration(unsigned char comID)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


    if (comID == NEXTAI_COM1_ID) {
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;

        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    } else if (comID == NEXTAI_COM2_ID) {
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;

        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    } else if (comID == NEXTAI_COM3_ID) {
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

}


/*******************************************************************************
* ������  : nextai_com_open
* ��  ��  : ��һ���ض��Ĵ���
* ��  ��  : comName:  �����������������
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
unsigned char nextai_com_open(unsigned char *comName, inodes_com_param *com_info)
{
    USART_InitTypeDef USART_InitStructure;
    char comID = 0;

    if (strcmp(NEXTAI_COM1_NAME, (const char *)comName) == 0) {
        comID = NEXTAI_COM1_ID;
    } else if (strcmp(NEXTAI_COM2_NAME, (const char *)comName) == 0) {
        comID = NEXTAI_COM2_ID;  //�д����
        GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    } else if (strcmp(NEXTAI_COM3_NAME, (const char *)comName) == 0) {
        return 0xff;
    }

    if (0xff == nextai_com_reg_port(comID))
        return 0xff;



    /* Configure the USARTx */

    /* System Clocks Configuration */
    nextai_com_rcc_configuration(comID);


    /* Configure the GPIO ports */
    nextai_com_gpio_configuration(comID);


    if (0 == com_info) {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_Parity = USART_Parity_No;
    } else {
        USART_InitStructure.USART_WordLength = com_info->inodes_wordlength;
        USART_InitStructure.USART_Parity = com_info->inodes_parity;
    }

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


    if (comID == NEXTAI_COM1_ID) {
        if (0 == com_info) {
            USART_InitStructure.USART_BaudRate = 9600;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;
        } else {
            USART_InitStructure.USART_BaudRate = com_info->inodes_baudrate;
            USART_InitStructure.USART_StopBits = com_info->inodes_stopbits;
        }
        USART_Init(USART1, &USART_InitStructure);

    } else if (comID == NEXTAI_COM2_ID) {
        //USART_ClockInit(USART2, &USART_ClockInitStructure);
        USART_InitStructure.USART_BaudRate = 115200;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_Init(USART2, &USART_InitStructure);

    } else if (comID == NEXTAI_COM3_ID) {

    }

    nextai_com_nvic_configuration(comID);

    if (comID == NEXTAI_COM1_ID) {
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_Cmd(USART1, ENABLE);
    } else if (comID == NEXTAI_COM2_ID) {
        USART_Cmd(USART2, ENABLE);
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    } else if (comID == NEXTAI_COM3_ID) {
    }

    return comID;
}



/*******************************************************************************
* ������  : nextai_com_close
* ��  ��  : �ر�һ���ض��Ĵ���
* ��  ��  : comID:  ���ڱ�ʶ��
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
void nextai_com_close(unsigned char comID)
{
    if (comID == NEXTAI_COM1_ID) {
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        USART_Cmd(USART1, DISABLE);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

        memset(com1_recv_buf, 0, MAX_RECV_BUF_LEN);
        com1_gloabl_p = 0;
        com1_read_p = 0;
    } else if (comID == NEXTAI_COM2_ID) {
        USART_Cmd(USART2, DISABLE);
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
    } else if (comID == NEXTAI_COM3_ID) {

    }

    nextai_com_unreg_port(comID);
}


/*******************************************************************************
* ������  : nextai_com_send
* ��  ��  : ����ĳ���˿ڵ�����
* ��  ��  : comID:  ���ڱ�ʶ��
            databuf: �������ݻ�����
            datalen: �������ݳ���
* ��  ��  : None
* ��  ��  : None
*******************************************************************************/
char nextai_com_send(unsigned char comID, unsigned char *databuf, unsigned int datalen)
{
    int i = 0;
    unsigned char nextai_timer = 0;

    if (comID == NEXTAI_COM1_ID) {
        for (i = 0; i < datalen; i++) {
            //485 ����ӳ�ʱ���
#if 1
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
                nextai_timer++;
                if (nextai_timer >> 8)
                    return 0;
            }
#endif
            USART_SendData(USART1, databuf[i]);
        }
    } else if (comID == NEXTAI_COM2_ID) {
        GPIO_SetBits(GPIOD, GPIO_Pin_7);
        for (i = 0; i < datalen; i++) {
            //232
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) {
                nextai_timer++;
                if (nextai_timer >> 8)
                    return 0;
            }

            USART_SendData(USART2, databuf[i]);
        }
        for (i = 0; i < 10000; i++);
        GPIO_ResetBits(GPIOD, GPIO_Pin_7);

    } else if (comID == NEXTAI_COM3_ID) {

    }

    return datalen;
}


/*******************************************************************************
* ������  : nextai_com_recv
* ��  ��  : ����һ�����ȵ�����
* ��  ��  : comID:  ���ڱ�ʶ��
            databuf: �������ݻ�����
            datalen: ��Ҫ�������ݳ���
* ��  ��  : None
* ��  ��  : ʵ�ʽ��յ������ݳ���
*******************************************************************************/
unsigned int nextai_com_recv(unsigned char comID, unsigned char *databuf, unsigned int datalen)
{
    unsigned int i = 0, nextai_indx = 0, j = 0;
    NEXTAI_MGR_INFO *me = local_com_mgr_p;

    if ((comID > NEXTAI_COM5_ID) || (comID < NEXTAI_COM1_ID))return 0xffffffff;
    nextai_indx = comID - NEXTAI_COM1_ID;

    if (me->local_channel_data[nextai_indx].used_flag == 0) {
        nextai_dprintf("using flag null\r\n");
        return 0xffffffff;
    }

    if (!(me->data_recv_flag & (1 << nextai_indx))) {
        nextai_dprintf("recv flag null\r\n");
        return 0xffffffff;
    }

    if (comID == NEXTAI_COM2_ID) {
        unsigned short write_p = 0, read_p = 0;
        write_p = com2_gloabl_p;
        read_p = com2_read_p;

        i = 0;
        while (1) {
            if (read_p != write_p) {
                databuf[i++] = com2_recv_buf[read_p];
                read_p++;
                read_p &= MAX_RECV_DATA_MASK;

                if (i >= datalen)break;
            } else {
                me->data_recv_flag = me->data_recv_flag & (~(1 << nextai_indx));
                break;
            }
            //i &= MAX_RECV_DATA_MASK;
            //for(j = 0; j < 1000; j++);
        }

        com2_read_p = read_p;

        return i;
    }

    if (comID == NEXTAI_COM1_ID) {
        i = 0;
        while (1) {
            if (com1_read_p == com1_gloabl_p)
                break;
            databuf[i] = com1_recv_buf[com1_read_p];
            com1_read_p++;
            com1_read_p &= MAX_RECV_DATA_MASK;
            i++;
            for (j = 0; j < 1000; j++);
        }
        me->data_recv_flag = me->data_recv_flag & (~(1 << nextai_indx));
    }

    return i;
}


/*******************************************************************************
* ������  : nextai_com_getevent
* ��  ��  : ��ȡ����ϵͳ�������¼�
* ��  ��  : None
* ��  ��  : ����¼��������¼����ͣ��¼����ƣ��¼�����
* ��  ��  : 0:���¼�   1:���¼�
*******************************************************************************/
char nextai_com_getevent(unsigned char comID)
{
    unsigned char i = 0;
    NEXTAI_MGR_INFO *me = local_com_mgr_p;
    unsigned short nextai_timer = 0;

    if (0 == me)
        return 0;

    {
        i = comID - NEXTAI_COM1_ID;

        if (me->data_recv_flag & (1 << i)) {
            nextai_timer = 0;
            switch (comID) {
                case NEXTAI_COM1_ID: {
                        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {
                            nextai_timer++;
                            if (nextai_timer >> 3)
                                return 0;
                        }
                        break;
                    }
                case NEXTAI_COM2_ID: {
                        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) {
                            nextai_timer++;
                            if (nextai_timer >> 3)
                                return 0;
                        }
                        break;
                    }
                case NEXTAI_COM3_ID: {
                        break;
                    }
            }

            return 1;
        }
    }

    return 0;
}

#endif

