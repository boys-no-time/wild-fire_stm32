#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/9/17
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��PB1Ϊ���.��ʹ��ʱ��	    
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOB_CLK_ENABLE();           	//����GPIOBʱ��
		__HAL_RCC_GPIOD_CLK_ENABLE();           	//����GPIOEʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0 | GPIO_PIN_1; 				//PB0-1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          	//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
		
		GPIO_Initure.Pin=GPIO_PIN_6;
		HAL_GPIO_Init(GPIOD,&GPIO_Initure);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);
	
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0 | GPIO_PIN_1,GPIO_PIN_SET);	//PB5��1��Ĭ�ϳ�ʼ�������
	

}
