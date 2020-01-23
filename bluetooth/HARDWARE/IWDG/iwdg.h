#ifndef __IWDG_H
#define __IWDG_H

#include "stm32f4xx.h"
void IWDG_init(u8 prer,u16 rlr);

void feed_dog(void);

#endif

//int main(void)
//{
//	int key_state=0;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
////如果使用到延时函数就要初始化系统时钟，否则会卡死在循环中，无关delay.c，使用的是systick.c
//	Systick_init(168);
//	UART1_init(115200); 
//	
//	//需要调用初始化函数
//	LED_init();
//	KEY_init();

//  //时间计算：((4*2^4))/32000=0.2ms 计数一次0.2ms 500次就是1s
//	IWDG_init(4,1000);
//	//加延时是使灯的闪烁现象比较明显
//	delay_ms(1000);
//	LED0=0;
//	
//	while(1)
//	{
//		key_state=key_scanf(1);
//		if((key_state==1)||(key_state==2)||(key_state==3)||(key_state==4))
//		{
//			feed_dog();
//			
//			LED1=0;
//			delay_ms(100);
//			LED1=1;
//		}
//		
//	}
//}
