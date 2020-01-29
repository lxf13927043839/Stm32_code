#ifndef __TIM3_H

#define __TIM3_H
#include "stm32f4xx.h"
#include "sys.h"
#include "systick.h"

void TIM3_init(u16 arr,u16 psc);
void TIM4_init(u16 arr,u16 psc);
void TIM2_init(u32 arr,u16 psc);

#endif

//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
//	{
//		LED1=!LED1;//测试完成后需要注释
//		
//	}
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
//}


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//如果使用到延时函数就要初始化系统时钟，否则会卡死在循环中，无关delay.c，使用的是systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	LED_init();
//	//定时器时钟是84M 
//	//分频系数是8400，所以 84M/8400=10KHZ的计数频率，计数5000次为500ms
//  // 1/10 000 * 5000 = 0.5s = 500ms
//  TIM3_init(5000-1,8400-1);
//	
//	 while(1)
//	 {

//	 }
//}

