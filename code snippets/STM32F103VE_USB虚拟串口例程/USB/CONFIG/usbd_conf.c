//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//usbd_conf ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_cdc.h"

#include "hw_config.h"

PCD_HandleTypeDef hpcd_USB_FS;
void Error_Handler(void);

static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);

void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state);

/* MSP Init */
//��ʼ��PCD MSP
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
  if(pcdHandle->Instance==USB)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  }
}

/* MSP Init */
//DeInit PCD MSP��ע��PCD MSP
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
  if(pcdHandle->Instance==USB)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USB_CLK_DISABLE();

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  }
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
                       USBD LL PCD�����ص�����(PCD->USB Device Library)
*******************************************************************************/

//USBD���ý׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t *)hpcd->Setup);
}

//USBD OUT�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

//USBD IN�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

//USBD SOF�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

//USBD ��λ�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{ 
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  if ( hpcd->Init.speed != PCD_SPEED_FULL)
  {
    Error_Handler();
  }
    /* Set Speed. */
  USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);

  /* Reset Device. */
  USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

//USBD ����ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)//suspend�ص�
{
  /* Inform USB library that core enters in suspend Mode. */
  USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
  /* Enter in STOP mode. */
  /* USER CODE BEGIN 2 */
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
  //
  Enter_LowPowerMode();
  /* USER CODE END 2 */
}

//USBD �ָ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);
}

//USBD ISO OUT������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

//USBD ISO IN������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

//USBD ���ӳɹ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

//USBD �Ͽ����ӻص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
                       USBD LL �����ӿں���(USB Device Library --> PCD)
*******************************************************************************/

//USBD�ײ��ʼ������
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
  /* Init USB Ip. */
  /* Link the driver to the stack. */
	//��ʼ��USB IP�������������ӵ�Э��ջ
  hpcd_USB_FS.pData = pdev;
  pdev->pData = &hpcd_USB_FS;

  hpcd_USB_FS.Instance = USB;//ʹ��USBʵ��
  hpcd_USB_FS.Init.dev_endpoints = 8;//�˵���Ϊ8
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;//USBȫ��ģʽ
  hpcd_USB_FS.Init.low_power_enable = DISABLE;//��ʹ�õ͹���ģʽ
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;//��ʹ�õ�س��ģʽ
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)//PCD Init
  {
    Error_Handler( );
  }
  //�˵�����
  /* USER CODE BEGIN EndPoint_Configuration */
  HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData , 0x00 , PCD_SNG_BUF, 0x18);
  HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData , 0x80 , PCD_SNG_BUF, 0x58);
  /* USER CODE END EndPoint_Configuration */
  /* USER CODE BEGIN EndPoint_Configuration_CDC */
  HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData , 0x81 , PCD_SNG_BUF, 0xC0);
  HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData , 0x01 , PCD_SNG_BUF, 0x110);
  HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData , 0x82 , PCD_SNG_BUF, 0x100);
  /* USER CODE END EndPoint_Configuration_CDC */
  return USBD_OK;
}

/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_DeInit(pdev->pData);

  usb_status =  USBD_Get_USB_Status(hal_status);
 
  return usb_status; 
}

//USBD�ײ�������ʼ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
 
  hal_status = HAL_PCD_Start(pdev->pData);
     
  usb_status =  USBD_Get_USB_Status(hal_status);
  
  return usb_status;
}

//USBD�ײ�����ֹͣ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_Stop(pdev->pData);

  usb_status =  USBD_Get_USB_Status(hal_status);
  
  return usb_status;
}

//USBD��ʼ��(��)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵��
//ep_mps:�˵���������(�ֽ�)
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

  usb_status =  USBD_Get_USB_Status(hal_status);
 
  return usb_status;
}

//USBDȡ����ʼ��(�ر�)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);
      
  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status;  
}

//USBD���ĳ���˵������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);
      
  usb_status =  USBD_Get_USB_Status(hal_status);
  
  return usb_status;  
}

//USBD��ĳ���˵�����һ����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

  usb_status =  USBD_Get_USB_Status(hal_status);
 
  return usb_status;  
}

//USBDȡ��ĳ���˵����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);  
     
  usb_status =  USBD_Get_USB_Status(hal_status);

  return usb_status; 
}

//USBD�����Ƿ�����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����ͣ; 1,��ͣ.
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;
  
  if((ep_addr & 0x80) == 0x80)
  {
    return hpcd->IN_ep[ep_addr & 0x7F].is_stall; 
  }
  else
  {
    return hpcd->OUT_ep[ep_addr & 0x7F].is_stall; 
  }
}

//USBDΪ�豸ָ���µ�USB��ַ
//pdev:USBD���ָ��
//dev_addr:�µ��豸��ַ,USB1_OTG_HS/USB2_OTG_HS
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;
  
  hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);
     
  usb_status =  USBD_Get_USB_Status(hal_status);
 
  return usb_status;  
}

//USBDͨ���˵㷢������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���͵����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
     
  usb_status =  USBD_Get_USB_Status(hal_status);
  
  return usb_status;    
}

//USBD׼��һ���˵��������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���յ����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_StatusTypeDef usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
     
  usb_status =  USBD_Get_USB_Status(hal_status);
  	
  return usb_status; 
}

//USBD��ȡ���һ��������Ĵ�С
//pdev:USBD���ָ��
//ep_addr:�˵��  
//����ֵ:����С
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

//USBD ��ʱ����(��msΪ��λ)
//Delay:��ʱ��ms��.
void USBD_LL_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size)
{
  static uint32_t mem[(sizeof(USBD_CDC_HandleTypeDef)/4)+1];/* On 32-bit boundary */
  return mem;
}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{

}

/**
  * @brief Software Device Connection
  * @param hpcd: PCD handle
  * @param state: Connection state (0: disconnected / 1: connected)
  * @retval None
  */

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
static void PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state)
#else
void HAL_PCDEx_SetConnectionState(PCD_HandleTypeDef *hpcd, uint8_t state)
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  /* USER CODE BEGIN 6 */
  if (state == 1)
  {
    /* Configure Low connection state. */

  }
  else
  {
    /* Configure High connection state. */

  }
  /* USER CODE END 6 */
}

/**
  * @brief  Retuns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
  USBD_StatusTypeDef usb_status = USBD_OK;

  switch (hal_status)
  {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
