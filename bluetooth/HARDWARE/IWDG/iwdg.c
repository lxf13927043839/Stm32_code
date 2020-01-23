#include "iwdg.h"

//stm32f4的独立看门狗由内部专门的32khz低速时钟（LSI）驱动
//是一个内部的RC时钟，所以并不是十分准确的32k hz


//prer:分频系数0-7只有低3位有效
//rlr 一开始的初始值是0xFFF 支持（0-0xFFF） 低11位有效
//分频系数因子=4*2^prer  最大值位256
//时间计算的公式 T=((4*2^prer)*rlr)/(32K)  (ms)   

void IWDG_init(u8 prer,u16 rlr)
{
	
	 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	 IWDG_SetPrescaler(prer);
	 IWDG_SetReload(rlr);
	 IWDG_ReloadCounter();
	
	 IWDG_Enable();
}

void feed_dog(void)
{
		IWDG_ReloadCounter();
}
