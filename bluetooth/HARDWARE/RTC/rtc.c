#include "rtc.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "24cxx.h"
#include "myiic.h"


RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
u8 My_RTC_init(void)
{
	RTC_InitTypeDef RTC_InitStructure;

	
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
		RTC_TimeStruct.RTC_Hours = 4;
		RTC_TimeStruct.RTC_Minutes = 58;
		RTC_TimeStruct.RTC_Seconds = 50;
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		//设置日期
		RTC_DateStruct.RTC_Date = 0x16;
		RTC_DateStruct.RTC_Month = 0x02;
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Saturday;
		RTC_DateStruct.RTC_Year = 0x20;
		
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5030);//标记已经初始化过了
	}
	return 0;
}

//设置闹钟时间（按日期闹铃，24小时设置）

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


/*
	一秒执行一次，用来在00：00-06：00夜晚蜂鸣器警报
*/
u8 Zero_to_six_clock=0;//1代表0-6时刻来临，0：未来到

//记得解开注释，调试wifi的时候注释了


extern u8 enter_set_time;
extern u8 curtain_status;//当前窗帘的状态
extern u8 has_set_time_online;
extern u8 set_time_buff[];
void open_rotate(void);//电机转动打开窗帘
void close_rotate(void);



void LCD_showtime_RTC(void)
{
	static char first=0;
	u8 had_set;
	u8 curtain_flag;
	//-------------------------------------------------------------------------------------
	int x=0,y=0;
	RTC_TimeTypeDef RTC_TimeStruct;
	u8 temp_buff[40]; 
	
	//显示时间
	x=142;y=295;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)temp_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	//printf("%s\n",temp_buff);
	if(RTC_TimeStruct.RTC_Hours>=0&&RTC_TimeStruct.RTC_Hours<=5)//00：00
	{
		Zero_to_six_clock=1;
		//printf("zero to six is arrival\n");
	}
  else
	{
		Zero_to_six_clock=0;
		//printf("six is arrival\n");
	}
	if(enter_set_time==0)
	LCD_DisplayString(x,y,24,temp_buff); //显示一个12/16/24字体字符串
	
	//-----------------判断是否有设置了定时-------

	//考虑重启情况
	if(first==0)
	{
		AT24CXX_Read(4,&had_set,1);
		if(had_set==1||had_set==0)
		{
			has_set_time_online=1;
			AT24CXX_Read(5,set_time_buff,18);
			//printf("set_time_buff = %s\n",set_time_buff);
		}
		first=1;
	}
	//实时在线的情况
	if(has_set_time_online==1)
	{
		sprintf((char*)temp_buff,"20%02d-%02d-%02d%02d:%02d:%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date
		,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
		//printf("temp_buff = %s \n",temp_buff);
		if(strcmp((char *)set_time_buff,(char *)temp_buff)==0)
		{
			AT24CXX_Read(4,&curtain_flag,1);
			if(curtain_flag==1)//定时的窗帘是开还是关 1 开 0 关
			{
				if(curtain_status==0)
				{
					printf("open operation in settime \n");
				//add open operation
//=================================================================
					open_rotate();
					curtain_status=1;
					LCD_DisplayChinese_one(140,143,24,24);
					
				}
				
				
			}
			else if(curtain_flag==0)
			{
				if(curtain_status==1)
				{
					printf("close operation \n");
					//add close operation		
//====================================================================
					
					close_rotate();
					curtain_status=0;
					LCD_DisplayChinese_one(140,143,25,24);
				}				
			}
			curtain_flag=3;
			AT24CXX_Write(4,&curtain_flag,1);
			has_set_time_online=0;
		}
	}
	
}



//RTC wake up中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP 中断
	{
		RTC_ClearFlag(RTC_FLAG_WUTF); //
		LCD_showtime_RTC();		
		LED0=!LED0;
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//
}
//add----copy
u8 const month_amendBuf[12]={0,3,3,6,1,4,6,2,5,0,3,5};
/****************************************************************************
* 名    称: u8 RTC_GetWeek(u16 wyear,u8 wmonth,u8 wday)
* 功    能：获得某天是星期几
* 入口参数：wyear：年(最大99)  wmonth：月  wday：日
* 返回参数：星期几
* 说    明：从2000~2099有效
            返回值1-7依次对应星期一到星期天  
            该函数用于设置时间时，无需输入星期几，取时间星期几时无需使用该函数，
            因直接读取日期寄存器的星期值就行
            注意形参“年”是8位，即要输入2位的年的参数  15：合法    2015：不合法
****************************************************************************/																						 
u8 RTC_GetWeek(u8 wyear,u8 wmonth,u8 wday)
{	
	u16 middata;
	u8 yearL;
	
	yearL=wyear+100; 	 //从2000年开始，加100

	middata=yearL+yearL/4;
	middata=middata%7; 
	middata=middata+wday+month_amendBuf[wmonth-1];
	if (yearL%4==0&&wmonth<3)middata--;
	
	if((middata%7==0)) return 7;       //与STM32F103的rtc例程不同，该函数星期天返回值为7
	return(middata%7);
}	
/****************************************************************************
* 名    称: ErrorStatus RTC_SetTimes(u8 year,u8 month,u8 date,u8 hour,u8 min,u8 sec)
* 功    能：设置RTC时间
* 入口参数：年月日时分秒
* 返回参数：成功与否  1：成功   0：失败
* 说    明：注意形参“年”是8位，即要输入4位的年的参数  15：合法    2015：不合法    
****************************************************************************/	
ErrorStatus RTC_SetTimes(u8 year,u8 month,u8 date,u8 hour,u8 min,u8 sec)
{
  RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=RTC_GetWeek(year,month,date);
	//RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;

	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	
	if(hour>12)
	     RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_PM;
	else RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	return ( RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure) && RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure) );
}
/****************************************************************************
* 名    称: void RTC_GetTimes(uint32_t RTC_Format)
* 功    能：读取RTC时间
* 入口参数：RTC_Format：读取时间数据的数据类型  RTC_Format_BIN：字节型  RTC_Format_BCD：BCD码数据类型
* 返回参数：无
* 说    明：     
****************************************************************************/
void RTC_GetTimes(uint32_t RTC_Format)
{
		RTC_GetDate(RTC_Format,&RTC_DateStruct);
	  RTC_GetTime(RTC_Format,&RTC_TimeStruct);
}


