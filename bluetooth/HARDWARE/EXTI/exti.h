#ifndef __EXTI_H

#define __EXTI_H

void EXTI_init(void);

#endif

////����3 ���жϴ�����
//void EXTI9_5_IRQHandler(void)
//{
//	//key3
//	if(RESET != EXTI_GetITStatus(EXTI_Line6))
//	{
//		delay_ms(10);	//ȥ����
//		if(key3==0)	 
//		{
//				 printf("key3 test by interrupt\n");
//		}		
//		//����жϱ�־λ�������һֱ�����жϺ���
//		
//		LED0=!LED0;
//		
//		EXTI_ClearITPendingBit(EXTI_Line6);
//	}
//}

//int main(void)
//{ 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//���ʹ�õ���ʱ������Ҫ��ʼ��ϵͳʱ�ӣ�����Ῠ����ѭ���У��޹�delay.c��ʹ�õ���systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	LED_init();
//	EXTI_init();
//	 while(1)
//	 {

//	 }
//}
 

