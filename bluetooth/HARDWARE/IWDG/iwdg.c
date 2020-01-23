#include "iwdg.h"

//stm32f4�Ķ������Ź����ڲ�ר�ŵ�32khz����ʱ�ӣ�LSI������
//��һ���ڲ���RCʱ�ӣ����Բ�����ʮ��׼ȷ��32k hz


//prer:��Ƶϵ��0-7ֻ�е�3λ��Ч
//rlr һ��ʼ�ĳ�ʼֵ��0xFFF ֧�֣�0-0xFFF�� ��11λ��Ч
//��Ƶϵ������=4*2^prer  ���ֵλ256
//ʱ�����Ĺ�ʽ T=((4*2^prer)*rlr)/(32K)  (ms)   

void IWDG_init(u8 prer,u16 rlr)
{
	
	 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	 IWDG_SetPrescaler(prer);
	 IWDG_SetReload(rlr);
	 IWDG_ReloadCounter();
	
	 IWDG_Enable();
}

void feed_dog(void)
{
		IWDG_ReloadCounter();
}
