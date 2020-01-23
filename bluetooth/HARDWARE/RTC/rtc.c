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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//Ê¹ÄÜPWRÊ±ÖÓ
	PWR_BackupAccessCmd(ENABLE); //Ê¹ÄÜºó±¸¼Ä´æÆ÷·ÃÎÊ
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5030)//ÊÇ·ñµÚÒ»´ÎÅäÖÃ¡£¿
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ¿ªÆô
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) 
		//¼ì²éÖÆ¶¨µÄRCC±êÖ¾Î»ÉèÖÃÓë·ñ£¬µÈ´ıµÍËÙ¾§Õñ¾ÍĞ÷
		{ 
			retry++;
			delay_ms(10);
		}
		if(retry==0)return 1; //LSE ¿ªÆôÊ§°Ü
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //Ñ¡ÔñLSE ×÷ÎªRTC Ê±ÖÓ

		RCC_RTCCLKCmd(ENABLE); //Ê¹ÄÜRTC Ê±ÖÓ
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC Òì²½·ÖÆµÏµÊı(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;//RTC Í¬²½·ÖÆµÏµÊı(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24 Ğ¡Ê±¸ñÊ½
		RTC_Init(&RTC_InitStructure);
		
		//3.ÉèÖÃÊ±¼ä
		//RTC_TimeStruct.RTC_H12 =  
		RTC_TimeStruct.RTC_Hours = 8;
		RTC_TimeStruct.RTC_Minutes = 20;
		RTC_TimeStruct.RTC_Seconds = 50;
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		//ÉèÖÃÈÕÆÚ
		RTC_DateStruct.RTC_Date = 0x20;
		RTC_DateStruct.RTC_Month = 0x06;
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Tuesday;
		RTC_DateStruct.RTC_Year = 0x19;
		
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5030);//±ê¼ÇÒÑ¾­³õÊ¼»¯¹ıÁË
	}
	return 0;
}

//ÉèÖÃÄÖÖÓÊ±¼ä£¨°´ĞÇÆÚÄÖÁå£¬24Ğ¡Ê±ÉèÖÃ£©

void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//¹Ø±ÕÄÖÖÓ A
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;//Ğ¡Ê±
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//·ÖÖÓ
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//Ãë
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//ĞÇÆÚ
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel
	=RTC_AlarmDateWeekDaySel_WeekDay;//°´ĞÇÆÚÄÖ
	
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
	//¾«È·Æ¥ÅäĞÇÆÚ¡¢Ê±·ÖÃë
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//Çå³ıRTC ÄÖÖÓ A ±êÖ¾
	EXTI_ClearITPendingBit(EXTI_Line17);//Çå³ı LINE17 ÉÏµÄÖĞ¶Ï±êÖ¾Î» 
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//¿ªÆôÄÖÖÓ A ÖĞ¶Ï
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//¿ªÆô A
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//ÖĞ¶ÏÊÂ¼ş
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //ÉÏÉıÑØ´¥·¢
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//Ê¹ÄÜLINE17
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//ÇÀÕ¼ÓÅÏÈ¼¶1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//ÏìÓ¦ÓÅÏÈ¼¶ 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//ÖÜÆÚĞÔ»½ĞÑ¶¨Ê±Æ÷ÉèÖÃ
//cnt×Ô¶¯ÖØ×°ÔØÖµ£¬¼ûµ½0 ²úÉúÖĞ¶Ï
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);// ¹Ø±ÕWAKE UP
	RTC_WakeUpClockConfig(wksel);//»½ĞÑÊ±ÖÓÑ¡Ôñ
	
	
	RTC_SetWakeUpCounter(cnt);//ÉèÖÃWAKE UP ÖØ×°ÔØÖµ
	RTC_ClearITPendingBit(RTC_IT_WUT); //Çå³ı RTC WAKE UP ±êÖ¾
	EXTI_ClearITPendingBit(EXTI_Line22);//Çå³ı LINE22 ±êÖ¾
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//¿ªÆô WAKE UP ¶¨Ê±Æ÷ÖĞ¶Ï
	RTC_WakeUpCmd( ENABLE);//¿ªÆô WAKE UP ¶¨Ê±Æ÷
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//ÖĞ¶ÏÊÂ¼ş
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //ÉÏÉıÑØ
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//Ê¹ÄÜ LINE22
	EXTI_Init(&EXTI_InitStructure);//ÅäÖÃ
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);//
}

//ÄÖÖÓÖĞ¶Ï·şÎñº¯Êı
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A ÖĞ¶Ï
	{ 
		RTC_ClearFlag(RTC_FLAG_ALRAF);//Çå³ıÖĞ¶Ï
		printf("ALARM A!\r\n");
		LED2=!LED2; 
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17); //Çå³ıÖĞ¶ÏÏß17µÄ±êÖ¾Î»
}

//RTC wake upÖĞ¶Ï·şÎñº¯Êı
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP ÖĞ¶Ï
	{
		RTC_ClearFlag(RTC_FLAG_WUTF); //
		LED0=!LED0;
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//
}







