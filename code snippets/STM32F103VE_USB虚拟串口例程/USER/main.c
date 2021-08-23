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

    HAL_Init();                    	 	//初始化HAL库
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M，9倍频 8Mhzx9 = 72Mhz
    delay_init(72);               		//初始化延时函数
    uart_init(115200);					//初始化串口
    LED_Init();							//初始化LED

    printf("USB Connecting...\r\n");//提示USB开始连接
	
    USB_Reset();//USB断开再重连
    MX_USB_DEVICE_Init();//USB设备初始化
	
    printf("USB inited...\r\n");
    while(1)
    {
        if(usbstatus!=USB_GetStatus())
        {
            usbstatus = USB_GetStatus();//记录新的状态
            if(usbstatus==USBD_STATE_CONFIGURED)
            {               
                printf("USB Connected    \r\n");//提示USB连接成功
                LED1=0;//DS1亮
            } else
            {
                printf("USB disConnected \r\n");//提示USB断开
                LED1=1;//DS1灭
            }
        }
        if(USB_USART_RX_STA&0x8000)
        {
            len=USB_USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
            USB_Printf("\r\n您发送的消息长度为:%d\r\n",len);
            for(t=0; t<len; t++)
            {
                USB_USART_SendData(&USB_USART_RX_BUF[t]);//字符发送
            }
            USB_Printf("\r\n");//插入换行
            USB_USART_RX_STA=0;
        } else
        {
            times++;
            if(times%5000==0)
            {
                USB_Printf("\r\n战舰STM32开发板USB虚拟串口实验\r\n");
            }
            if(times%200==0) {
                USB_Printf("请输入数据,以回车键结束\r\n");
            }
            if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
            delay_ms(10);
        }
    }
}

