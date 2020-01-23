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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE); //ʹ�ܺ󱸼Ĵ�������
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5030)//�Ƿ��һ�����á��
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) 
		//����ƶ���RCC��־λ������񣬵ȴ����پ������
		{ 
			retry++;
			delay_ms(10);
		}
		if(retry==0)return 1; //LSE ����ʧ��
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //ѡ��LSE ��ΪRTC ʱ��

		RCC_RTCCLKCmd(ENABLE); //ʹ��RTC ʱ��
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC �첽��Ƶϵ��(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;//RTC ͬ����Ƶϵ��(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24 Сʱ��ʽ
		RTC_Init(&RTC_InitStructure);
		
		//3.����ʱ��
		//RTC_TimeStruct.RTC_H12 =  
		RTC_TimeStruct.RTC_Hours = 8;
		RTC_TimeStruct.RTC_Minutes = 20;
		RTC_TimeStruct.RTC_Seconds = 50;
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
		
		//��������
		RTC_DateStruct.RTC_Date = 0x20;
		RTC_DateStruct.RTC_Month = 0x06;
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Tuesday;
		RTC_DateStruct.RTC_Year = 0x19;
		
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
		
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5030);//����Ѿ���ʼ������
	}
	return 0;
}

//��������ʱ�䣨���������壬24Сʱ���ã�

void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//�ر����� A
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;//Сʱ
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//����
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//��
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;//����
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel
	=RTC_AlarmDateWeekDaySel_WeekDay;//��������
	
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
	//��ȷƥ�����ڡ�ʱ����
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//���RTC ���� A ��־
	EXTI_ClearITPendingBit(EXTI_Line17);//��� LINE17 �ϵ��жϱ�־λ 
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//�������� A �ж�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//���� A
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE17
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//�����Ի��Ѷ�ʱ������
//cnt�Զ���װ��ֵ������0 �����ж�
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);// �ر�WAKE UP
	RTC_WakeUpClockConfig(wksel);//����ʱ��ѡ��
	
	
	RTC_SetWakeUpCounter(cnt);//����WAKE UP ��װ��ֵ
	RTC_ClearITPendingBit(RTC_IT_WUT); //��� RTC WAKE UP ��־
	EXTI_ClearITPendingBit(EXTI_Line22);//��� LINE22 ��־
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//���� WAKE UP ��ʱ���ж�
	RTC_WakeUpCmd( ENABLE);//���� WAKE UP ��ʱ��
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ�� LINE22
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);//
}

//�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A �ж�
	{ 
		RTC_ClearFlag(RTC_FLAG_ALRAF);//����ж�
		printf("ALARM A!\r\n");
		LED2=!LED2; 
		
	}
	EXTI_ClearITPendingBit(EXTI_Line17); //����ж���17�ı�־λ
}

//RTC wake up�жϷ�����
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP �ж�
	{
		RTC_ClearFlag(RTC_FLAG_WUTF); //
		LED0=!LED0;
	}
	EXTI_ClearITPendingBit(EXTI_Line22);//
}







