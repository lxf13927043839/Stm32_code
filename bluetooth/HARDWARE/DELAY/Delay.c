#include"Delay.h"

void Delay_us(u32 i)	 //�򵥵���ʱ����
{
	uint32_t ret;
	count=i;
	ret=SysTick_Config(168);//ʱ���趨1us��������	

	while(count !=0);
	SysTick->CTRL  &=~SysTick_CTRL_ENABLE_Msk;//ʱ�ӹر�
}


void Delay_ms(u32 i)	
 {
	uint32_t ret;
	count=i;
	ret=SysTick_Config(168*1000);//ʱ���趨1ms��������	
	while(count !=0);
	SysTick->CTRL  &=~SysTick_CTRL_ENABLE_Msk;//ʱ�ӹر�
 }
