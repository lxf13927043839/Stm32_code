#ifndef H_Led_H
#define H_Led_H
#include "stm32f4xx.h"

void LED_init(void);


//LED�˿ڶ���
#define LED0 PEout(3)	// D0
#define LED1 PEout(4)	// D1	 
#define LED2 PGout(9)	// D1	 


#endif  


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//���ʹ�õ���ʱ������Ҫ��ʼ��ϵͳʱ�ӣ�����Ῠ����ѭ���У��޹�delay.c��ʹ�õ���systick.c
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
//			//��ʱ1��Ҫ����500ms���ӵķ�ʽ
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
