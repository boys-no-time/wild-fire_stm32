
#ifndef _HAL_COM_H_
#define _HAL_COM_H_

#include "stm32f10x.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

void HalCom_Init(void);
void HalCom_Write(int8_t *send, int32_t len);
int8_t HalCom_WaitTx(int32_t timeout);
int32_t HalCom_GetSize(void);
void HalCom_ResetRx(void);
int32_t HalCom_Read(int8_t *buf, int32_t size);
int8_t HalCom_TxBusy(void);


#endif


