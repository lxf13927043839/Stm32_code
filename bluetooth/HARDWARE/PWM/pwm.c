#include "pwm.h"

void PWM_TIM14_init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	
	//�ȶ�ʱ����io��ʹ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);//TIM14 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //F��
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GF9���ó�TIM14
	
	//��ʼ��GPIOF  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; //GPIOF9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; //
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //50MHz
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	//��ʱ����ʼ��
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//��ʱ���ַ�Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=arr;//�Զ���װֵ
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStruct);
	
	//pwm Ƶ��1ģʽ

	TIM_OCInitStruct.TIM_OCMode= TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High; //�߼��� 
	// �߼���=====����arr�ǵ͵�ƽ ����arr�Ǹߵ�ƽ
	// �ͼ���=====����arr�Ǹߵ�ƽ ����arr�ǵ͵�ƽ
	TIM_OC1Init(TIM14, &TIM_OCInitStruct); 
	
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); //Ԥװ��
	TIM_ARRPreloadConfig(TIM14,ENABLE);//APREʹ��
	//TIM_Cmd(TIM14, ENABLE); //ʹ�ܹ���
	
}
