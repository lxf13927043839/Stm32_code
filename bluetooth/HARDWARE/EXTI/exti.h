#ifndef __EXTI_H

#define __EXTI_H

void EXTI_init(void);

#endif

////按键3 的中断处理函数
//void EXTI9_5_IRQHandler(void)
//{
//	//key3
//	if(RESET != EXTI_GetITStatus(EXTI_Line6))
//	{
//		delay_ms(10);	//去抖动
//		if(key3==0)	 
//		{
//				 printf("key3 test by interrupt\n");
//		}		
//		//清楚中断标志位，否则会一直进入中断函数
//		
//		LED0=!LED0;
//		
//		EXTI_ClearITPendingBit(EXTI_Line6);
//	}
//}

//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//如果使用到延时函数就要初始化系统时钟，否则会卡死在循环中，无关delay.c，使用的是systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	LED_init();
//	EXTI_init();
//	 while(1)
//	 {

//	 }
//}
 

