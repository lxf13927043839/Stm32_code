#include "tim3.h"
#include "led.h"

//通用定时器3中断初始化
//arr:自动重装值，psc：时钟预分频系数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率
void TIM3_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //使能TIM3时钟
  TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// 初始化 TIM3
	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// 初始化NVIC
	
	
	TIM_Cmd(TIM3,ENABLE); //使能3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3中断
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}


void TIM4_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //使能TIM4时钟
  TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);// 初始化 TIM4
	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// 初始化NVIC
	
	
	TIM_Cmd(TIM4,ENABLE); //使能4
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器4中断
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
}

void TIM2_init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //使能TIM2时钟
  TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);// 初始化 TIM2
	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //响应优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);// 初始化NVIC
	
	
	TIM_Cmd(TIM2,ENABLE); //使能2
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2中断
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}
