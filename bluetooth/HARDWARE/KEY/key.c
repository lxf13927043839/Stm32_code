#include "key.h"
/*
	key0 1 2 3 对应了 PF 9 8 7 6 

*/

void KEY_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	//按键PF9 8 7 6 的初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}

int key_scanf(int mode)
{
	
	static int key_mode=1;//1支持长按 0 不支持 相当于是一个标志位
	if(mode) key_mode=1;
	if(key_mode&&((key0==0)||(key1==0)||(key2==0)||(key3==0)))
	{
		delay_ms(10);
		if(key0==0)
		{
			key_mode=0;
			return 1;
			
		}else if(key1==0)
		{
			key_mode=0;
			return 2;
			
		}else if(key2==0)
		{
			key_mode=0;
			return 3;
		}else if(key3==0)
		{
			key_mode=0;
			return 4;
		}
		
	//没有按键时，要置位一下，才能按第一次	
	}else if((key0==1)&&(key1==1)&&(key2==1)&&(key3==1))
	{
		key_mode=1;
	}
	return 0;
}
