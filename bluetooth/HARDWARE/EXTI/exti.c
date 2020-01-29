#include "exti.h"
#include "key.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
//˵��
//�ĸ������ֱ�ΪPF 9 8 7 6
//��ʼ�������ĸ���
//������io�����ж��ߵ�ӳ���ϵ
//��ʼ���ж��ߣ����ô���������ʹ�ܵȵ�
//�����жϷ��鲢ʹ��
//��д�жϺ���������жϱ�־λ
//ע�⣺Ҫ���������������жϷ���======NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
//���Ƶ�ʱ�򰴼�û�иĶ������²�������


void EXTI_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//�����ڵĳ�ʼ��
	KEY_init();
	
	//ʱ�ӵĳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
	//���ŵ�ӳ�� ,���ܹ�д��һ������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF,EXTI_PinSource6);
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF,EXTI_PinSource9);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF,EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF,EXTI_PinSource7);
	
	//|EXTI_Line9|EXTI_Line8|EXTI_Line7
	EXTI_InitStruct.EXTI_Line=EXTI_Line6|EXTI_Line8|EXTI_Line7;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; 
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	
	
	//�ж���9-5
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
	//��ռ���ȼ� 3
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;
	//��Ӧ���ȼ� 2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}
