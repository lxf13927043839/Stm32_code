#include "rtc.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "led.h"


u8 My_RTC_init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	u16 retry=0X1FFF;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE); //使能后备寄存器访问
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5030)//是否第一次配置。�
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) 
		//检查制定的RCC标志位设置与否，等待低速晶振就绪
		{ 
			retry++;
			delay_ms(10);
		}
		if(retry==0)return 1; //LSE 开启失败
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //选择LSE 作为RTC 时钟

		RCC_RTCCLKCmd(ENABLE); //使能RTC 时钟
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC 异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;//RTC 同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24 小时格式
		RTC_Init(&RTC_InitStructure);
		
		//3.设置时间
		//RTC_TimeStruct.RTC_H12 =  
		RTC_TimeStruct.RTC_Hours = 8;
		RTC_TimeStruct.RTC_Minutes = 20;
		RTC_TimeStruct.RTC_Seconds = 50;
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		//设置日期
		RTC_DateStruct.RTC_Date = 0x20;
		RTC_DateStruct.RTC_Month = 0x06;
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Tuesday;
		RTC_DateStruct.RTC_Year = 0x19;
		
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5030);//标记已经初始化过了
	}
	return 0;
}

//设置闹钟时间（按星期闹铃，24小时设置）

void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟 A
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;//小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//秒
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel
	=RTC_AlarmDateWeekDaySel_WeekDay;//按星期闹
	
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
	//精确匹配星期、时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC 闹钟 A 标志
	EXTI_ClearITPendingBit(EXTI_Line17);//清除 LINE17 上的中断标志位 
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启闹钟 A 中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启 A
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE17
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//周期性唤醒定时器设置
//cnt自动重装载值，见到0 产生中断
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);// 关闭WAKE UP
	RTC_WakeUpClockConfig(wksel);//唤醒时钟选择
	
	
	RTC_SetWakeUpCounter(cnt);//设置WAKE UP 重装载值
	RTC_ClearITPendingBit(RTC_IT_WUT); //清除 RTC WAKE UP 标志
	EXTI_ClearITPendingBit(EXTI_Line22);//清除 LINE22 标志
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启 WAKE UP 定时器中断
	RTC_WakeUpCmd( ENABLE);//开启 WAKE UP 定时器
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能 LINE22
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);//
}

//闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A 中断
	{ 
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断
		printf("ALARM A!\r\n");
		LED2=!LED2; 
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17); //清除中断线17的标志位
}

//RTC wake up中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP 中断
	{
		RTC_ClearFlag(RTC_FLAG_WUTF); //
		LED0=!LED0;
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//
}







