#include "beep.h"
//蜂鸣器的初始化，一开始是不响的


void BEEP_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);//外设时钟的初始化

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	//io口的初始化
	GPIO_Init(GPIOG,  &GPIO_InitStruct);
	
	//让蜂鸣器初始化为不响的状态，给低电平
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_7);
	
}
