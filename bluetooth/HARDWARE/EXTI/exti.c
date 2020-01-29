#include "exti.h"
#include "key.h"
#include "beep.h"
#include "led.h"
#include "delay.h"
//说明
//四个按键分别为PF 9 8 7 6
//初始化了这四个口
//设置了io口与中断线的映射关系
//初始化中断线，设置触发条件，使能等等
//配置中断分组并使能
//编写中断函数，清除中断标志位
//注意：要在主函数中设置中断分组======NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//复制的时候按键没有改动，导致不能运行


void EXTI_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//按键口的初始化
	KEY_init();
	
	//时钟的初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
	//引脚的映射 ,不能够写在一起会出错
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

	
	
	//中断线9-5
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
	//抢占优先级 3
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x03;
	//响应优先级 2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}
