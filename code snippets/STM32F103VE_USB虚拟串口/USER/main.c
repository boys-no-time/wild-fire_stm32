#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "usbd_ctlreq.h"
#include "hw_config.h"
#include "usbd_core.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern PCD_HandleTypeDef hpcd_USB_FS;

int main(void)
{
    u16 t;
    u16 len;
    u16 times=0;
    u8 usbstatus=0;

    HAL_Init();                    	 	//��ʼ��HAL��
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M��9��Ƶ 8Mhzx9 = 72Mhz
    delay_init(72);               		//��ʼ����ʱ����
    uart_init(115200);					//��ʼ������
    LED_Init();							//��ʼ��LED

    printf("USB Connecting...\r\n");//��ʾUSB��ʼ����
	
    USB_Reset();//USB�Ͽ�������
    MX_USB_DEVICE_Init();//USB�豸��ʼ��
	
    printf("USB inited...\r\n");
    while(1)
    {
        if(usbstatus!=USB_GetStatus())
        {
            usbstatus = USB_GetStatus();//��¼�µ�״̬
            if(usbstatus==USBD_STATE_CONFIGURED)
            {               
                printf("USB Connected    \r\n");//��ʾUSB���ӳɹ�
                LED1=0;//DS1��
            } else
            {
                printf("USB disConnected \r\n");//��ʾUSB�Ͽ�
                LED1=1;//DS1��
            }
        }
        if(USB_USART_RX_STA&0x8000)
        {
            len=USB_USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
            USB_Printf("\r\n�����͵���Ϣ����Ϊ:%d\r\n",len);
            for(t=0; t<len; t++)
            {
                USB_USART_SendData(&USB_USART_RX_BUF[t]);//�ַ�����
            }
            USB_Printf("\r\n");//���뻻��
            USB_USART_RX_STA=0;
        } else
        {
            times++;
            if(times%5000==0)
            {
                USB_Printf("\r\nս��STM32������USB���⴮��ʵ��\r\n");
            }
            if(times%200==0) {
                USB_Printf("����������,�Իس�������\r\n");
            }
            if(times%30==0)LED0=!LED0;//��˸LED,��ʾϵͳ��������.
            delay_ms(10);
        }
    }
}

