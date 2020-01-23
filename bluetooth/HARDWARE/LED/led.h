#ifndef H_Led_H
#define H_Led_H
#include "stm32f4xx.h"

void LED_init(void);


//LED端口定义
#define LED0 PEout(3)	// D0
#define LED1 PEout(4)	// D1	 
#define LED2 PGout(9)	// D1	 


#endif  


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//如果使用到延时函数就要初始化系统时钟，否则会卡死在循环中，无关delay.c，使用的是systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	
//	LED_init();
//	BEEP_init();
//	 while(1)
//	 {
//			LED0=1;
//		  LED1=1;
//		  LED2=1;
//		  BEEP=0;
//			//延时1秒要采用500ms叠加的方式
//			delay_ms(500);
//		  delay_ms(500);
//		  LED0=0;
//		  LED1=0;
//		  LED2=0;
//		  BEEP=1;
//		 	delay_ms(500);
//		  delay_ms(500);
//	 }
//}
