#ifndef __IWDG_H
#define __IWDG_H

#include "stm32f4xx.h"
void IWDG_init(u8 prer,u16 rlr);

void feed_dog(void);

#endif

//int main(void)
//{
//	int key_state=0;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
////���ʹ�õ���ʱ������Ҫ��ʼ��ϵͳʱ�ӣ�����Ῠ����ѭ���У��޹�delay.c��ʹ�õ���systick.c
//	Systick_init(168);
//	UART1_init(115200); 
//	
//	//��Ҫ���ó�ʼ������
//	LED_init();
//	KEY_init();

//  //ʱ����㣺((4*2^4))/32000=0.2ms ����һ��0.2ms 500�ξ���1s
//	IWDG_init(4,1000);
//	//����ʱ��ʹ�Ƶ���˸����Ƚ�����
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
