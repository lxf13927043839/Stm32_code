#include "people.h"


void People_init(void)//PC12
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
		GPIO_Init(GPIOC, &GPIO_InitStructure);
}
int People_scan(void)
{
	 if(PEOPLE==1)
	 {
			delay_ms(10);
			if(PEOPLE==1)
			{
				return 1;
			}
	 }
	 return 0;
}
