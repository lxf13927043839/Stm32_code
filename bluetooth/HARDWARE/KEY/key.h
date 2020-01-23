#ifndef __KEY_H

#define __KEY_H

#include "stm32f4xx.h"
#include "systick.h"	
#include "sys.h" 

void KEY_init(void);

int key_scanf(int mode);

#define key0 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9)
#define key1 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)
#define key2 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7)
#define key3 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)


#endif
