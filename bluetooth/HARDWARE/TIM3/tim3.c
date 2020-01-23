#include "tim3.h"
#include "led.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr:�Զ���װֵ��psc��ʱ��Ԥ��Ƶϵ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��
void TIM3_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //ʹ��TIM3ʱ��
  TIM_TimeBaseInitStructure.TIM_Period = arr; //�Զ���װֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// ��ʼ�� TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //��Ӧ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// ��ʼ��NVIC
	TIM_Cmd(TIM3,ENABLE); //ʹ��3
}

