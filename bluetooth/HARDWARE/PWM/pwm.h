#ifndef __PWM_H

#include "stm32f4xx.h"
#define __PWM_H

//arr���Զ���װֵ��psc:ʱ��Ԥ��Ƶϵ��
void PWM_TIM14_init(u32 arr,u32 psc);

#endif


//PWM�Ĳ���

//int main(void)
//{
//	//int states=-1;
//	
//	u16 count=0;
//	int flag=0;
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
//	delay_init(168);
//	 

//	//��Ҫ���ó�ʼ������
//	//LED_init();
//	//BEEP_init();
//	//KEY_init();
//	//EXTIX_init();
//	
//	//ע:��ʱ����ʱ����84M ����Ϊ1M
//	//��װֵ��500,pwmƵ����2k
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
//		//�޸�ռ�ձ�
//		TIM_SetCompare1(TIM14,count);
//		
//		//TIM_SetCompare1(TIM1,count);
//	}
//}
