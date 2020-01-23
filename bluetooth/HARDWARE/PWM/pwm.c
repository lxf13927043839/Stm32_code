#include "pwm.h"

void PWM_TIM14_init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	
	//先定时器、io口使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);//TIM14 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //F口
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GF9复用成TIM14
	
	//初始化GPIOF  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; //GPIOF9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; //
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; //50MHz
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	//定时器初始化
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//定时器分分频
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=arr;//自动重装值
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStruct);
	
	//pwm 频道1模式

	TIM_OCInitStruct.TIM_OCMode= TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High; //高极性 
	// 高极性=====低于arr是低电平 大于arr是高电平
	// 低极性=====低于arr是高电平 大于arr是低电平
	TIM_OC1Init(TIM14, &TIM_OCInitStruct); 
	
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); //预装载
	TIM_ARRPreloadConfig(TIM14,ENABLE);//APRE使能
	//TIM_Cmd(TIM14, ENABLE); //使能工作
	
}
