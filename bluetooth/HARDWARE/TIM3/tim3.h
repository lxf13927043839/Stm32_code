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
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
//	{
//		LED1=!LED1;//������ɺ���Ҫע��
//		
//	}
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
//}


//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//���ʹ�õ���ʱ������Ҫ��ʼ��ϵͳʱ�ӣ�����Ῠ����ѭ���У��޹�delay.c��ʹ�õ���systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	LED_init();
//	//��ʱ��ʱ����84M 
//	//��Ƶϵ����8400������ 84M/8400=10KHZ�ļ���Ƶ�ʣ�����5000��Ϊ500ms
//  // 1/10 000 * 5000 = 0.5s = 500ms
//  TIM3_init(5000-1,8400-1);
//	
//	 while(1)
//	 {

//	 }
//}

