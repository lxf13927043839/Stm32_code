#ifndef __PWM_H

#include "stm32f4xx.h"
#define __PWM_H

//arr是自动重装值，psc:时钟预分频系数
void PWM_TIM14_init(u32 arr,u32 psc);

#endif


//PWM的测试

//int main(void)
//{
//	//int states=-1;
//	
//	u16 count=0;
//	int flag=0;
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	delay_init(168);
//	 

//	//需要调用初始化函数
//	//LED_init();
//	//BEEP_init();
//	//KEY_init();
//	//EXTIX_init();
//	
//	//注:定时器的时钟是84M 计数为1M
//	//重装值是500,pwm频率是2k
//	
//	PWM_TIM14_init(499,83);
//	
//	while(1)
//	{
//		delay_ms(10);
//		if(flag==0)
//		{
//			count+=3;
//		}
//		else
//		{
//			count-=3;
//		}
//		if(count>499)
//		{
//			flag=1;
//			//delay_ms(10);
//		}
//		if(count==0)
//			flag=0;
//		
//		//修改占空比
//		TIM_SetCompare1(TIM14,count);
//		
//		//TIM_SetCompare1(TIM1,count);
//	}
//}
