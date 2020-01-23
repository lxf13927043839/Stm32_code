#ifndef __RTC_H

#define __RTC_H
#include "stm32f4xx.h"
#include "sys.h"
#include "systick.h"

u8 My_RTC_init(void);
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);
void RTC_Alarm_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void RTC_Set_WakeUp(u32 wksel,u16 cnt);

#endif

/*
	���һ��ʼ�ĳ�ʼ����Ҫ��rtc.c�е�My_RTC_init��
	3.����ʱ��
	  RTC_TimeStruct.RTC_H12 =  
		RTC_TimeStruct.RTC_Hours = 9;
		RTC_TimeStruct.RTC_Minutes = 20;
		RTC_TimeStruct.RTC_Seconds = 30;
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		//????
		RTC_DateStruct.RTC_Date = 0x31;
		RTC_DateStruct.RTC_Month = 0x05;
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Tuesday;
		RTC_DateStruct.RTC_Year = 0x19;
		�������ã�ͬһ�����ӵ�5050Ҫ�޸ģ����ܸ��³ɹ�
*/


//int main(void)
//{ 
//	RTC_TimeTypeDef RTC_TimeStruct;
//	RTC_DateTypeDef RTC_DateStruct;
//	u8 tbuf[40];
//	u8 t=0;
//	int count=0;
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	Systick_init(168);
//	UART1_init(115200); 

//	KEY_init(); 
//	My_RTC_init();
//	
//	
//	
//	//����������ѡ����ck_spre��ʱ��Ƶ�ʣ���1HZ��Ƶ��
//	//���и�0 �����1S
//	//�ɿ��Ĵ���������,10x��ѡ����ck_spre��ʱ��Ƶ��
//	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP ?????
//	
//	
//	
//	while(1)
//	{
//		t++;
//		if((t%10)==0) //? 100ms ????????
//		{
//			
//			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
//			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,
//			RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
//			printf("%s\n",tbuf);

//		
//			sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.RTC_WeekDay);
//			printf("%s\n",tbuf);
//		
//		
//			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
//			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,
//			RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
//			printf("%s\n",tbuf);
//		
//			printf("\n");
//			count=RTC_TimeStruct.RTC_Seconds;

//		}
//		//if((t%20)==0)LED0=!LED0; //
//		delay_ms(100);
//		
//		if(count%5==0)
//		{
//			RTC_Set_AlarmA(RTC_DateStruct.RTC_WeekDay,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,count+1);
//		}
//	 }
//}




