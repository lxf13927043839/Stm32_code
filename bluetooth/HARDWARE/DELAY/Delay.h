#ifndef Delay_H
#define Delay_H
#include "stm32f4xx.h"

void Delay_us(u32 i);
void Delay_ms(u32 i);	 
extern  volatile  uint32_t count;	

#endif  
