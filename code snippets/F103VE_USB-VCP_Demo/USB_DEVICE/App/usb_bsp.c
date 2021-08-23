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



//USB虚拟串口相关配置参数，VCP的设置在usbd_cdc_if.c中的CDC_Control_FS中实现
USBD_CDC_LineCodingTypeDef LineCoding= 
{
    115200,     //波特率
    0x00,       //停止位,默认1位
    0x00,       //校验位,默认无
    0x08        //数据位,默认8位
};
uint8_t  USART_PRINTF_Buffer[USB_USART_REC_LEN];	//usb_printf发送缓冲区
uint8_t USB_USART_RX_BUF[USB_USART_REC_LEN]; 	//USB接收的数据缓冲区，最大USART_REC_LEN个字节

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USB_USART_RX_STA=0;       				//接收状态标记	 
extern USBD_HandleTypeDef hUsbDeviceFS;

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
	*Error_Handler for usb
    *发生错误时的回调函数，用户可以在其加入调试信息
  */

//USB进入低功耗模式,当USB进入suspend模式时,MCU进入低功耗模式
//需自行添加低功耗代码(比如关时钟等),在usbd_conf.c文件中的HAL_PCD_SuspendCallback中调用
void Enter_LowPowerMode(void)
{
 	printf("usb enter low power mode\r\n");
} 

/*@brief 上电后拉低USB数据线，让主机识别为断开，
 * 延时一段时间再释放去初始化，避免了复位后需要插拔数据线。
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
	获取设备状态，返回当前的设备状态
	USBD_STATE_DEFAULT                              0x01U
	USBD_STATE_ADDRESSED                            0x02U
	USBD_STATE_CONFIGURED                           0x03U
	USBD_STATE_SUSPENDED                            0x04U
*/
uint8_t USB_GetStatus()
{
	return hUsbDeviceFS.dev_state;//返回设备状态
}

//用类似串口1接收数据的方法,来处理USB虚拟串口接收到的数据.
//处理从USB虚拟串口接收到的数据
//databuffer:数据缓存区
//Nb_bytes:接收到的字节数.
//recv data
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint32_t Nb_bytes)
{ 
	uint8_t i;
	uint8_t res;
	for(i=0;i<Nb_bytes;i++)
	{  
		res=data_buffer[i]; 
		if((USB_USART_RX_STA&0x8000)==0)		//接收未完成
		{
			if(USB_USART_RX_STA&0x4000)			//接收到了0x0d
			{
				if(res!=0x0a)USB_USART_RX_STA=0;//接收错误,重新开始
				else {
					USB_USART_RX_STA|=0x8000;	//接收完成了 
				}
			}else //还没收到0X0D
			{	
				if(res==0x0d)USB_USART_RX_STA|=0x4000;
				else
				{
					USB_USART_RX_BUF[USB_USART_RX_STA&0X3FFF]=res;
					USB_USART_RX_STA++;
					if(USB_USART_RX_STA>(USB_USART_REC_LEN-1))USB_USART_RX_STA=0;//接收数据错误,重新开始接收	
				}				
			}
		}   
	}  
} 

//发送一个字节数据到USB虚拟串口
void USB_USART_SendData(uint8_t *data)
{
	//等待数据发送完毕再发送下一个字节
	while((CDC_Transmit_FS(data,sizeof(*data)))==USBD_BUSY);
}

//usb虚拟串口的printf函数p
//确保一次发送数据不超USB_USART_REC_LEN字节
void USB_Printf(char* fmt,...)  
{  
	uint16_t i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	len=strlen((const char*)USART_PRINTF_Buffer);//此次发送数据的长度
	for(i=0;i<len;i++)
	{
		USB_USART_SendData(&USART_PRINTF_Buffer[i]);
	}
	//CDC_Transmit_FS(USART_PRINTF_Buffer,len);
} 

