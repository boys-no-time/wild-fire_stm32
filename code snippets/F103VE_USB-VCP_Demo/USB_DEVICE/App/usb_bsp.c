#include "usb_bsp.h"
#include "usbd_def.h"
#include "usbd_ctlreq.h"
#include "usbd_core.h"

#include "usbd_cdc_if.h"
#include "usart.h"  
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	
#include "stm32f1xx_hal_gpio.h"



//USB���⴮��������ò�����VCP��������usbd_cdc_if.c�е�CDC_Control_FS��ʵ��
USBD_CDC_LineCodingTypeDef LineCoding= 
{
    115200,     //������
    0x00,       //ֹͣλ,Ĭ��1λ
    0x00,       //У��λ,Ĭ����
    0x08        //����λ,Ĭ��8λ
};
uint8_t  USART_PRINTF_Buffer[USB_USART_REC_LEN];	//usb_printf���ͻ�����
uint8_t USB_USART_RX_BUF[USB_USART_REC_LEN]; 	//USB���յ����ݻ����������USART_REC_LEN���ֽ�

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
uint16_t USB_USART_RX_STA=0;       				//����״̬���	 
extern USBD_HandleTypeDef hUsbDeviceFS;

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
	*Error_Handler for usb
    *��������ʱ�Ļص��������û�����������������Ϣ
  */

//USB����͹���ģʽ,��USB����suspendģʽʱ,MCU����͹���ģʽ
//��������ӵ͹��Ĵ���(�����ʱ�ӵ�),��usbd_conf.c�ļ��е�HAL_PCD_SuspendCallback�е���
void Enter_LowPowerMode(void)
{
 	printf("usb enter low power mode\r\n");
} 

/*@brief �ϵ������USB�����ߣ�������ʶ��Ϊ�Ͽ���
 * ��ʱһ��ʱ�����ͷ�ȥ��ʼ���������˸�λ����Ҫ��������ߡ�
 */
void USB_Reset()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
 
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
 
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		HAL_Delay(10);//delay 10 ms
}
/*
	��ȡ�豸״̬�����ص�ǰ���豸״̬
	USBD_STATE_DEFAULT                              0x01U
	USBD_STATE_ADDRESSED                            0x02U
	USBD_STATE_CONFIGURED                           0x03U
	USBD_STATE_SUSPENDED                            0x04U
*/
uint8_t USB_GetStatus()
{
	return hUsbDeviceFS.dev_state;//�����豸״̬
}

//�����ƴ���1�������ݵķ���,������USB���⴮�ڽ��յ�������.
//�����USB���⴮�ڽ��յ�������
//databuffer:���ݻ�����
//Nb_bytes:���յ����ֽ���.
//recv data
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint32_t Nb_bytes)
{ 
	uint8_t i;
	uint8_t res;
	for(i=0;i<Nb_bytes;i++)
	{  
		res=data_buffer[i]; 
		if((USB_USART_RX_STA&0x8000)==0)		//����δ���
		{
			if(USB_USART_RX_STA&0x4000)			//���յ���0x0d
			{
				if(res!=0x0a)USB_USART_RX_STA=0;//���մ���,���¿�ʼ
				else {
					USB_USART_RX_STA|=0x8000;	//��������� 
				}
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USB_USART_RX_STA|=0x4000;
				else
				{
					USB_USART_RX_BUF[USB_USART_RX_STA&0X3FFF]=res;
					USB_USART_RX_STA++;
					if(USB_USART_RX_STA>(USB_USART_REC_LEN-1))USB_USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	
				}				
			}
		}   
	}  
} 

//����һ���ֽ����ݵ�USB���⴮��
void USB_USART_SendData(uint8_t *data)
{
	//�ȴ����ݷ�������ٷ�����һ���ֽ�
	while((CDC_Transmit_FS(data,sizeof(*data)))==USBD_BUSY);
}

//usb���⴮�ڵ�printf����p
//ȷ��һ�η������ݲ���USB_USART_REC_LEN�ֽ�
void USB_Printf(char* fmt,...)  
{  
	uint16_t i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	len=strlen((const char*)USART_PRINTF_Buffer);//�˴η������ݵĳ���
	for(i=0;i<len;i++)
	{
		USB_USART_SendData(&USART_PRINTF_Buffer[i]);
	}
	//CDC_Transmit_FS(USART_PRINTF_Buffer,len);
} 

